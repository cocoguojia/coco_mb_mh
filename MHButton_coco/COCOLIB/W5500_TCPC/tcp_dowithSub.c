/**
************************************************************************************************
* @file   		tcp_demo.c
* @author  		WIZnet Software Team 
* @version 		V1.0
* @date    		2015-02-14
* @brief   		TCP ��ʾ����
* @attention  
************************************************************************************************
**/



#include "tcp_dowithSub.h"

#include "w5500.h"
#include "W5500_conf.h"
#include "socket.h"
#include "utility.h"

uint8_t buff[MB_TCP_BUF_SIZE+3];				                    /*����һ������*/

//----------------------------------------------------------------------------------------------------------------------------
//ʵ�ʴ�����
void TcpServerDoWith(void)
{	
    static uint8_t stateLastN=0;
    uint16 len=0;	
    uint16 msg;
	switch(getSn_SR(SOCK_TCPS))						    //��ȡsocket��״̬
	{
		case SOCK_CLOSED:								//socket���ڹر�״̬
        if(stateLastN!=SOCK_CLOSED)
        {
            App_Printf("$$W5500 tcpServer...\r\n");
        }


        socket(SOCK_TCPS,Sn_MR_TCP,g_w5500_local_port,Sn_MR_ND);           
        osDelay(2000);                                  //����Ҫ��ʱ1s ������ߺ�����̫Ƶ���������翨��
        stateLastN=SOCK_CLOSED;
		break;
		
		case SOCK_INIT:									//socket���ڳ�ʼ��״̬
        //������TCPSERVER ��������� �����TCPlink�����������ӷ�����          
        if(stateLastN!=SOCK_INIT)
        {
            App_Printf("$$W5500 tcpServer listen\r\n");
        }
        osDelay(100);//��΢��ʱ
        stateLastN=SOCK_INIT;      
        listen(SOCK_TCPS);							    //socket��������
		break;
		
		case SOCK_ESTABLISHED: 						    //socket�������ӽ���״̬
        if(stateLastN!=SOCK_ESTABLISHED)
        {
            App_Printf("$$W5500 tcpServer Run\r\n");  
        }
        
        if(getSn_IR(SOCK_TCPS) & Sn_IR_CON)
        {
            setSn_IR(SOCK_TCPS, Sn_IR_CON); 		    //��������жϱ�־λ
        }
    
        len=getSn_RX_RSR(SOCK_TCPS); 				    //����lenΪ�ѽ������ݵĳ���
        
        //-------------------------------------------------
        //������һ���޶� ����MB_TCP_BUF_SIZE���ֽڵĶ���ȥ
        if((len>0)&&(MB_TCP_BUF_SIZE>=len))
        {
            recv(SOCK_TCPS,(buff+3),len); 			    //��������Server������
        
            //$$W5500 Rece datLen=12r\n",len);
       
            buff[0]=(uint8_t)(len>>8);  			    //��λΪ��Ϣ��ʵ�ʳ��ȸ��ֽ�
            buff[1]=(uint8_t)(len&0x00ff);   		    //�ڶ�λΪ��Ϣ��ʵ�ʳ��ȵ��ֽ�
            buff[2]=1;  							    //����λΪ1��ʾ����  Ϊ2��ʾwifi
             

            //##############################################################################################
            //�������
            osMutexAcquire(PendingCachePointsMutexHandle, osWaitForever);   //һֱ�ȴ���ȡ������Դ 
 
            if(8>g_netBridgeModbusQueueN) 
            {
                msg=g_netBridgeModbusQueueN;
                g_netBridgeModbusQueueTable[msg]=&buff[0];                  //�����Ž�modbus����ָ��洢
                g_netBridgeModbusQueueN++;
            }
            else
            {
                g_netBridgeModbusQueueN=0;
                msg=g_netBridgeModbusQueueN;
                g_netBridgeModbusQueueTable[msg]=&buff[0];                  //�����Ž�modbus����ָ��洢
                g_netBridgeModbusQueueN++;
            }
            osMutexRelease(PendingCachePointsMutexHandle);                  //�ͷŻ�����Դ    
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
			
		case SOCK_CLOSE_WAIT: 											    //socket���ڵȴ��ر�״̬           
		if(stateLastN!=SOCK_CLOSE_WAIT)
        {
           App_Printf("$$W5500 tcpServer close\r\n");  
        }
        close(SOCK_TCPS);       
        osDelay(1000);//��΢��ʱ
        stateLastN=SOCK_CLOSE_WAIT;
		break;
        
        default:           
        break;

	}
}





