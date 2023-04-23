
//############################################################################################################################################
//cocoguojia 封装2023.3.1
//功能说明
//检测5路按钮,为了保证实时性，用了计数消抖法，5路串联快刷，消抖时间可设置(按下与抬起) KEY_DOWNTIME KEY_HOLDTIME
//############################################################################################################################################

////////////////////////////////////////////////////////////////////////
//本地.h文件
#include "KeyTask.h"

//////////////////////////////////////////////////////////////////////////////////////////
//任务句柄
osThreadId_t KeyTask_NHandle;
//////////////////////////////////////////////////////////////////////////////////////////
//任务属性
const osThreadAttr_t KeyTask_N_attributes = {
  .name = "KeyTask_N",
  .stack_size = KEYTASK_stack_size,
  .priority = (osPriority_t) KEYTASK_Priority,
};

//////////////////////////////////////////////////////////////////////////////////////////
//内部预定义
//-------------------------------------
//检测按钮按下消抖时间 单位ms
#define KEY_DOWNTIME 5
//-------------------------------------
//检测按钮抬起消抖时间 单位ms
#define KEY_HOLDTIME 50


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//内部变量
uint8_t i_keyTable[6]={0,0,0,0,0,0};//按下1ms就++ 抬起1ms就- - 注意为了方便思维[0]元素没有用 下同 不在重复
uint8_t i_keyState[6]={0,0,0,0,0,0};//0=还没被按下  1=被按下


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//静态函数
//------------------------------
//分别为5个按键检测的函数
static void Key1Test(void);
static void Key2Test(void);
static void Key3Test(void);
static void Key4Test(void);
static void Key5Test(void);


//////////////////////////////////////////////////////////////////////////////////////////
//任务
void KeyTask(void *argument)
{
  for(;;)
  {
      //--------------------------------------------------------------------------------
      //分别检测5个按键当前的状态 并且配合 按键计时任务完成对按下累计时长的统计
      Key1Test();
      Key2Test();
      Key3Test();
      Key4Test();
      Key5Test();
      
      osDelay(1);
  }
}

//////////////////////////////////////////////////////////////////////////////////////////
//静态函数
static void Key1Test(void)
{

    uint8_t n=1;
    if(0==i_keyState[n])
    {
      if(KEY1ING)
      {
           i_keyTable[n]++;
          if(KEY_DOWNTIME<=i_keyTable[n])
          {
              g_keyStateTalbe[n]=1;
              i_keyTable[n]=KEY_HOLDTIME;
              i_keyState[n]=1; 
              g_holdingChangeFlag=1;
             //##############################################################################################
             //互斥操作
             osMutexAcquire(ReadWriteRegHoldingMutexHandle, osWaitForever);   //一直等待获取互斥资源 
             usRegHoldingBuf[0]=g_keyStateTalbe[n];
             osMutexRelease(ReadWriteRegHoldingMutexHandle);                  //释放互斥资源    
             //##############################################################################################
          }
      }
      else
      {
          if(0<i_keyTable[n])
          {
                i_keyTable[n]--;
          }
           else
          {
               i_keyState[n]=0; 
          }
      }
    }
    else
    {
      if(KEY1ING)
      {
          ;
      }
      else
      {
          if(0<i_keyTable[n])
          {
                i_keyTable[n]--;
          }
           else
          {
              g_keyStateTalbe[n]=0;
               i_keyState[n]=0;
              g_holdingChangeFlag=1;
             //##############################################################################################
             //互斥操作
             osMutexAcquire(ReadWriteRegHoldingMutexHandle, osWaitForever);   //一直等待获取互斥资源 
             usRegHoldingBuf[0]=g_keyStateTalbe[n];
             osMutexRelease(ReadWriteRegHoldingMutexHandle);                  //释放互斥资源    
             //##############################################################################################              
          }
      }  
    }
}

void static Key2Test(void)
{
     uint8_t n=2;
    if(0==i_keyState[n])
    {
      if(KEY2ING)
      {
           i_keyTable[n]++;
          if(KEY_DOWNTIME<=i_keyTable[n])
          {
              g_keyStateTalbe[n]=1;
              i_keyTable[n]=KEY_HOLDTIME;
              i_keyState[n]=1;
              g_holdingChangeFlag=1;
             //##############################################################################################
             //互斥操作
             osMutexAcquire(ReadWriteRegHoldingMutexHandle, osWaitForever);   //一直等待获取互斥资源 
             usRegHoldingBuf[1]=g_keyStateTalbe[n];
             osMutexRelease(ReadWriteRegHoldingMutexHandle);                  //释放互斥资源    
             //##############################################################################################                 
          }
      }
      else
      {
          if(0<i_keyTable[n])
          {
                i_keyTable[n]--;
          }
           else
          {
               i_keyState[n]=0; 
          }
      }
    }
    else
    {
      if(KEY2ING)
      {
          ;
      }
      else
      {
          if(0<i_keyTable[n])
          {
                i_keyTable[n]--;
          }
           else
          {
              g_keyStateTalbe[n]=0;
               i_keyState[n]=0; 
              g_holdingChangeFlag=1;
             //##############################################################################################
             //互斥操作
             osMutexAcquire(ReadWriteRegHoldingMutexHandle, osWaitForever);   //一直等待获取互斥资源 
             usRegHoldingBuf[1]=g_keyStateTalbe[n];
             osMutexRelease(ReadWriteRegHoldingMutexHandle);                  //释放互斥资源    
             //##############################################################################################    
          }
      }  
    }
}

void static Key3Test(void)
{
     uint8_t n=3;
    if(0==i_keyState[n])
    {
      if(KEY3ING)
      {
           i_keyTable[n]++;
          if(KEY_DOWNTIME<=i_keyTable[n])
          {
              g_keyStateTalbe[n]=1;
              i_keyTable[n]=KEY_HOLDTIME;
              i_keyState[n]=1; 
              g_holdingChangeFlag=1;
              //##############################################################################################
             //互斥操作
             osMutexAcquire(ReadWriteRegHoldingMutexHandle, osWaitForever);   //一直等待获取互斥资源 
             usRegHoldingBuf[2]=g_keyStateTalbe[n];
             osMutexRelease(ReadWriteRegHoldingMutexHandle);                  //释放互斥资源    
             //##############################################################################################    
          }
      }
      else
      {
          if(0<i_keyTable[n])
          {
                i_keyTable[n]--;
          }
           else
          {
               i_keyState[n]=0; 
          }
      }
    }
    else
    {
      if(KEY3ING)
      {
          ;
      }
      else
      {
          if(0<i_keyTable[n])
          {
                i_keyTable[n]--;
          }
           else
          {
              g_keyStateTalbe[n]=0;
               i_keyState[n]=0;
              g_holdingChangeFlag=1;
            //##############################################################################################
             //互斥操作
             osMutexAcquire(ReadWriteRegHoldingMutexHandle, osWaitForever);   //一直等待获取互斥资源 
             usRegHoldingBuf[2]=g_keyStateTalbe[n];
             osMutexRelease(ReadWriteRegHoldingMutexHandle);                  //释放互斥资源    
             //##############################################################################################                  
          }
      }  
    }
}

void static Key4Test(void)
{
     uint8_t n=4;
    if(0==i_keyState[n])
    {
      if(KEY4ING)
      {
           i_keyTable[n]++;
          if(KEY_DOWNTIME<=i_keyTable[n])
          {
              g_keyStateTalbe[n]=1;
              i_keyTable[n]=KEY_HOLDTIME;
              i_keyState[n]=1; 
              g_holdingChangeFlag=1;
               //##############################################################################################
             //互斥操作
             osMutexAcquire(ReadWriteRegHoldingMutexHandle, osWaitForever);   //一直等待获取互斥资源 
             usRegHoldingBuf[3]=g_keyStateTalbe[n];
             osMutexRelease(ReadWriteRegHoldingMutexHandle);                  //释放互斥资源    
             //##############################################################################################    
          }
      }
      else
      {
          if(0<i_keyTable[n])
          {
                i_keyTable[n]--;
          }
           else
          {
               i_keyState[n]=0; 
          }
      }
    }
    else
    {
      if(KEY4ING)
      {
          ;
      }
      else
      {
          if(0<i_keyTable[n])
          {
                i_keyTable[n]--;
          }
           else
          {
              g_keyStateTalbe[n]=0;
               i_keyState[n]=0; 
              g_holdingChangeFlag=1;
             //##############################################################################################
             //互斥操作
             osMutexAcquire(ReadWriteRegHoldingMutexHandle, osWaitForever);   //一直等待获取互斥资源 
             usRegHoldingBuf[3]=g_keyStateTalbe[n];
             osMutexRelease(ReadWriteRegHoldingMutexHandle);                  //释放互斥资源    
             //##############################################################################################    
          }
      }  
    }
}

void static Key5Test(void)
{
    uint8_t n=5;
    if(0==i_keyState[n])
    {
      if(KEY5ING)
      {
           i_keyTable[n]++;
          if(KEY_DOWNTIME<=i_keyTable[n])
          {
              g_keyStateTalbe[n]=1;
              i_keyTable[n]=KEY_HOLDTIME;
              i_keyState[n]=1; 
              g_holdingChangeFlag=1;
               //##############################################################################################
             //互斥操作
             osMutexAcquire(ReadWriteRegHoldingMutexHandle, osWaitForever);   //一直等待获取互斥资源 
             usRegHoldingBuf[4]=g_keyStateTalbe[n];
             osMutexRelease(ReadWriteRegHoldingMutexHandle);                  //释放互斥资源    
             //##############################################################################################    
          }
      }
      else
      {
          if(0<i_keyTable[n])
          {
                i_keyTable[n]--;
          }
           else
          {
               i_keyState[n]=0; 
          }
      }
    }
    else
    {
      if(KEY5ING)
      {
          ;
      }
      else
      {
          if(0<i_keyTable[n])
          {
                i_keyTable[n]--;
          }
           else
          {
              g_keyStateTalbe[n]=0;
               i_keyState[n]=0; 
              g_holdingChangeFlag=1;
             //##############################################################################################
             //互斥操作
             osMutexAcquire(ReadWriteRegHoldingMutexHandle, osWaitForever);   //一直等待获取互斥资源 
             usRegHoldingBuf[4]=g_keyStateTalbe[n];
             osMutexRelease(ReadWriteRegHoldingMutexHandle);                  //释放互斥资源    
             //##############################################################################################    
          }
      }  
    }
}


