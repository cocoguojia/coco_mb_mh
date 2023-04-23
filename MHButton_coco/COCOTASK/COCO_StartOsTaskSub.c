

//############################################################################################################################################
//cocoguojia ��װ2023.3.1
//����˵��
//���ļ�Ϊ����ϵͳ������������(��MXCUBE�Զ����ɵ���������)
//�ڱ�.h�ļ��� �й�����������ȼ��Ͷ�̬ջ��define ����ͳһ����ͷ���
//############################################################################################################################################

////////////////////////////////////////////////////////////////////////
//����.h�ļ�
#include "COCO_StartOsTaskSub.h"

//----------------------------------------------------------------------------------------------------------------
//�ѷ�mxcube���ɵ���������һ��
//�������� Ĭ������StartDefaultTask�� ���м���
//cubemx��ȱ������������һ��c�ļ���,�۵�
//�ѳ���Ĭ������֮����������񶼷ֱ����һ��c�ļ���,����ģ�黯
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
    ////creation of monitoring_N  ֻ�е��Ե�ʱ��ſ��������� �ܺ���Դ�ͺܺ�ʵʱ��
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


