
//############################################################################################################################################
//cocoguojia ��û��װ�� ���ҵ����� 2023.3.1
//����˵��
//���5·��ť����ʱ��,Ϊ�˱�֤ʵʱ�ԣ�����ɨ���������5·������ɨ�����ȴּ�Ϊ10ms
//############################################################################################################################################

////////////////////////////////////////////////////////////////////////
//����.h�ļ�
#include "WifiTask.h"

#include "atk_mw8266d_struct.h"

uint8_t atk_mw8266d_monitor_first1(void);

////////////////////////////////////////////////////////////////////////
//������
osThreadId_t WifiTask_NHandle;
////////////////////////////////////////////////////////////////////////
//��������
const osThreadAttr_t WifiTask_N_attributes = {
  .name = "WifiTask_N",
  .stack_size =WIFITASK_stack_size,
  .priority = (osPriority_t) WIFITASK_Priority,
};

////////////////////////////////////////////////////////////////////////
//����
void WifiTask(void *argument)
{
    uint8_t runInitFlag=1;//1=����Ҫִ��wifiģ���ʼ��
    
    uint8_t ret=0;
    uint16_t i=1;


    
    HAL_GPIO_WritePin(WIFI_EN_GPIO_Port, WIFI_EN_Pin, GPIO_PIN_RESET);
    osDelay(500);
    HAL_GPIO_WritePin(WIFI_EN_GPIO_Port, WIFI_EN_Pin, GPIO_PIN_SET);
    osDelay(3000);

    for(;;)
    {
        if(1==runInitFlag)
        {
            runInitFlag=0;
            do{
                App_Printf("$$wifi init start for=%d\r\n",i);
                ret=g_atk_AT_MW8266_InitSub();        //wifi��ʼ�� ������ִ��� ����0 
                if(0!=ret)
                {  
                    App_Printf("$$wifi init Err delay=%ds\r\n",i*2); 
                    osDelay(i*2000);
                               
                    if(3<=i)
                    {
                        i=1;
                    }
                    else
                    {
                        i++;
                    }         
                }
            }
            while(0!=ret);
            
            g_openFreeModbusFlag=1;     //wifi��ʼ����� ��λ 1=����FreeModbus���� 
            App_Printf("$$wifi init is OK, modbus code En\r\n"); 
            osDelay(1000);
            
            g_atk_mw8266d_getApStaIp();
            
             
        }
        
        runInitFlag=g_atk_mw8266d_monitor();//����������1 ���ʾ�����ش���� Ҫ���³�ʼ��
        osDelay(100);
      
    }
}
