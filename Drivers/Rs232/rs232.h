#pragma once

#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include "usart.h"
#include "cmsis_os.h"

extern osSemaphoreId_t uart_tx_semHandle;

#define RX_BUFFER_SIZE 128
extern uint8_t rx_buffer[RX_BUFFER_SIZE];

/* Queue handle to be created in CubeMX (Item Size = uint16_t) */
extern osMessageQueueId_t uartRxQueueHandle;

void debug_log(const char* fmt, ...);

void StartTask_CommandParser(void *argument);