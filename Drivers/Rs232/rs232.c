#include "rs232.h"


char tx_buf[256];

uint8_t rx_buffer[RX_BUFFER_SIZE];

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART1) {
        osSemaphoreRelease(uart_tx_semHandle);
    }
}

/**
 * @brief This callback is called when the UART line goes IDLE (message complete)
 *        or when the DMA buffer is completely full.
 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if (huart->Instance == USART1)
    {
        // In Circular DMA mode, 'Size' is the current write position in rx_buffer.
        // We simply push this position (uint16_t) directly to the queue.
        // The parsing task will calculate the length and handle wraparounds.
        osMessageQueuePut(uartRxQueueHandle, &Size, 0, 0);
    }
}

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

void StartTask_CommandParser(void *argument)
{
    uint16_t current_pos = 0;
    static uint16_t old_pos = 0;
    char local_buf[RX_BUFFER_SIZE + 1];
    

    for(;;)
    {
        // Wait forever until a message arrives in the queue
        if (osMessageQueueGet(uartRxQueueHandle, &current_pos, NULL, osWaitForever) == osOK)
        {
            uint16_t len = 0;
            
            // Extract from circular buffer
            if (current_pos > old_pos) {
                len = current_pos - old_pos;
                memcpy(local_buf, &rx_buffer[old_pos], len);
            } 
            else if (current_pos < old_pos) {
                len = (RX_BUFFER_SIZE - old_pos) + current_pos;
                memcpy(local_buf, &rx_buffer[old_pos], RX_BUFFER_SIZE - old_pos);
                memcpy(local_buf + (RX_BUFFER_SIZE - old_pos), rx_buffer, current_pos);
            }
            
            local_buf[len] = '\0';
            old_pos = current_pos;
            
            // Parse command
            char* cmd = strtok(local_buf, "\n");
            if (cmd != NULL && cmd[0] != '\0') 
            {
                switch (cmd[0]) 
                {
                    case 'h':
                        debug_log("--- Options Menu ---\r\n");
                        debug_log("1. Show RTOS Statistics\r\n");
                        debug_log("--------------------\r\n");
                        break;
                    
                    case '1':
                        debug_log("--- RTOS Statistics ---\r\n");
                        
                        // Heap Usage
                        size_t freeHeap = xPortGetFreeHeapSize();
                        size_t minFreeHeap = xPortGetMinimumEverFreeHeapSize();
                        debug_log("Free Heap: %u bytes (%u%% free)\r\n", (uint32_t)freeHeap, (uint32_t)((freeHeap * 100) / 524288));
                        debug_log("Min Free Heap Ever: %u bytes (%u%% free)\r\n", (uint32_t)minFreeHeap, (uint32_t)((minFreeHeap * 100) / 524288));
                        
                        // Task List
                        debug_log("\r\n%-20s | %-14s | %-14s | %-14s | %-14s\r\n", "Task Name", "State", "Priority", "Remain Stack", "Num");
                        debug_log("---------------------|----------------|----------------|----------------|----------------\r\n");
                        
                        UBaseType_t uxArraySize = uxTaskGetNumberOfTasks();
                        TaskStatus_t *pxTaskStatusArray = pvPortMalloc(uxArraySize * sizeof(TaskStatus_t));
                        
                        if (pxTaskStatusArray != NULL)
                        {
                            uint32_t ulTotalRunTime;
                            uxArraySize = uxTaskGetSystemState(pxTaskStatusArray, uxArraySize, &ulTotalRunTime);
                            
                            for (UBaseType_t i = 0; i < uxArraySize; i++)
                            {
                                char cStatus;
                                switch (pxTaskStatusArray[i].eCurrentState)
                                {
                                    case eRunning:   cStatus = 'X'; break;
                                    case eReady:     cStatus = 'R'; break;
                                    case eBlocked:   cStatus = 'B'; break;
                                    case eSuspended: cStatus = 'S'; break;
                                    case eDeleted:   cStatus = 'D'; break;
                                    default:         cStatus = '?'; break;
                                }
                                
                                debug_log("%-20s | %-14c | %-14u | %-14u | %-14u\r\n",
                                          pxTaskStatusArray[i].pcTaskName,
                                          cStatus,
                                          (uint32_t)pxTaskStatusArray[i].uxCurrentPriority,
                                          (uint32_t)pxTaskStatusArray[i].usStackHighWaterMark,
                                          (uint32_t)pxTaskStatusArray[i].xTaskNumber);
                            }
                            
                            vPortFree(pxTaskStatusArray);
                        }
                        else
                        {
                            debug_log("Failed to allocate memory for task status.\r\n");
                        }
                        
                        debug_log("-----------------------\r\n");
                        break;
                    
                    default:
                        debug_log("Unknown command. Send 'h' for help.\r\n");
                        break;
                }
            }
        }
    }
}