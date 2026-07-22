#include "uart_task.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "freertos/task.h"
#include <string.h>

#define UART_NUM_STM32 UART_NUM_0
#define UART_NUM_DEBUG UART_NUM_1

#define BUF_SIZE (1024)

QueueHandle_t uart_rx_queue;
QueueHandle_t uart_tx_queue;

static const char *TAG = "UART_TASK";

void uart_task_send(const protocol_msg_t *msg) {
    if (uart_tx_queue) {
        // Create a dynamically allocated copy to send to the queue
        protocol_msg_t *msg_copy = (protocol_msg_t *)malloc(sizeof(protocol_msg_t));
        if (msg_copy) {
            memcpy(msg_copy, msg, sizeof(protocol_msg_t));
            xQueueSend(uart_tx_queue, &msg_copy, 0);
        }
    }
}

static void uart_rx_thread(void *arg) {
    uint8_t *data = (uint8_t *) malloc(BUF_SIZE);
    protocol_parser_t parser;
    protocol_parser_init(&parser);

    while (1) {
        int len = uart_read_bytes(UART_NUM_STM32, data, BUF_SIZE, 20 / portTICK_RATE_MS);
        if (len > 0) {
            for (int i = 0; i < len; i++) {
                protocol_msg_t *in_msg = (protocol_msg_t *)malloc(sizeof(protocol_msg_t));
                if (in_msg) {
                    if (protocol_parse_byte(&parser, data[i], in_msg)) {
                        if (xQueueSend(uart_rx_queue, &in_msg, 0) != pdTRUE) {
                            free(in_msg);
                        }
                    } else {
                        free(in_msg);
                    }
                }
            }
        }
    }
}

static void uart_tx_thread(void *arg) {
    protocol_msg_t *msg;
    uint8_t tx_buf[MAX_PAYLOAD + 4];
    
    while (1) {
        if (xQueueReceive(uart_tx_queue, &msg, portMAX_DELAY) == pdTRUE) {
            int len = protocol_build_frame(tx_buf, msg);
            uart_write_bytes(UART_NUM_STM32, (const char *)tx_buf, len);
            free(msg);
        }
    }
}

void uart_task_init(void) {
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    
    // Configure UART0 for STM32 comms (GPIO1/TX, GPIO3/RX)
    uart_param_config(UART_NUM_STM32, &uart_config);
    uart_driver_install(UART_NUM_STM32, BUF_SIZE * 2, BUF_SIZE * 2, 0, NULL, 0);

    // Configure UART1 for Debug logging (GPIO2/TX only)
    uart_param_config(UART_NUM_DEBUG, &uart_config);
    uart_driver_install(UART_NUM_DEBUG, BUF_SIZE * 2, 0, 0, NULL, 0);

    // Redirect standard output to UART1
    // esp_log_set_vprintf(...) is an option, but usually UART1 can be set as console in menuconfig
    // The user requested to repurpose GPIO2 for UART1 debug logging.

    uart_rx_queue = xQueueCreate(10, sizeof(protocol_msg_t *));
    uart_tx_queue = xQueueCreate(10, sizeof(protocol_msg_t *));

    xTaskCreate(uart_rx_thread, "uart_rx_thread", 2048, NULL, 10, NULL);
    xTaskCreate(uart_tx_thread, "uart_tx_thread", 2048, NULL, 10, NULL);
}
