/*
* FreeModbus Libary: lwIP Port
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
* File: $Id: porttcp.c,v 1.1 2006/08/30 23:18:07 wolti Exp $
*/


#include <stdio.h>
#include <string.h>
#include "port_fm.h"

#include "mb.h"
#include "mbport.h"
#include "w5500_conf.h"
#include "w5500.h"
#include "socket.h"

#include "myinclude.h"
#include "atk_mw8266d_struct.h"

/* ----------------------- MBAP Header --------------------------------------*/
#define MB_TCP_UID          6
#define MB_TCP_LEN          4
#define MB_TCP_FUNC         7

#define MB_TCP_DEFAULT_PORT  502          /* TCP listening port. */


/* ----------------------- Prototypes ---------------------------------------*/
UCHAR    aucTCPBuf[MB_TCP_BUF_SIZE];	   //接收缓冲区
USHORT   usTCPBufLen;


extern char temp_buf[12];
extern int llen;

int wifi_rx=0; //wifi接收到请求报文标识 

u8 *send_buff;

//------------------------------------------------------------------------------------
//modbus tcp的 tcp网络初始化 此功能由网络或者wifi模块初始化完成 这里直接返回完成
//这部分功能不应该被modbus接管，modbus负责好协议对接即可
BOOL xMBTCPPortInit( USHORT usTCPPort )
{   
      return TRUE;
}

//------------------------------------------------------------------------------------
//这部分功能不应该被modbus接管，modbus负责好协议对接即可
void  vMBTCPPortClose(  )
{
    ;
}

//------------------------------------------------------------------------------------
//这部分功能不应该被modbus接管，modbus负责好协议对接即可
void vMBTCPPortDisable( void )
{
    ;
}

BOOL xMBTCPPortGetRequest( UCHAR ** ppucMBTCPFrame, USHORT * usTCPLength )
{
	*ppucMBTCPFrame = &aucTCPBuf[0];
    *usTCPLength = usTCPBufLen;
    /* Reset the buffer. */
    usTCPBufLen = 0;
    //在这里添加  aucTCPBuf  usTCPBufLen接收缓冲区的填充和长度的赋值 
    //然后 xMBPortEventPost( EV_FRAME_RECEIVED );//发送已接收到新数据到Modbus-TCP状态机 告诉modbus 有数据接收到 让其进行处理
    return TRUE;
}



//BOOL xMBTCPPortSendResponse(const UCHAR * pucMBTCPFrame,  USHORT usTCPLength )//将回复报文发送到上位机
//{
//    uint8_t i=0;
//    char at_buff[20];
//    
//    HAL_StatusTypeDef statusDat=0;
//    
//   
//    if(1==g_tcpOrwifiFlag)//是网口传来的数据
//    {
//        App_Printf("$$W5500-->Send Modbus ACK\r\n");
//        send(SOCK_TCPS,pucMBTCPFrame,usTCPLength); /*向Server发送数据*/
//    }
//    else if(2==g_tcpOrwifiFlag)//是wifi传来的数据 应该wifi传出去
//    {
//       App_Printf("$$WIFI-->Send Modbus ACK\r\n");
//       
//        sprintf((char*)at_buff,"AT+CIPSEND=0,%d",usTCPLength);
//        
//		g_atk_mw8266d_send_CIPSEND_cmd(at_buff,"OK",100);  //发送指定长度的数据
//		osDelay(50); 

//         do
//         {
//             statusDat=HAL_UART_Transmit(&huart4, pucMBTCPFrame, usTCPLength, HAL_MAX_DELAY);
//             
//             if(HAL_OK!=statusDat)
//             {
//                osDelay(200);
//             }
//         }
//         while(HAL_OK!=statusDat);
//    }
//    else
//    {
//        ;
//    }
//    return TRUE;
//}



BOOL xMBTCPPortSendResponse(const UCHAR * pucMBTCPFrame,  USHORT usTCPLength )//将回复报文发送到上位机
{
    uint16_t w5500Msg=0;
    uint16_t wifiMsg=0;
    uint16_t i;
    UCHAR * pucMBTCPFrameTemp=(UCHAR *)pucMBTCPFrame;  
      
    if(1==g_tcpOrwifiFlag)//是网口传来的数据
    {        
        if(TCP_WIFI_SENDLEN>=usTCPLength)
        {
            App_Printf("$$W5500-->Send Modbus ACK\r\n");
            g_w5500SendPingPongFLag++;
            if(TCP_WIFI_PINGPONG_N<=g_w5500SendPingPongFLag)
            {
                g_w5500SendPingPongFLag=0;
            }
            w5500Msg=g_w5500SendPingPongFLag;  
            g_w5500Sendbuff[g_w5500SendPingPongFLag][0]=(uint8_t)(usTCPLength>>8);
            g_w5500Sendbuff[g_w5500SendPingPongFLag][1]=(uint8_t)(usTCPLength&0x00ff);
            for(i=0;i<usTCPLength;i++)
            {
                g_w5500Sendbuff[g_w5500SendPingPongFLag][i+2]=*pucMBTCPFrameTemp;
                pucMBTCPFrameTemp++;
            }              
            osMessageQueuePut(w5500SendQueueHandle,&w5500Msg, 0U, 100U);
        }
    }
    else if(2==g_tcpOrwifiFlag)//是wifi传来的数据 应该wifi传出去
    {
        if(TCP_WIFI_SENDLEN>=usTCPLength)
        {
            App_Printf("$$WIFI-->Send Modbus ACK\r\n");      
            g_wifiSendPingPongFLag++;
            if(TCP_WIFI_PINGPONG_N<=g_wifiSendPingPongFLag)
            {
                g_wifiSendPingPongFLag=0;
            }           
            wifiMsg=g_wifiSendPingPongFLag;
            g_wifiSendbuff[g_wifiSendPingPongFLag][0]=(uint8_t)(usTCPLength>>8);
            g_wifiSendbuff[g_wifiSendPingPongFLag][1]=(uint8_t)(usTCPLength&0x00ff);
            for(i=0;i<usTCPLength;i++)
            {
                g_wifiSendbuff[g_wifiSendPingPongFLag][i+2]=*pucMBTCPFrameTemp;
                pucMBTCPFrameTemp++;
            }            
            osMessageQueuePut(wifiSendQueueHandle,&wifiMsg, 0U, 100U);
        }
    }
    else
    {
        ;
    }
    return TRUE;
}




////----------------------------------------------------------------------------------------------------
////放在初始化里，注意eMBTCPInit的形式参数502没有意义，端口号由具体的网络wifi模块初始化决定
//eMBTCPInit( 502 );     // modbus TCP初始化 
//eMBEnable();	        // modbus 协议开始函数 

////----------------------------
////放在大循环里，处理modbus
//eMBPoll();



void EnterCriticalSection( void )
{
  taskENTER_CRITICAL();
}

void ExitCriticalSection( void )
{
  taskEXIT_CRITICAL();
}
