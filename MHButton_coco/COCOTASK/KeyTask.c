
//############################################################################################################################################
//cocoguojia ��װ2023.3.1
//����˵��
//���5·��ť,Ϊ�˱�֤ʵʱ�ԣ����˼�����������5·������ˢ������ʱ�������(������̧��) KEY_DOWNTIME KEY_HOLDTIME
//############################################################################################################################################

////////////////////////////////////////////////////////////////////////
//����.h�ļ�
#include "KeyTask.h"

//////////////////////////////////////////////////////////////////////////////////////////
//������
osThreadId_t KeyTask_NHandle;
//////////////////////////////////////////////////////////////////////////////////////////
//��������
const osThreadAttr_t KeyTask_N_attributes = {
  .name = "KeyTask_N",
  .stack_size = KEYTASK_stack_size,
  .priority = (osPriority_t) KEYTASK_Priority,
};

//////////////////////////////////////////////////////////////////////////////////////////
//�ڲ�Ԥ����
//-------------------------------------
//��ⰴť��������ʱ�� ��λms
#define KEY_DOWNTIME 5
//-------------------------------------
//��ⰴţ̌������ʱ�� ��λms
#define KEY_HOLDTIME 50


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//�ڲ�����
uint8_t i_keyTable[6]={0,0,0,0,0,0};//����1ms��++ ̧��1ms��- - ע��Ϊ�˷���˼ά[0]Ԫ��û���� ��ͬ �����ظ�
uint8_t i_keyState[6]={0,0,0,0,0,0};//0=��û������  1=������


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//��̬����
//------------------------------
//�ֱ�Ϊ5���������ĺ���
static void Key1Test(void);
static void Key2Test(void);
static void Key3Test(void);
static void Key4Test(void);
static void Key5Test(void);


//////////////////////////////////////////////////////////////////////////////////////////
//����
void KeyTask(void *argument)
{
  for(;;)
  {
      //--------------------------------------------------------------------------------
      //�ֱ���5��������ǰ��״̬ ������� ������ʱ������ɶ԰����ۼ�ʱ����ͳ��
      Key1Test();
      Key2Test();
      Key3Test();
      Key4Test();
      Key5Test();
      
      osDelay(1);
  }
}

//////////////////////////////////////////////////////////////////////////////////////////
//��̬����
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
             //�������
             osMutexAcquire(ReadWriteRegHoldingMutexHandle, osWaitForever);   //һֱ�ȴ���ȡ������Դ 
             usRegHoldingBuf[0]=g_keyStateTalbe[n];
             osMutexRelease(ReadWriteRegHoldingMutexHandle);                  //�ͷŻ�����Դ    
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
             //�������
             osMutexAcquire(ReadWriteRegHoldingMutexHandle, osWaitForever);   //һֱ�ȴ���ȡ������Դ 
             usRegHoldingBuf[0]=g_keyStateTalbe[n];
             osMutexRelease(ReadWriteRegHoldingMutexHandle);                  //�ͷŻ�����Դ    
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
             //�������
             osMutexAcquire(ReadWriteRegHoldingMutexHandle, osWaitForever);   //һֱ�ȴ���ȡ������Դ 
             usRegHoldingBuf[1]=g_keyStateTalbe[n];
             osMutexRelease(ReadWriteRegHoldingMutexHandle);                  //�ͷŻ�����Դ    
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
             //�������
             osMutexAcquire(ReadWriteRegHoldingMutexHandle, osWaitForever);   //һֱ�ȴ���ȡ������Դ 
             usRegHoldingBuf[1]=g_keyStateTalbe[n];
             osMutexRelease(ReadWriteRegHoldingMutexHandle);                  //�ͷŻ�����Դ    
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
             //�������
             osMutexAcquire(ReadWriteRegHoldingMutexHandle, osWaitForever);   //һֱ�ȴ���ȡ������Դ 
             usRegHoldingBuf[2]=g_keyStateTalbe[n];
             osMutexRelease(ReadWriteRegHoldingMutexHandle);                  //�ͷŻ�����Դ    
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
             //�������
             osMutexAcquire(ReadWriteRegHoldingMutexHandle, osWaitForever);   //һֱ�ȴ���ȡ������Դ 
             usRegHoldingBuf[2]=g_keyStateTalbe[n];
             osMutexRelease(ReadWriteRegHoldingMutexHandle);                  //�ͷŻ�����Դ    
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
             //�������
             osMutexAcquire(ReadWriteRegHoldingMutexHandle, osWaitForever);   //һֱ�ȴ���ȡ������Դ 
             usRegHoldingBuf[3]=g_keyStateTalbe[n];
             osMutexRelease(ReadWriteRegHoldingMutexHandle);                  //�ͷŻ�����Դ    
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
             //�������
             osMutexAcquire(ReadWriteRegHoldingMutexHandle, osWaitForever);   //һֱ�ȴ���ȡ������Դ 
             usRegHoldingBuf[3]=g_keyStateTalbe[n];
             osMutexRelease(ReadWriteRegHoldingMutexHandle);                  //�ͷŻ�����Դ    
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
             //�������
             osMutexAcquire(ReadWriteRegHoldingMutexHandle, osWaitForever);   //һֱ�ȴ���ȡ������Դ 
             usRegHoldingBuf[4]=g_keyStateTalbe[n];
             osMutexRelease(ReadWriteRegHoldingMutexHandle);                  //�ͷŻ�����Դ    
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
             //�������
             osMutexAcquire(ReadWriteRegHoldingMutexHandle, osWaitForever);   //һֱ�ȴ���ȡ������Դ 
             usRegHoldingBuf[4]=g_keyStateTalbe[n];
             osMutexRelease(ReadWriteRegHoldingMutexHandle);                  //�ͷŻ�����Դ    
             //##############################################################################################    
          }
      }  
    }
}


