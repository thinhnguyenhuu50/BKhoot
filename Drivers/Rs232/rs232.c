#include "rs232.h"

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART1) {
        osSemaphoreRelease(uart_tx_semHandle);
    }
}

// Static buffer so DMA can safely read after debug_log() returns
static char tx_buf[256];

void debug_log(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);

    // Wait for previous DMA transfer to finish (blocks the calling task, not the CPU)
    osSemaphoreAcquire(uart_tx_semHandle, osWaitForever);

    int len = vsnprintf(tx_buf, sizeof(tx_buf), fmt, args);
    va_end(args);

    if (len > 0) {
        HAL_UART_Transmit_DMA(&huart1, (uint8_t*)tx_buf, len);
        // Semaphore will be released in HAL_UART_TxCpltCallback
    } else {
        // Nothing to send, release the semaphore immediately
        osSemaphoreRelease(uart_tx_semHandle);
    }
}