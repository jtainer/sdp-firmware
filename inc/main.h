/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "stm32h7xx_hal.h"

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
#define USER0_Pin GPIO_PIN_10
#define USER0_GPIO_Port GPIOE
#define USER1_Pin GPIO_PIN_11
#define USER1_GPIO_Port GPIOE
#define USER2_Pin GPIO_PIN_12
#define USER2_GPIO_Port GPIOE
#define USER3_Pin GPIO_PIN_13
#define USER3_GPIO_Port GPIOE
#define USER4_Pin GPIO_PIN_14
#define USER4_GPIO_Port GPIOE
#define MUX0_A0_Pin GPIO_PIN_0
#define MUX0_A0_GPIO_Port GPIOD
#define MUX0_A1_Pin GPIO_PIN_1
#define MUX0_A1_GPIO_Port GPIOD
#define MUX0_A2_Pin GPIO_PIN_2
#define MUX0_A2_GPIO_Port GPIOD
#define MUX1_A0_Pin GPIO_PIN_3
#define MUX1_A0_GPIO_Port GPIOD
#define MUX1_A1_Pin GPIO_PIN_4
#define MUX1_A1_GPIO_Port GPIOD
#define MUX1_A2_Pin GPIO_PIN_5
#define MUX1_A2_GPIO_Port GPIOD
#define MUX0_RST_Pin GPIO_PIN_6
#define MUX0_RST_GPIO_Port GPIOD
#define MUX1_RST_Pin GPIO_PIN_7
#define MUX1_RST_GPIO_Port GPIOD

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
