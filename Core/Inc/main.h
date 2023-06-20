/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include "stm32g0xx_hal.h"

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
#define IIC_SDA_Pin GPIO_PIN_13
#define IIC_SDA_GPIO_Port GPIOC

#define IIC_SCL_Pin GPIO_PIN_14
#define IIC_SCL_GPIO_Port GPIOC

#define MOTOR_CCW_Pin GPIO_PIN_5
#define MOTOR_CCW_GPIO_Port GPIOA

#define MOTOR_CW_Pin GPIO_PIN_6
#define MOTOR_CW_GPIO_Port GPIOA

#define BATT_LED_Pin                GPIO_PIN_7
#define BATT_LED_GPIO_Port          GPIOA

#define OK_LED_Pin GPIO_PIN_12
#define OK_LED_GPIO_Port GPIOB



#define ERR_LED_Pin GPIO_PIN_14
#define ERR_LED_GPIO_Port GPIOB

#define LED_EN_Pin GPIO_PIN_15
#define LED_EN_GPIO_Port GPIOB

#define FP_POWER_Pin 					GPIO_PIN_11
#define FP_POWER_GPIO_Port 				GPIOA

#define SIDE_KEY_INPUT_Pin 								GPIO_PIN_13
#define SIDE_KEY_INPUT_GPIO_Port 						GPIOB
#define SIDE_KEY_INPUT_EXTI_IRQn 						EXTI4_15_IRQn


#define FP_INT_INPUT_Pin                   				GPIO_PIN_12
#define FP_INT_INPUT_GPIO_Port        					GPIOA
#define FP_INT_INPUT_EXTI_IRQn        					EXTI4_15_IRQn


#define SC12B_INT_INPUT_Pin 							GPIO_PIN_8
#define SC12B_INT_INPUT_GPIO_Port 						GPIOB
#define SC12B_INT_INPUT_EXTI_IRQn 						EXTI4_15_IRQn





#define POWER_Pin GPIO_PIN_9
#define POWER_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */
void SystemClock_Config(void);

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
