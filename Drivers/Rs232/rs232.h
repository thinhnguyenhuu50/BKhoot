#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include "usart.h"
#include "cmsis_os.h"

extern osSemaphoreId_t uart_tx_semHandle;

void debug_log(const char* fmt, ...);