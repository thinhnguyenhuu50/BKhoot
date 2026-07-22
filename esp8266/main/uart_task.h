#ifndef UART_TASK_H
#define UART_TASK_H

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "protocol.h"

extern QueueHandle_t uart_rx_queue;
extern QueueHandle_t uart_tx_queue;

void uart_task_init(void);
void uart_task_send(const protocol_msg_t *msg);

#endif // UART_TASK_H
