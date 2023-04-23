/**
 ****************************************************************************************************
 * @file        atk_mw8266d.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2022-06-21
 * @brief       ATK-MW8266Dģ����������
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� STM32F103������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 *
 ****************************************************************************************************
 */

#ifndef __ATK_MW8266D_H
#define __ATK_MW8266D_H


#include "atk_mw8266d_uart.h"

///* ���Ŷ��� */
//#define ATK_MW8266D_RST_GPIO_PORT           GPIOA
//#define ATK_MW8266D_RST_GPIO_PIN            GPIO_PIN_4
//#define ATK_MW8266D_RST_GPIO_CLK_ENABLE()   do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0) /* PA��ʱ��ʹ�� */



/* ������� */
#define ATK_MW8266D_EOK         0   /* û�д��� */
#define ATK_MW8266D_ERROR       1   /* ͨ�ô��� */
#define ATK_MW8266D_ETIMEOUT    2   /* ��ʱ���� */
#define ATK_MW8266D_EINVAL      3   /* �������� */

/* �������� */
uint8_t g_atk_AT_MW8266_InitSub(void);
uint8_t g_atk_mw8266d_monitor(void);
uint8_t g_atk_mw8266d_send_CIPSEND_cmd(char* cmd,char* ack,uint16_t timeout);

#endif
