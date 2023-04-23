/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
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
/* Includes ------------------------------------------------------------------*/
#include "usart.h"

/* USER CODE BEGIN 0 */
#include "stdio.h"
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "cmsis_os.h"
/* USER CODE END 0 */

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart4;

/* USART1 init function */

void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}
/* USART4 init function */

void MX_USART4_UART_Init(void)
{

  /* USER CODE BEGIN USART4_Init 0 */

  /* USER CODE END USART4_Init 0 */

  /* USER CODE BEGIN USART4_Init 1 */

  /* USER CODE END USART4_Init 1 */
  huart4.Instance = USART4;
  huart4.Init.BaudRate = 115200;
  huart4.Init.WordLength = UART_WORDLENGTH_8B;
  huart4.Init.StopBits = UART_STOPBITS_1;
  huart4.Init.Parity = UART_PARITY_NONE;
  huart4.Init.Mode = UART_MODE_TX_RX;
  huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart4.Init.OverSampling = UART_OVERSAMPLING_16;
  huart4.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart4.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART4_Init 2 */

  /* USER CODE END USART4_Init 2 */

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */
    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    GPIO_InitStruct.Pin = TXD1_Pin|RXD1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF1_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }
  else if(uartHandle->Instance==USART4)
  {
  /* USER CODE BEGIN USART4_MspInit 0 */

  /* USER CODE END USART4_MspInit 0 */
    /* USART4 clock enable */
    __HAL_RCC_USART4_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART4 GPIO Configuration
    PA0     ------> USART4_TX
    PA1     ------> USART4_RX
    */
    GPIO_InitStruct.Pin = TXD4_Pin|RXD4_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_USART4;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART4 interrupt Init */
    HAL_NVIC_SetPriority(USART3_6_IRQn, 3, 0);
    HAL_NVIC_EnableIRQ(USART3_6_IRQn);
  /* USER CODE BEGIN USART4_MspInit 1 */
    __HAL_UART_ENABLE_IT(&huart4,UART_IT_RXNE);//接收中断
	__HAL_UART_ENABLE_IT(&huart4,UART_IT_IDLE);//空闲中断 
//__HAL_UART_ENABLE_IT(&huart4,UART_IT_TC);//发送完成中断 
    
  /* USER CODE END USART4_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();

    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    HAL_GPIO_DeInit(GPIOA, TXD1_Pin|RXD1_Pin);

  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
  else if(uartHandle->Instance==USART4)
  {
  /* USER CODE BEGIN USART4_MspDeInit 0 */

  /* USER CODE END USART4_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART4_CLK_DISABLE();

    /**USART4 GPIO Configuration
    PA0     ------> USART4_TX
    PA1     ------> USART4_RX
    */
    HAL_GPIO_DeInit(GPIOA, TXD4_Pin|RXD4_Pin);

    /* USART4 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART3_6_IRQn);
  /* USER CODE BEGIN USART4_MspDeInit 1 */

  /* USER CODE END USART4_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
#define App_Printf_NUM_MAX 253
extern osMutexId_t AppPrintfMutexHandle;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//全局函数
//----------------------------------------------------------------------------------------------
//线程安全的printf方式
//用互斥信号量保证打印的线程安全性能
void  App_Printf(const char *fmt, ...)
{
    char  buf_str[App_Printf_NUM_MAX + 1];  //App_Printf_NUM_MAX 根据实际需要可以设置更大或者小的值
    va_list   v_args;

    va_start(v_args, fmt);
   (void)vsnprintf((char       *)&buf_str[0],
                   (size_t      ) sizeof(buf_str),
                   (char const *) fmt,
                                  v_args);
    va_end(v_args);

    //##############################################################################################
	//互斥操作
    osMutexAcquire(AppPrintfMutexHandle, osWaitForever);    //一直等待获取互斥资源 
    printf("%s", buf_str);
    osMutexRelease(AppPrintfMutexHandle);                   //释放互斥资源
    //##############################################################################################
}

//----------------------------------------------------------------------------------------------
///重定向c库函数printf到串口，重定向后可使用printf函数
int fputc(int ch, FILE *f)
{
	//HAL_StatusTypeDef returnDat;
	
    //------------------------------------------------------------------------------
    //cocoguojia send
    
    HAL_UART_Transmit(&huart1,(uint8_t*)&ch,1,10);
    
  
    //----------------------------------------------------------------------------------------
    ////这里不能这样 当串口多了的时候 容易互相影响    
    //returnDat=HAL_UART_Transmit(&huart1,(uint8_t*)&ch,1,10);
    //do
    //{
    //returnDat=HAL_UART_Transmit(&huart1,(uint8_t*)&ch,1,10);
    //if(HAL_OK!=returnDat)
    //{
    ////osDelay(10);
    //}
    //}
    //while(HAL_OK!=returnDat);
		
	return (ch);
}


//----------------------------------------------------------------------------------------------
//线程安全的 打印字符串 函数
//用互斥信号量保证打印的线程安全性能
void  App_PutStr(const char *str)
{

    uint16_t len=strlen(str);
    //##############################################################################################
	//互斥操作
    osMutexAcquire(AppPrintfMutexHandle, osWaitForever);    //一直等待获取互斥资源 
    HAL_UART_Transmit(&huart1,(uint8_t*)str,len,10);
    osMutexRelease(AppPrintfMutexHandle);                   //释放互斥资源
    //############################################################################################## 
}


/* USER CODE END 1 */
