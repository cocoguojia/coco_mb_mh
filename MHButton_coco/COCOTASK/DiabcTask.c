






//############################################################################################################################################
//cocoguojia 封装2023.3.1
//功能说明
//检测3路开关量
//############################################################################################################################################

////////////////////////////////////////////////////////////////////////
//本地.h文件
#include "DiabcTask.h"

////////////////////////////////////////////////////////////////////////
//任务句柄
osThreadId_t DiabcTask_NHandle;
////////////////////////////////////////////////////////////////////////
//任务属性
const osThreadAttr_t DiabcTask_N_attributes = {
  .name = "DiabcTask_N",
  .stack_size = DIABCTASK_stack_size,
  .priority = (osPriority_t) DIABCTASK_Priority,
};


////////////////////////////////////////////////////////////////////////
//任务
void DiabcTask(void *argument)
{

  uint8_t lastDiStateFlag[4]={0,1,1,1};//1=上一次的状态时高电平  0=上一次的状态时低电平  上电的时候为高电平
  osDelay(100);//上电开关量可能处于混沌状态 延时100ms 带稳定
  
  for(;;)
  {
     //--------------------------------
     //A路上升沿的次数统计
     if(0==lastDiStateFlag[1])      //如果上次是低电平状态，则检测上跳沿
     {
        if(DIAING)
        {
            g_diNumTalbe[1]++;      //存在上跳沿 则计数++
            lastDiStateFlag[1]=1;   //则当前状态为高电平
            g_holdingChangeFlag=1;
             //##############################################################################################
             //互斥操作
             osMutexAcquire(ReadWriteRegHoldingMutexHandle, osWaitForever);   //一直等待获取互斥资源 
             usRegHoldingBuf[5]=g_diNumTalbe[1];
             osMutexRelease(ReadWriteRegHoldingMutexHandle);                  //释放互斥资源    
             //##############################################################################################        
        }    
     }
     else                          //如果上次是高电平状态，则检测低电平
     {
        if(DIAING)
        {
            ;
        }
        else
        {
            lastDiStateFlag[1]=0;
        }            
     } 
     
    //--------------------------------
    //B路上升沿的次数统计 
    if(0==lastDiStateFlag[2])
    {
        if(DIBING)
        {
            g_diNumTalbe[2]++;
            lastDiStateFlag[2]=1;
            g_holdingChangeFlag=1;
              //##############################################################################################
             //互斥操作
             osMutexAcquire(ReadWriteRegHoldingMutexHandle, osWaitForever);   //一直等待获取互斥资源 
             usRegHoldingBuf[6]=g_diNumTalbe[2];
             osMutexRelease(ReadWriteRegHoldingMutexHandle);                  //释放互斥资源    
             //##############################################################################################  
        }    
    }
    else
    {
        if(DIBING)
        {
            ;
        }
        else
        {
            lastDiStateFlag[2]=0;
        }            
    } 
    
    //--------------------------------
    //C路上升沿的次数统计 
    if(0==lastDiStateFlag[3])
    {
        if(DICING)
        {
            g_diNumTalbe[3]++;
            lastDiStateFlag[3]=1;
            g_holdingChangeFlag=1;
             //##############################################################################################
             //互斥操作
             osMutexAcquire(ReadWriteRegHoldingMutexHandle, osWaitForever);   //一直等待获取互斥资源 
             usRegHoldingBuf[7]=g_diNumTalbe[3];
             osMutexRelease(ReadWriteRegHoldingMutexHandle);                  //释放互斥资源    
             //##############################################################################################  
        }    
    }
    else
    {
        if(DICING)
        {
            ;
        }
        else
        {
            lastDiStateFlag[3]=0;
        }            
    }            
    osDelay(1);
  }
}





