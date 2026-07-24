#include "espnow_task.h"
#include "uart_task.h"
#include "esp_now.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event.h"
#include "tcpip_adapter.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>

static const char *TAG = "ESPNOW_TASK";

static uint8_t broadcast_mac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// ESP-NOW Receive Callback
static void espnow_recv_cb(const uint8_t *mac_addr, const uint8_t *data, int len) {
    if (len < 2) return; // Need at least cmd + len
    
    // We can directly package it into a protocol message to send to STM32
    protocol_msg_t msg;
    msg.cmd = data[0];
    msg.len = data[1];
    if (msg.len > 0 && (len >= msg.len + 2)) {
        memcpy(msg.payload, &data[2], msg.len);
    }
    
    // Some commands need MAC address injected, like QUESTION_RECEIVED might just be payload
    // Let's create specific protocol frames for STM32
    protocol_msg_t out_msg;
    
    if (msg.cmd == CMD_HOST_FOUND || msg.cmd == CMD_JOIN_HOST || msg.cmd == CMD_SUBMIT_ANSWER) {
        out_msg.cmd = msg.cmd;
        out_msg.len = 6 + msg.len;
        memcpy(out_msg.payload, mac_addr, 6);
        if (msg.len > 0) {
            memcpy(out_msg.payload + 6, msg.payload, msg.len);
        }
        uart_task_send(&out_msg);
    } else {
        uart_task_send(&msg);
    }
}

// ESP-NOW Send Callback
static void espnow_send_cb(const uint8_t *mac_addr, esp_now_send_status_t status) {
    // Optionally handle send failure, but for ESP-NOW UDP-like behavior, we just log
    ESP_LOGD(TAG, "Send to " MACSTR ", status: %d", MAC2STR(mac_addr), status);
}

static void espnow_main_thread(void *arg) {
    protocol_msg_t *msg;
    
    while (1) {
        if (xQueueReceive(uart_rx_queue, &msg, portMAX_DELAY) == pdTRUE) {
            
            // Reconstruct payload for ESP-NOW
            // ESP-NOW max payload is 250 bytes. We send [CMD, LEN, PAYLOAD...]
            uint8_t espnow_data[250];
            espnow_data[0] = msg->cmd;
            espnow_data[1] = msg->len;
            
            int data_len = 2;
            
            if (msg->cmd == CMD_JOIN_HOST || msg->cmd == CMD_SEND_FEEDBACK) {
                // These commands contain MAC in the payload. We extract it to know who to send to.
                uint8_t target_mac[6];
                memcpy(target_mac, msg->payload, 6);
                
                // Add peer if not exists
                if (!esp_now_is_peer_exist(target_mac)) {
                    esp_now_peer_info_t peerInfo = {};
                    memcpy(peerInfo.peer_addr, target_mac, 6);
                    peerInfo.channel = 1;
                    peerInfo.encrypt = false;
                    esp_now_add_peer(&peerInfo);
                }
                
                // For SEND_FEEDBACK, we skip MAC in the ESP-NOW payload
                if (msg->cmd == CMD_SEND_FEEDBACK) {
                    espnow_data[1] = msg->len - 6; // Adjust len
                    if (msg->len > 6) {
                        memcpy(&espnow_data[2], &msg->payload[6], msg->len - 6);
                        data_len += (msg->len - 6);
                    }
                } else {
                    // For JOIN_HOST, maybe just send the command to the host
                    espnow_data[1] = 0; 
                }
                esp_now_send(target_mac, espnow_data, data_len);
                
            } else if (msg->cmd == CMD_BROADCAST_QUESTION || msg->cmd == CMD_SCAN_HOSTS) {
                if (msg->len > 0) {
                    memcpy(&espnow_data[2], msg->payload, msg->len);
                    data_len += msg->len;
                }
                esp_now_send(broadcast_mac, espnow_data, data_len);
            } else {
                // General broadcast or unicast logic for other commands
                if (msg->len > 0) {
                    memcpy(&espnow_data[2], msg->payload, msg->len);
                    data_len += msg->len;
                }
                esp_now_send(broadcast_mac, espnow_data, data_len);
            }
            
            free(msg);
        }
    }
}

void espnow_task_init(void) {
    tcpip_adapter_init();
    ESP_ERROR_CHECK(esp_event_loop_init(NULL, NULL));
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK( esp_wifi_start());
    ESP_ERROR_CHECK( esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE));

    ESP_ERROR_CHECK( esp_now_init() );
    esp_now_register_recv_cb(espnow_recv_cb);
    esp_now_register_send_cb(espnow_send_cb);

    // Add broadcast peer
    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, broadcast_mac, 6);
    peerInfo.channel = 1;
    peerInfo.encrypt = false;
    esp_now_add_peer(&peerInfo);

    xTaskCreate(espnow_main_thread, "espnow_thread", 3072, NULL, 10, NULL);
}
