






//############################################################################################################################################
//cocoguojia ��װ2023.3.1
//����˵��
//���3·������
//############################################################################################################################################

////////////////////////////////////////////////////////////////////////
//����.h�ļ�
#include "DiabcTask.h"

////////////////////////////////////////////////////////////////////////
//������
osThreadId_t DiabcTask_NHandle;
////////////////////////////////////////////////////////////////////////
//��������
const osThreadAttr_t DiabcTask_N_attributes = {
  .name = "DiabcTask_N",
  .stack_size = DIABCTASK_stack_size,
  .priority = (osPriority_t) DIABCTASK_Priority,
};


////////////////////////////////////////////////////////////////////////
//����
void DiabcTask(void *argument)
{

  uint8_t lastDiStateFlag[4]={0,1,1,1};//1=��һ�ε�״̬ʱ�ߵ�ƽ  0=��һ�ε�״̬ʱ�͵�ƽ  �ϵ��ʱ��Ϊ�ߵ�ƽ
  osDelay(100);//�ϵ翪�������ܴ��ڻ���״̬ ��ʱ100ms ���ȶ�
  
  for(;;)
  {
     //--------------------------------
     //A·�����صĴ���ͳ��
     if(0==lastDiStateFlag[1])      //����ϴ��ǵ͵�ƽ״̬������������
     {
        if(DIAING)
        {
            g_diNumTalbe[1]++;      //���������� �����++
            lastDiStateFlag[1]=1;   //��ǰ״̬Ϊ�ߵ�ƽ
            g_holdingChangeFlag=1;
             //##############################################################################################
             //�������
             osMutexAcquire(ReadWriteRegHoldingMutexHandle, osWaitForever);   //һֱ�ȴ���ȡ������Դ 
             usRegHoldingBuf[5]=g_diNumTalbe[1];
             osMutexRelease(ReadWriteRegHoldingMutexHandle);                  //�ͷŻ�����Դ    
             //##############################################################################################        
        }    
     }
     else                          //����ϴ��Ǹߵ�ƽ״̬������͵�ƽ
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
    //B·�����صĴ���ͳ�� 
    if(0==lastDiStateFlag[2])
    {
        if(DIBING)
        {
            g_diNumTalbe[2]++;
            lastDiStateFlag[2]=1;
            g_holdingChangeFlag=1;
              //##############################################################################################
             //�������
             osMutexAcquire(ReadWriteRegHoldingMutexHandle, osWaitForever);   //һֱ�ȴ���ȡ������Դ 
             usRegHoldingBuf[6]=g_diNumTalbe[2];
             osMutexRelease(ReadWriteRegHoldingMutexHandle);                  //�ͷŻ�����Դ    
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
    //C·�����صĴ���ͳ�� 
    if(0==lastDiStateFlag[3])
    {
        if(DICING)
        {
            g_diNumTalbe[3]++;
            lastDiStateFlag[3]=1;
            g_holdingChangeFlag=1;
             //##############################################################################################
             //�������
             osMutexAcquire(ReadWriteRegHoldingMutexHandle, osWaitForever);   //һֱ�ȴ���ȡ������Դ 
             usRegHoldingBuf[7]=g_diNumTalbe[3];
             osMutexRelease(ReadWriteRegHoldingMutexHandle);                  //�ͷŻ�����Դ    
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





