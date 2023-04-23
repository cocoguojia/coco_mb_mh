

//############################################################################################################################################
//cocoguojia 封装2023.3.1
//功能说明
//本文件为操作系统任务启动函数(非MXCUBE自动生成的任务启动)
//在本.h文件里 有关于任务的优先级和动态栈的define 便于统一管理和分配
//############################################################################################################################################

////////////////////////////////////////////////////////////////////////
//本地.h文件
#include "COCO_StartOsTaskSub.h"

//----------------------------------------------------------------------------------------------------------------
//把非mxcube生成的任务启动一下
//函数放在 默认任务StartDefaultTask里 运行即可
//cubemx的缺点是任务都整在一个c文件里,槽点
//把除了默认任务之外的其他任务都分别放在一个c文件里,便于模块化
void COCO_MX_FREERTOS_Init(void) 
{

    uint8_t ret=NULL;
        
  //-----------------------------------------------------------
  //Create the thread(s)


  // creation of KeyTask_N
  KeyTask_NHandle = osThreadNew(KeyTask, NULL, &KeyTask_N_attributes);
  if(NULL==KeyTask_NHandle)
  {
      ret++;
  }

  //creation of DiabcTask_N
  DiabcTask_NHandle = osThreadNew(DiabcTask, NULL, &DiabcTask_N_attributes);
  if(NULL==DiabcTask_NHandle)
  {
      ret++;
  }

  //creation of KeyTimerTask_N
  KeyTimerTask_NHandle = osThreadNew(KeyTimerTask, NULL, &KeyTimerTask_N_attributes);
  if(NULL==KeyTimerTask_NHandle)
  {
      ret++;
  }
    
  //creation of NetBridgeModbus_N
  NetBridgeModbus_NHandle = osThreadNew(NetBridgeModbus, NULL, &NetBridgeModbus_N_attributes);
  if(NULL==NetBridgeModbus_NHandle)
  {
      ret++;
  }
  
  if(1==g_w5500EnFlag)  
  {
      //creation of W5500Task_N
      W5500Task_NHandle = osThreadNew(W5500Task, NULL, &W5500Task_N_attributes);
      if(NULL==W5500Task_NHandle)
      {
          ret++;
      }
      //creation of W5500SendTask_N
      W5500SendTask_NHandle = osThreadNew(W5500SendTask, NULL, &W5500SendTask_N_attributes);
      if(NULL==W5500SendTask_NHandle)
      {
          ret++;
      }
  }

  if(1==g_wifiEnFlag)  
  {     
      //creation of WifiTask_N
      WifiTask_NHandle = osThreadNew(WifiTask, NULL, &WifiTask_N_attributes);    
      if(NULL==WifiTask_NHandle)
      {
          ret++;
      }

      //creation of WifiSendTask_N
      WifiSendTask_NHandle = osThreadNew(WifiSendTask, NULL, &WifiSendTask_N_attributes);
      if(NULL==WifiSendTask_NHandle)
      {
          ret++;
      }
  }
  
      
  //creation of PowerDownDoTask_N
  PowerDownDoTask_NHandle = osThreadNew(PowerDownDoTask, NULL, &PowerDownDoTask_N_attributes);
  if(NULL==PowerDownDoTask_NHandle)
  {
      ret++;
  }
  
 //creation of eMBPoll_N
  eMBPollTask_NHandle = osThreadNew(eMBPollTask, NULL, &eMBPollTask_N_attributes);
  if(NULL==eMBPollTask_NHandle)
  {
      ret++;
  }

    //--------------------------------------------------------------------------------------------
    ////creation of monitoring_N  只有调试的时候才开启此任务 很耗资源和很耗实时性
    //monitoringTask_NHandle = osThreadNew(monitoringTask, NULL, &monitoringTask_N_attributes);
    //if(NULL==monitoringTask_NHandle)
    //{
    //  ret++;
    //}
  
  
  if(NULL!=ret)
  {
     App_Printf("$$####### CreatrTask is Err #######\r\n");
  }
  else
  {
     App_Printf("$$@@@@@@@ CreatrTask is OK @@@@@@@\r\n");
  }

}


