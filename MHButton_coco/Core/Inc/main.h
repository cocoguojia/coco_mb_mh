/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "stm32f0xx_hal.h"

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
#define BL8025_INT_Pin GPIO_PIN_13
#define BL8025_INT_GPIO_Port GPIOC
#define TXD4_Pin GPIO_PIN_0
#define TXD4_GPIO_Port GPIOA
#define RXD4_Pin GPIO_PIN_1
#define RXD4_GPIO_Port GPIOA
#define LEDB_Pin GPIO_PIN_2
#define LEDB_GPIO_Port GPIOA
#define KEY1_Pin GPIO_PIN_3
#define KEY1_GPIO_Port GPIOA
#define KEY2_Pin GPIO_PIN_4
#define KEY2_GPIO_Port GPIOA
#define KEY3_Pin GPIO_PIN_5
#define KEY3_GPIO_Port GPIOA
#define KEY4_Pin GPIO_PIN_6
#define KEY4_GPIO_Port GPIOA
#define KEY5_Pin GPIO_PIN_7
#define KEY5_GPIO_Port GPIOA
#define DIC_Pin GPIO_PIN_0
#define DIC_GPIO_Port GPIOB
#define DIB_Pin GPIO_PIN_1
#define DIB_GPIO_Port GPIOB
#define DIA_Pin GPIO_PIN_2
#define DIA_GPIO_Port GPIOB
#define POWER_CONTROL_Pin GPIO_PIN_10
#define POWER_CONTROL_GPIO_Port GPIOB
#define POWER_CHECK_Pin GPIO_PIN_11
#define POWER_CHECK_GPIO_Port GPIOB
#define W5500_NSS_Pin GPIO_PIN_12
#define W5500_NSS_GPIO_Port GPIOB
#define W5500_SCK_Pin GPIO_PIN_13
#define W5500_SCK_GPIO_Port GPIOB
#define W5500_MISO_Pin GPIO_PIN_14
#define W5500_MISO_GPIO_Port GPIOB
#define W5500_MOSI_Pin GPIO_PIN_15
#define W5500_MOSI_GPIO_Port GPIOB
#define WIFI_EN_Pin GPIO_PIN_8
#define WIFI_EN_GPIO_Port GPIOA
#define TXD1_Pin GPIO_PIN_9
#define TXD1_GPIO_Port GPIOA
#define RXD1_Pin GPIO_PIN_10
#define RXD1_GPIO_Port GPIOA
#define W5500_INT_Pin GPIO_PIN_11
#define W5500_INT_GPIO_Port GPIOA
#define W5500_RESET_Pin GPIO_PIN_12
#define W5500_RESET_GPIO_Port GPIOA
#define W25Q16_CS_Pin GPIO_PIN_15
#define W25Q16_CS_GPIO_Port GPIOA
#define W25Q16_SCK_Pin GPIO_PIN_3
#define W25Q16_SCK_GPIO_Port GPIOB
#define W25Q16_MISO_Pin GPIO_PIN_4
#define W25Q16_MISO_GPIO_Port GPIOB
#define W25Q16_MOSI_Pin GPIO_PIN_5
#define W25Q16_MOSI_GPIO_Port GPIOB
#define BL8025_SCL_Pin GPIO_PIN_6
#define BL8025_SCL_GPIO_Port GPIOB
#define BL8025_SDA_Pin GPIO_PIN_7
#define BL8025_SDA_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
