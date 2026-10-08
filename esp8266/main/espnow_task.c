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
#include <stdbool.h>

static const char *TAG = "ESPNOW_TASK";

static uint8_t broadcast_mac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// Tracks the reliability of each unicast peer (question targets on the master,
// the joined host on a slave) so a consistently-failing link can be dropped.
#define MAX_TRACKED_PEERS   20
#define PEER_MISS_THRESHOLD 3

typedef struct {
    uint8_t mac[6];
    uint8_t miss_count;
    bool active;
} peer_link_t;

static peer_link_t tracked_peers[MAX_TRACKED_PEERS];
static int tracked_peers_count = 0;

// The host a slave has joined; only slaves use this.
static uint8_t joined_host_mac[6];
static bool has_joined_host = false;

static peer_link_t *find_tracked_peer(const uint8_t *mac) {
    for (int i = 0; i < tracked_peers_count; i++) {
        if (tracked_peers[i].active && memcmp(tracked_peers[i].mac, mac, 6) == 0) {
            return &tracked_peers[i];
        }
    }
    return NULL;
}

// Starts tracking a peer's link quality, adding it as an ESP-NOW peer if needed.
static peer_link_t *track_peer(const uint8_t *mac) {
    peer_link_t *p = find_tracked_peer(mac);
    if (p != NULL) return p;
    if (tracked_peers_count >= MAX_TRACKED_PEERS) return NULL;

    if (!esp_now_is_peer_exist(mac)) {
        esp_now_peer_info_t peerInfo = {};
        memcpy(peerInfo.peer_addr, mac, 6);
        peerInfo.channel = 1;
        peerInfo.encrypt = false;
        esp_now_add_peer(&peerInfo);
    }

    p = &tracked_peers[tracked_peers_count++];
    memcpy(p->mac, mac, 6);
    p->miss_count = 0;
    p->active = true;
    return p;
}

// Makes a MAC reachable by unicast without tracking its link quality.
static void ensure_peer(const uint8_t *mac) {
    if (!esp_now_is_peer_exist(mac)) {
        esp_now_peer_info_t peerInfo = {};
        memcpy(peerInfo.peer_addr, mac, 6);
        peerInfo.channel = 1;
        peerInfo.encrypt = false;
        esp_now_add_peer(&peerInfo);
    }
}

// Removes a peer whose link has gone bad and tells the STM32 so it can update the game/UI.
static void drop_peer(const uint8_t *mac) {
    peer_link_t *p = find_tracked_peer(mac);
    if (p != NULL) p->active = false;

    if (esp_now_is_peer_exist(mac)) {
        esp_now_del_peer(mac);
    }

    ESP_LOGW(TAG, "Dropping weak peer " MACSTR, MAC2STR(mac));

    protocol_msg_t drop_msg;
    drop_msg.cmd = CMD_PLAYER_DROPPED;
    drop_msg.len = 6;
    memcpy(drop_msg.payload, mac, 6);
    uart_task_send(&drop_msg);
}

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

    // A slave just joined us: start tracking its link so we can unicast
    // questions to it (with delivery ACKs) and detect a weak connection.
    if (msg.cmd == CMD_JOIN_HOST) {
        track_peer(mac_addr);
    }

    // Some commands need MAC address injected, like QUESTION_RECEIVED might just be payload
    // Let's create specific protocol frames for STM32
    protocol_msg_t out_msg;

    if (msg.cmd == CMD_SCAN_HOSTS || msg.cmd == CMD_HOST_FOUND || msg.cmd == CMD_JOIN_HOST || msg.cmd == CMD_SUBMIT_ANSWER) {
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

// ESP-NOW Send Callback: this is our delivery ACK for unicast sends.
static void espnow_send_cb(const uint8_t *mac_addr, esp_now_send_status_t status) {
    peer_link_t *p = find_tracked_peer(mac_addr);
    if (p == NULL) {
        // Untracked target: a broadcast, or a board we only answered a scan for.
        // Free the scanner's peer slot again, the peer table is small.
        if (memcmp(mac_addr, broadcast_mac, 6) != 0 && esp_now_is_peer_exist(mac_addr)) {
            esp_now_del_peer(mac_addr);
        }
        return;
    }

    if (status == ESP_NOW_SEND_SUCCESS) {
        p->miss_count = 0;
        return;
    }

    p->miss_count++;
    ESP_LOGW(TAG, "Send miss #%d to " MACSTR, p->miss_count, MAC2STR(mac_addr));
    if (p->miss_count >= PEER_MISS_THRESHOLD) {
        drop_peer(mac_addr);
    }
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
            
            if (msg->cmd == CMD_JOIN_HOST || msg->cmd == CMD_SEND_FEEDBACK || msg->cmd == CMD_JOIN_ACK) {
                // These commands contain MAC in the payload. We extract it to know who to send to.
                uint8_t target_mac[6];
                memcpy(target_mac, msg->payload, 6);

                // Track link quality for this peer (adds it as an ESP-NOW peer if needed).
                track_peer(target_mac);

                // For SEND_FEEDBACK and JOIN_ACK (player ID), we skip MAC in the ESP-NOW payload
                if (msg->cmd == CMD_SEND_FEEDBACK || msg->cmd == CMD_JOIN_ACK) {
                    espnow_data[1] = msg->len - 6; // Adjust len
                    if (msg->len > 6) {
                        memcpy(&espnow_data[2], &msg->payload[6], msg->len - 6);
                        data_len += (msg->len - 6);
                    }
                } else {
                    // JOIN_HOST: remember the host so we can later unicast answers to it.
                    memcpy(joined_host_mac, target_mac, 6);
                    has_joined_host = true;
                    espnow_data[1] = 0;
                }
                esp_now_send(target_mac, espnow_data, data_len);

            } else if (msg->cmd == CMD_HOST_FOUND && msg->len >= 6) {
                // Payload is [scanner MAC(6)][lobby name]. Unicast the name to the board
                // that scanned, so the other scanning boards don't list this host again.
                // The scanner hasn't joined, so its link isn't tracked.
                uint8_t target_mac[6];
                memcpy(target_mac, msg->payload, 6);
                espnow_data[1] = msg->len - 6;
                memcpy(&espnow_data[2], &msg->payload[6], msg->len - 6);
                data_len += (msg->len - 6);

                ensure_peer(target_mac);
                if (esp_now_send(target_mac, espnow_data, data_len) == ESP_ERR_ESPNOW_NOT_FOUND) {
                    // espnow_send_cb freed the peer after an earlier answer; add it back
                    ensure_peer(target_mac);
                    esp_now_send(target_mac, espnow_data, data_len);
                }

            } else if (msg->cmd == CMD_BROADCAST_QUESTION || msg->cmd == CMD_TIME_UP) {
                // Unicast to every known slave instead of broadcasting, so each
                // delivery is hardware-ACKed via espnow_send_cb.
                if (msg->len > 0) {
                    memcpy(&espnow_data[2], msg->payload, msg->len);
                    data_len += msg->len;
                }
                for (int i = 0; i < tracked_peers_count; i++) {
                    if (tracked_peers[i].active) {
                        esp_now_send(tracked_peers[i].mac, espnow_data, data_len);
                    }
                }
            } else if (msg->cmd == CMD_SUBMIT_ANSWER && has_joined_host) {
                // Unicast the answer to the joined host so we get a delivery ACK too.
                if (msg->len > 0) {
                    memcpy(&espnow_data[2], msg->payload, msg->len);
                    data_len += msg->len;
                }
                esp_now_send(joined_host_mac, espnow_data, data_len);
            } else if (msg->cmd == CMD_SCAN_HOSTS) {
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
