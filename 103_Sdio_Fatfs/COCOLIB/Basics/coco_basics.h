

#ifndef __COCO_BASICS_H__
#define __COCO_BASICS_H__

#include "myinclude.h"

extern void  App_Printf(const char *fmt, ...);


//--------------------------------------------------------------------------------------------
//LED1 LED2 小蓝灯 预定义
#define LED1_EN     HAL_GPIO_WritePin(LED1_GPIO_Port,LED1_Pin,GPIO_PIN_RESET)
#define LED1_NO     HAL_GPIO_WritePin(LED1_GPIO_Port,LED1_Pin,GPIO_PIN_SET)

#define LED2_EN     HAL_GPIO_WritePin(LED2_GPIO_Port,LED2_Pin,GPIO_PIN_RESET)
#define LED2_NO     HAL_GPIO_WritePin(LED2_GPIO_Port,LED2_Pin,GPIO_PIN_SET)

//--------------------------------------------------------------------------------------------
//蜂鸣器 预定义
//#define BEEP_EN     HAL_GPIO_WritePin(BEEP_GPIO_Port,BEEP_Pin,GPIO_PIN_SET)
#define BEEP_EN     HAL_GPIO_WritePin(BEEP_GPIO_Port,BEEP_Pin,GPIO_PIN_RESET)
#define BEEP_NO     HAL_GPIO_WritePin(BEEP_GPIO_Port,BEEP_Pin,GPIO_PIN_RESET)


//--------------------------------------------------------------------------------------------
//RGB 预定义
#define RGB_R_EN     HAL_GPIO_WritePin(RGB_R_GPIO_Port,RGB_R_Pin,GPIO_PIN_RESET)
#define RGB_R_NO     HAL_GPIO_WritePin(RGB_R_GPIO_Port,RGB_R_Pin,GPIO_PIN_SET)

#define RGB_G_EN     HAL_GPIO_WritePin(RGB_G_GPIO_Port,RGB_G_Pin,GPIO_PIN_RESET)
#define RGB_G_NO     HAL_GPIO_WritePin(RGB_G_GPIO_Port,RGB_G_Pin,GPIO_PIN_SET)

#define RGB_B_EN     HAL_GPIO_WritePin(RGB_B_GPIO_Port,RGB_B_Pin,GPIO_PIN_RESET)
#define RGB_B_NO     HAL_GPIO_WritePin(RGB_B_GPIO_Port,RGB_B_Pin,GPIO_PIN_SET)

//如果单独亮 为红
#define RGBR_EN			RGB_G_NO;	RGB_B_NO;	RGB_R_EN;
//如果单独亮 为绿
#define RGBG_EN			RGB_R_NO;	RGB_B_NO;	RGB_G_EN;
//如果单独亮 为蓝
#define RGBB_EN			RGB_R_NO;	RGB_G_NO;	RGB_B_EN;
//红+绿=黄
#define RGBY_EN		    RGB_B_NO;	RGB_R_EN;	RGB_G_EN;
//红+蓝=紫
#define RGBP_EN			RGB_G_NO;	RGB_R_EN;	RGB_B_EN;
//绿+蓝=青
#define RGBC_EN			RGB_R_NO;	RGB_G_EN;	RGB_B_EN;
//红+绿+蓝=白
#define RGBW_EN			RGB_R_EN;	RGB_G_EN;	RGB_B_EN;
//全灭
#define RGB_ALLOFF		RGB_R_NO;	RGB_G_NO;	RGB_B_NO;


//-----------------------------------------------------------------------------
////按钮预定义
#define K1ING (GPIO_PIN_SET==HAL_GPIO_ReadPin(K1_GPIO_Port,K1_Pin))
#define K2ING (GPIO_PIN_SET==HAL_GPIO_ReadPin(K2_GPIO_Port,K2_Pin))



#endif






