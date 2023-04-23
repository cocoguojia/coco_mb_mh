/**
 ****************************************************************************************************
 * @file        atk_mw8266d.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-06-21
 * @brief       ATK-MW8266D模块驱动代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 STM32F103开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 ****************************************************************************************************
 */

#ifndef __ATK_MW8266D_H
#define __ATK_MW8266D_H


#include "atk_mw8266d_uart.h"

///* 引脚定义 */
//#define ATK_MW8266D_RST_GPIO_PORT           GPIOA
//#define ATK_MW8266D_RST_GPIO_PIN            GPIO_PIN_4
//#define ATK_MW8266D_RST_GPIO_CLK_ENABLE()   do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0) /* PA口时钟使能 */



/* 错误代码 */
#define ATK_MW8266D_EOK         0   /* 没有错误 */
#define ATK_MW8266D_ERROR       1   /* 通用错误 */
#define ATK_MW8266D_ETIMEOUT    2   /* 超时错误 */
#define ATK_MW8266D_EINVAL      3   /* 参数错误 */

/* 操作函数 */
uint8_t g_atk_AT_MW8266_InitSub(void);
uint8_t g_atk_mw8266d_monitor(void);
uint8_t g_atk_mw8266d_send_CIPSEND_cmd(char* cmd,char* ack,uint16_t timeout);

#endif
