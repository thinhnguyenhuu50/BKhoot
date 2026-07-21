/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED_DEBUG_Pin GPIO_PIN_4
#define LED_DEBUG_GPIO_Port GPIOE
#define FSMC_RES_Pin GPIO_PIN_13
#define FSMC_RES_GPIO_Port GPIOC
#define ESP_BUSY_Pin GPIO_PIN_9
#define ESP_BUSY_GPIO_Port GPIOF
#define ESP_POWER_Pin GPIO_PIN_10
#define ESP_POWER_GPIO_Port GPIOF
#define ESP_RX_Pin GPIO_PIN_2
#define ESP_RX_GPIO_Port GPIOA
#define ESP_TX_Pin GPIO_PIN_3
#define ESP_TX_GPIO_Port GPIOA
#define T_CS_Pin GPIO_PIN_7
#define T_CS_GPIO_Port GPIOG
#define T_CLK_Pin GPIO_PIN_8
#define T_CLK_GPIO_Port GPIOG
#define T_PEN_Pin GPIO_PIN_8
#define T_PEN_GPIO_Port GPIOC
#define T_MOSI_Pin GPIO_PIN_9
#define T_MOSI_GPIO_Port GPIOC
#define FSMC_BLK_Pin GPIO_PIN_8
#define FSMC_BLK_GPIO_Port GPIOA
#define T_MISO_Pin GPIO_PIN_12
#define T_MISO_GPIO_Port GPIOC

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
