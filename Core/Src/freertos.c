/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef StaticTask_t osStaticThreadDef_t;
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for ledBlinkingTask */
osThreadId_t ledBlinkingTaskHandle;
const osThreadAttr_t ledBlinkingTask_attributes = {
  .name = "ledBlinkingTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for GUI_Task */
osThreadId_t GUI_TaskHandle;
uint32_t GUI_TaskBuffer[ 2048 ];
osStaticThreadDef_t GUI_TaskControlBlock;
const osThreadAttr_t GUI_Task_attributes = {
  .name = "GUI_Task",
  .cb_mem = &GUI_TaskControlBlock,
  .cb_size = sizeof(GUI_TaskControlBlock),
  .stack_mem = &GUI_TaskBuffer[0],
  .stack_size = sizeof(GUI_TaskBuffer),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for Communication_T */
osThreadId_t Communication_THandle;
const osThreadAttr_t Communication_T_attributes = {
  .name = "Communication_T",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for Game_Logic_Task */
osThreadId_t Game_Logic_TaskHandle;
const osThreadAttr_t Game_Logic_Task_attributes = {
  .name = "Game_Logic_Task",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for CommandParser */
osThreadId_t CommandParserHandle;
const osThreadAttr_t CommandParser_attributes = {
  .name = "CommandParser",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for commQueue */
osMessageQueueId_t commQueueHandle;
const osMessageQueueAttr_t commQueue_attributes = {
  .name = "commQueue"
};
/* Definitions for uartRxQueue */
osMessageQueueId_t uartRxQueueHandle;
const osMessageQueueAttr_t uartRxQueue_attributes = {
  .name = "uartRxQueue"
};
/* Definitions for uart_tx_sem */
osSemaphoreId_t uart_tx_semHandle;
const osSemaphoreAttr_t uart_tx_sem_attributes = {
  .name = "uart_tx_sem"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartTask_LedBlinking(void *argument);
extern void StartTask_LVGL(void *argument);
extern void StartTask_COMM(void *argument);
extern void StartTask_GAME(void *argument);
extern void StartTask_CommandParser(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* creation of uart_tx_sem */
  uart_tx_semHandle = osSemaphoreNew(1, 1, &uart_tx_sem_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of commQueue */
  commQueueHandle = osMessageQueueNew (4, sizeof(uint32_t), &commQueue_attributes);

  /* creation of uartRxQueue */
  uartRxQueueHandle = osMessageQueueNew (3, sizeof(uint16_t), &uartRxQueue_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of ledBlinkingTask */
  ledBlinkingTaskHandle = osThreadNew(StartTask_LedBlinking, NULL, &ledBlinkingTask_attributes);

  /* creation of GUI_Task */
  GUI_TaskHandle = osThreadNew(StartTask_LVGL, NULL, &GUI_Task_attributes);

  /* creation of Communication_T */
  Communication_THandle = osThreadNew(StartTask_COMM, NULL, &Communication_T_attributes);

  /* creation of Game_Logic_Task */
  Game_Logic_TaskHandle = osThreadNew(StartTask_GAME, NULL, &Game_Logic_Task_attributes);

  /* creation of CommandParser */
  CommandParserHandle = osThreadNew(StartTask_CommandParser, NULL, &CommandParser_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartTask_LedBlinking */
/**
  * @brief  Function implementing the ledBlinkingTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartTask_LedBlinking */
void StartTask_LedBlinking(void *argument)
{
  /* USER CODE BEGIN StartTask_LedBlinking */
  /* Infinite loop */
  for(;;)
  {
    HAL_GPIO_TogglePin(LED_DEBUG_GPIO_Port, LED_DEBUG_Pin);
    osDelay(1000);
  }
  /* USER CODE END StartTask_LedBlinking */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
osStatus_t osThreadDetach(osThreadId_t thread_id)
{
    (void)thread_id;
    return osOK;
}

#include <string.h>

#define HEAP_WRAP_OFFSET 8

void* __wrap_malloc(size_t size) {
    void *p = pvPortMalloc(size + HEAP_WRAP_OFFSET);
    if (p != NULL) {
        *(size_t*)p = size;
        return (void*)((uint8_t*)p + HEAP_WRAP_OFFSET);
    }
    return NULL;
}

void __wrap_free(void* ptr) {
    if (ptr != NULL) {
        void *p = (uint8_t*)ptr - HEAP_WRAP_OFFSET;
        vPortFree(p);
    }
}

void* __wrap_realloc(void* ptr, size_t new_size) {
    if (new_size == 0) {
        __wrap_free(ptr);
        return NULL;
    }
    if (ptr == NULL) {
        return __wrap_malloc(new_size);
    }
    
    void *p = (uint8_t*)ptr - HEAP_WRAP_OFFSET;
    size_t old_size = *(size_t*)p;
    
    void *new_ptr = __wrap_malloc(new_size);
    if (new_ptr != NULL) {
        memcpy(new_ptr, ptr, (old_size < new_size) ? old_size : new_size);
        __wrap_free(ptr);
    }
    return new_ptr;
}

void* __wrap_calloc(size_t num, size_t size) {
    size_t total = num * size;
    void *ptr = __wrap_malloc(total);
    if (ptr != NULL) {
        memset(ptr, 0, total);
    }
    return ptr;
}
/* USER CODE END Application */

