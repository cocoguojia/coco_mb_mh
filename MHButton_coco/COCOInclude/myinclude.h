
#ifndef __COCO_MYINCLUDE_H__
#define __COCO_MYINCLUDE_H__


#include "stdio.h"
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"



#include "main.h"
#include "usart.h"
#include "gpio.h"
#include "usart.h"
#include "atk_mw8266d.h"

#include "globalVariable.h"

#include "COCO_StartOsTaskSub.h"
#include "KeyTask.h"
#include "KeyTimerTask.h"
#include "DiabcTask.h"
#include "WifiTask.h"
#include "W5500Task.h"
#include "WifiSendTask.h"
#include "W5500SendTask.h"
#include "powerDownDoTask.h"
#include "eMBPollTask.h"
#include "monitoringTask.h"


#include "NetBridgeModbusTask.h"

  
#include "w25q64.h"



#include "port_fm.h"

typedef __IO uint32_t  vu32;
typedef __IO uint16_t vu16;
typedef __IO uint8_t  vu8;
typedef uint32_t  u32;
typedef uint16_t u16;
typedef uint8_t  u8;



///////////////////////////////////////////////////////////////////////////////////////////////
//管脚预定义

//-----------------------------------------------------------------------------
//按钮预定义
#define KEY1ING (GPIO_PIN_RESET==HAL_GPIO_ReadPin(KEY1_GPIO_Port,KEY1_Pin))
#define KEY2ING (GPIO_PIN_RESET==HAL_GPIO_ReadPin(KEY2_GPIO_Port,KEY2_Pin))
#define KEY3ING (GPIO_PIN_RESET==HAL_GPIO_ReadPin(KEY3_GPIO_Port,KEY3_Pin))
#define KEY4ING (GPIO_PIN_RESET==HAL_GPIO_ReadPin(KEY4_GPIO_Port,KEY4_Pin))
#define KEY5ING (GPIO_PIN_RESET==HAL_GPIO_ReadPin(KEY5_GPIO_Port,KEY5_Pin))

//-----------------------------------------------------------------------------
//开关量预定义
#define DIAING  (GPIO_PIN_SET==HAL_GPIO_ReadPin(DIA_GPIO_Port,DIA_Pin))
#define DIBING  (GPIO_PIN_SET==HAL_GPIO_ReadPin(DIB_GPIO_Port,DIB_Pin))
#define DICING  (GPIO_PIN_SET==HAL_GPIO_ReadPin(DIC_GPIO_Port,DIC_Pin))

#endif

