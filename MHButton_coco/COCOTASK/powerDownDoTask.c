
//############################################################################################################################################
//cocoguojia 封装2023.4.6
//功能说明
//接收EXIT中断发来的信号 紧急处理掉电处理事件
//############################################################################################################################################

////////////////////////////////////////////////////////////////////////
//本地.h文件
#include "powerDownDoTask.h"

////////////////////////////////////////////////////////////////////////
//任务句柄
osThreadId_t PowerDownDoTask_NHandle;
////////////////////////////////////////////////////////////////////////
//任务属性
const osThreadAttr_t PowerDownDoTask_N_attributes = {
  .name = "PowerDownDoTask_N",
  .stack_size = POWERDOWNDOTASK_stack_size,
  .priority = (osPriority_t) POWERDOWNDTASK_Priority,
};



uint8_t save_Buffer[20];


void PowerDownFrontSaveParameter(void);

////////////////////////////////////////////////////////////////////////
//任务
void PowerDownDoTask(void *argument)
{

    uint8_t i=6;
    osDelay(500);

    for(;;)
    {      
        if(GPIO_PIN_SET==HAL_GPIO_ReadPin(POWER_CHECK_GPIO_Port,POWER_CHECK_Pin))
        {
           osDelay(1);//1ms的消抖哦
           if(GPIO_PIN_SET==HAL_GPIO_ReadPin(POWER_CHECK_GPIO_Port,POWER_CHECK_Pin))
           {              
               if(1==g_holdingChangeFlag)
               {      
                   PowerDownFrontSaveParameter();
                   g_holdingChangeFlag=0; 
                   //App_Printf("$$POWEN DOWN,Save Parameter is OK1\r\n"); 
               }
                            
              //---------------------------------------------------------------------------
              //掉电了 小灯指示一下 弱亮闪烁10次
              i=10;
              while(i--)
              {
                    HAL_GPIO_WritePin(LEDB_GPIO_Port, LEDB_Pin, GPIO_PIN_RESET);
                    osDelay(5);
                    HAL_GPIO_WritePin(LEDB_GPIO_Port, LEDB_Pin, GPIO_PIN_SET);
                    osDelay(40);
              }
                            
              #ifdef POWER_BATTERY_MANAGEMENT
              POWER_BATTERY_NO;//关掉锂电池
              #else
              //---------------------------------------------------------------------------
              //复位
              taskENTER_CRITICAL();
              HAL_NVIC_SystemReset();
              taskEXIT_CRITICAL();
              #endif
              
              osDelay(3000);
             
           }
        }           
        osDelay(2);
    }
}


void PowerDownFrontSaveParameter(void)
{
    uint8_t m;
  
    //##############################################################################################
    //互斥操作
    osMutexAcquire(ReadWriteRegHoldingMutexHandle, osWaitForever);   //一直等待获取互斥资源 
    for(m=0;m<6;)
    {
        save_Buffer[m]=(uint8_t)(g_diNumTalbe[m/2+1]>>8);
          
        m++;
        save_Buffer[m]=(uint8_t)(g_diNumTalbe[m/2+1]&0x00ff);
        m++;
    }
    for(m=0;m<10;)
    {
        save_Buffer[m+6]=(uint8_t)(g_keyTimeTalbe[(m/2)+1]>>8);
        m++;
        save_Buffer[m+6]=(uint8_t)(g_keyTimeTalbe[(m/2)+1]&0x00ff);
        m++;
    }          
    osMutexRelease(ReadWriteRegHoldingMutexHandle);                  //释放互斥资源    
    //##############################################################################################  
    
    //##############################################################################################
    //互斥操作
    osMutexAcquire(W25q64MutexHandle, osWaitForever);   //一直等待获取互斥资源 
    //W25Q128_Erase_Sector(WQ_PAGE_SW5ROAD3); //上电已经擦除或者已经全为0XFFFF 不用擦除 
    W25Q128_Write(&save_Buffer[0],WQ_PAGE_SW5ROAD3,16); //时间紧 共8个16位 一共16个字节
    osMutexRelease(W25q64MutexHandle);                  //释放互斥资源    
    //##############################################################################################    
 
}








