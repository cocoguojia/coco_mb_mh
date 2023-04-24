
//############################################################################################################################################
//cocoguojia 还没封装好 最乱的任务 2023.3.1
//功能说明
//检测5路按钮按下时间,为了保证实时性，用了扫描计数法，5路串联快扫，精度粗记为10ms
//############################################################################################################################################

////////////////////////////////////////////////////////////////////////
//本地.h文件
#include "WifiTask.h"

#include "atk_mw8266d_struct.h"

uint8_t atk_mw8266d_monitor_first1(void);

////////////////////////////////////////////////////////////////////////
//任务句柄
osThreadId_t WifiTask_NHandle;
////////////////////////////////////////////////////////////////////////
//任务属性
const osThreadAttr_t WifiTask_N_attributes = {
  .name = "WifiTask_N",
  .stack_size =WIFITASK_stack_size,
  .priority = (osPriority_t) WIFITASK_Priority,
};

////////////////////////////////////////////////////////////////////////
//任务
void WifiTask(void *argument)
{
    uint8_t runInitFlag=1;//1=即将要执行wifi模块初始化
    
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
                ret=g_atk_AT_MW8266_InitSub();        //wifi初始化 如果出现错误 返回0 
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
            
            g_openFreeModbusFlag=1;     //wifi初始化完成 置位 1=开启FreeModbus功能 
            App_Printf("$$wifi init is OK, modbus code En\r\n"); 
            osDelay(1000);
            
            g_atk_mw8266d_getApStaIp();
            
             
        }
        
        runInitFlag=g_atk_mw8266d_monitor();//当函数返回1 则表示出现重大错误 要重新初始化
        osDelay(100);
      
    }
}
