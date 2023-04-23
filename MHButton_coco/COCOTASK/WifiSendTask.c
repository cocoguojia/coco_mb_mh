






//############################################################################################################################################
//cocoguojia ��װ2023.3.1
//����˵��
//
//############################################################################################################################################

////////////////////////////////////////////////////////////////////////
//����.h�ļ�
#include "WifiSendTask.h"

////////////////////////////////////////////////////////////////////////
//������
osThreadId_t WifiSendTask_NHandle;
////////////////////////////////////////////////////////////////////////
//��������
const osThreadAttr_t WifiSendTask_N_attributes = {
  .name = "WifiSendTask_N",
  .stack_size = WIFISENDTASK_stack_size,
  .priority = (osPriority_t) WIFISENDTASK_Priority,
};





//msg ���ⷢ�Ͷ���
//0xA1 ���� wdt h ��ѯ���

//0xB1 ���� w5500 h ��ѯ���
//0xB2 ���� w5500 info ��ѯ���


//0xC1 ���� wifi h ��ѯ���
//0xC2 ���� wifi info ��ѯ���

//0xD1 ���� clk h ��ѯ���
//0xD2 ���� clk view ��ѯ���


//g_wifiSendPingPongFLag




////////////////////////////////////////////////////////////////////////
//����
void WifiSendTask(void *argument)
{
    osStatus_t status;
//    HAL_StatusTypeDef statusDat=0;    
    uint16_t msg;
    uint16_t lenNum;
    char at_buff[20];

    for(;;)
    {
        //----------------------------------------------------------------------------------------
        //wifiҪ�������ݵ� ��Ϣ���� 
        status = osMessageQueueGet(wifiSendQueueHandle, &msg, NULL, osWaitForever);

        //----------------------------------------------------------------------------------------
        //��ȡwifiҪ�������ݵ���Ϣ���� ��
        if (status == osOK)
        {   
            //��������ƹ�һ���  msg��ֵ �ڽ�����Ϣ����֮ǰg_wifiSendPingPongFLag�Ѿ���msgͬ�� ָʾ�����ĸ�ƹ�� ������3��ƹ����
            // msg������һ��ʼ���� ��Ȼ���о�̬��Ϥ         
            if(TCP_WIFI_PINGPONG_N>msg)                                                           
            {
                 
                //--------------------------------------------------------------------------------------------------------------------------------------------------
                //Ϊ�˴���ָ��֮�� �����Դ��� ��Ϣ���� ����Ϣ������ ���������ﶨ�� ������
                lenNum=g_wifiSendbuff[msg][0];                                               //��1�ֽ�Ϊ���ȵĸ��ֽ�
                lenNum<<=8;
                lenNum+=g_wifiSendbuff[msg][1];                                              //��2�ֽ�Ϊ���ȵĵ��ֽ�
                
                //�ӵ�3���ֽڿ�ʼΪ ����Ҫ���͵�����
                if(TCP_WIFI_SENDLEN>=lenNum)
                {
                    sprintf((char*)at_buff,"AT+CIPSEND=0,%d",lenNum);
            
                    g_atk_mw8266d_send_CIPSEND_cmd(at_buff,"OK",100);  //����ָ�����ȵ�����
                    osDelay(50); 
                    HAL_UART_Transmit(&huart4,(&g_wifiSendbuff[msg][2]), lenNum, 10);
                    
//                     statusDat=HAL_UART_Transmit(&huart4,(&g_wifiSendbuff[msg][2]), lenNum, 10);
//                     do
//                     {
//                         statusDat=HAL_UART_Transmit(&huart4,(&g_wifiSendbuff[msg][2]), lenNum, HAL_MAX_DELAY);
//                         
//                         if(HAL_OK!=statusDat)
//                         {
//                            osDelay(200);
//                         }
//                     }
//                     while(HAL_OK!=statusDat);
                 }
                else
                {
                    ;
                }   
            }
            else if((0XA1==msg)||(0XB1==msg)||(0XB2==msg)||(0XC1==msg)||(0XC2==msg)||(0XD1==msg)||(0XD2==msg))
            {
                g_showCmdH(msg,2);
  
            }
            else
            {
                ;
            }
        }
    }

}





            
          

