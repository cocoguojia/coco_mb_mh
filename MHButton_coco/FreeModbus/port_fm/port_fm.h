/*
 * FreeModbus Libary: BARE Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id: port.h,v 1.1 2006/08/22 21:35:13 wolti Exp $
 */

#ifndef _PORT_FM_H
#define _PORT_FM_H

#include <assert.h>
#include <inttypes.h>

#include "myinclude.h"

//#define	INLINE                      inline
#define PR_BEGIN_EXTERN_C           extern "C" {
#define	PR_END_EXTERN_C             }

#define ENTER_CRITICAL_SECTION( )   taskENTER_CRITICAL()//__set_PRIMASK(1)  //关总中断
#define EXIT_CRITICAL_SECTION( )    taskEXIT_CRITICAL()//__set_PRIMASK(0)  //开总中断

typedef uint8_t BOOL;

typedef unsigned char UCHAR;
typedef char CHAR;

typedef uint16_t USHORT;
typedef int16_t SHORT;

typedef uint32_t ULONG;
typedef int32_t LONG;

#ifndef TRUE
#define TRUE            1
#endif

#ifndef FALSE
#define FALSE           0
#endif


//有些modbus是从地址0开始的，所以我们在这里做个兼容,cocoguojia say
/******************************************************************************
                               定义Modbus相关参数
******************************************************************************/
#ifdef MODBUS_ADDR_0
#define REG_INPUT_START                          (USHORT)0x0000  //起始寄存器
#define REG_INPUT_NREGS                          (USHORT)8       //寄存器个数
#define REG_HOLDING_START                        (USHORT)0x0000  //保持寄存器
#define REG_HOLDING_NREGS                        (USHORT)16       //保持寄存器个数
#define REG_COILS_START                          (USHORT)0x0000
#define REG_COILS_SIZE                           (USHORT)16
#define REG_DISCRETE_START                       (USHORT)0x0000
#define REG_DISCRETE_SIZE                        (USHORT)16
#else
#define REG_INPUT_START                          (USHORT)0x0001  //起始寄存器
#define REG_INPUT_NREGS                          (USHORT)8       //寄存器个数
#define REG_HOLDING_START                        (USHORT)0x0001  //保持寄存器
#define REG_HOLDING_NREGS                        (USHORT)16       //保持寄存器个数
#define REG_COILS_START                          (USHORT)0x0001
#define REG_COILS_SIZE                           (USHORT)16
#define REG_DISCRETE_START                       (USHORT)0x0001
#define REG_DISCRETE_SIZE                        (USHORT)16
#endif


//                               外部调用功能函数
/******************************************************************************/

extern USHORT usRegInputBuf[REG_INPUT_NREGS];
extern USHORT usRegHoldingBuf[REG_HOLDING_NREGS];
extern UCHAR ucRegCoilsBuf[REG_COILS_SIZE / 8];
extern UCHAR usRegDiscreteBuf[REG_DISCRETE_SIZE / 8];


#endif
