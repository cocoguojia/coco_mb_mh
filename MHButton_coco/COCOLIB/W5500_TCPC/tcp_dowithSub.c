/**
************************************************************************************************
* @file   		tcp_demo.c
* @author  		WIZnet Software Team 
* @version 		V1.0
* @date    		2015-02-14
* @brief   		TCP 演示函数
* @attention  
************************************************************************************************
**/



#include "tcp_dowithSub.h"

#include "w5500.h"
#include "W5500_conf.h"
#include "socket.h"
#include "utility.h"

uint8_t buff[MB_TCP_BUF_SIZE+3];				                    /*定义一个缓存*/

//----------------------------------------------------------------------------------------------------------------------------
//实际处理函数
void TcpServerDoWith(void)
{	
    static uint8_t stateLastN=0;
    uint16 len=0;	
    uint16 msg;
	switch(getSn_SR(SOCK_TCPS))						    //获取socket的状态
	{
		case SOCK_CLOSED:								//socket处于关闭状态
        if(stateLastN!=SOCK_CLOSED)
        {
            App_Printf("$$W5500 tcpServer...\r\n");
        }


        socket(SOCK_TCPS,Sn_MR_TCP,g_w5500_local_port,Sn_MR_ND);           
        osDelay(2000);                                  //这里要延时1s 否则掉线后，连接太频繁，会网络卡死
        stateLastN=SOCK_CLOSED;
		break;
		
		case SOCK_INIT:									//socket处于初始化状态
        //我们是TCPSERVER 在这里监听 如果是TCPlink则再这里连接服务器          
        if(stateLastN!=SOCK_INIT)
        {
            App_Printf("$$W5500 tcpServer listen\r\n");
        }
        osDelay(100);//稍微延时
        stateLastN=SOCK_INIT;      
        listen(SOCK_TCPS);							    //socket建立监听
		break;
		
		case SOCK_ESTABLISHED: 						    //socket处于连接建立状态
        if(stateLastN!=SOCK_ESTABLISHED)
        {
            App_Printf("$$W5500 tcpServer Run\r\n");  
        }
        
        if(getSn_IR(SOCK_TCPS) & Sn_IR_CON)
        {
            setSn_IR(SOCK_TCPS, Sn_IR_CON); 		    //清除接收中断标志位
        }
    
        len=getSn_RX_RSR(SOCK_TCPS); 				    //定义len为已接收数据的长度
        
        //-------------------------------------------------
        //这里做一个限定 超过MB_TCP_BUF_SIZE个字节的都舍去
        if((len>0)&&(MB_TCP_BUF_SIZE>=len))
        {
            recv(SOCK_TCPS,(buff+3),len); 			    //接收来自Server的数据
        
            //$$W5500 Rece datLen=12r\n",len);
       
            buff[0]=(uint8_t)(len>>8);  			    //首位为消息的实际长度高字节
            buff[1]=(uint8_t)(len&0x00ff);   		    //第二位为消息的实际长度低字节
            buff[2]=1;  							    //第三位为1表示网口  为2表示wifi
             

            //##############################################################################################
            //互斥操作
            osMutexAcquire(PendingCachePointsMutexHandle, osWaitForever);   //一直等待获取互斥资源 
 
            if(8>g_netBridgeModbusQueueN) 
            {
                msg=g_netBridgeModbusQueueN;
                g_netBridgeModbusQueueTable[msg]=&buff[0];                  //网络桥接modbus队列指针存储
                g_netBridgeModbusQueueN++;
            }
            else
            {
                g_netBridgeModbusQueueN=0;
                msg=g_netBridgeModbusQueueN;
                g_netBridgeModbusQueueTable[msg]=&buff[0];                  //网络桥接modbus队列指针存储
                g_netBridgeModbusQueueN++;
            }
            osMutexRelease(PendingCachePointsMutexHandle);                  //释放互斥资源    
            //##############################################################################################
            
            osMessageQueuePut(netBridgeModbusQueueHandle,&msg, 0U, 100U);
            App_Printf("$$W5500 PutQueue->netBridgeModbus msg=%d\r\n",msg);

        }
        else
        {
             osDelay(2);
        }
        stateLastN=SOCK_ESTABLISHED;        
		break;
			
		case SOCK_CLOSE_WAIT: 											    //socket处于等待关闭状态           
		if(stateLastN!=SOCK_CLOSE_WAIT)
        {
           App_Printf("$$W5500 tcpServer close\r\n");  
        }
        close(SOCK_TCPS);       
        osDelay(1000);//稍微延时
        stateLastN=SOCK_CLOSE_WAIT;
		break;
        
        default:           
        break;

	}
}





