
//############################################################################################################################################
//cocoguojia ��װ2023.4.6
//����˵��
//����EXIT�жϷ������ź� ����������紦���¼�
//############################################################################################################################################

////////////////////////////////////////////////////////////////////////
//����.h�ļ�
#include "powerDownDoTask.h"

////////////////////////////////////////////////////////////////////////
//������
osThreadId_t PowerDownDoTask_NHandle;
////////////////////////////////////////////////////////////////////////
//��������
const osThreadAttr_t PowerDownDoTask_N_attributes = {
  .name = "PowerDownDoTask_N",
  .stack_size = POWERDOWNDOTASK_stack_size,
  .priority = (osPriority_t) POWERDOWNDTASK_Priority,
};



uint8_t save_Buffer[20];


void PowerDownFrontSaveParameter(void);

////////////////////////////////////////////////////////////////////////
//����
void PowerDownDoTask(void *argument)
{

    uint8_t i=6;
    osDelay(500);

    for(;;)
    {      
        if(GPIO_PIN_SET==HAL_GPIO_ReadPin(POWER_CHECK_GPIO_Port,POWER_CHECK_Pin))
        {
           osDelay(1);//1ms������Ŷ
           if(GPIO_PIN_SET==HAL_GPIO_ReadPin(POWER_CHECK_GPIO_Port,POWER_CHECK_Pin))
           {              
               if(1==g_holdingChangeFlag)
               {      
                   PowerDownFrontSaveParameter();
                   g_holdingChangeFlag=0; 
                   //App_Printf("$$POWEN DOWN,Save Parameter is OK1\r\n"); 
               }
                            
              //---------------------------------------------------------------------------
              //������ С��ָʾһ�� ������˸10��
              i=10;
              while(i--)
              {
                    HAL_GPIO_WritePin(LEDB_GPIO_Port, LEDB_Pin, GPIO_PIN_RESET);
                    osDelay(5);
                    HAL_GPIO_WritePin(LEDB_GPIO_Port, LEDB_Pin, GPIO_PIN_SET);
                    osDelay(40);
              }
                            
              #ifdef POWER_BATTERY_MANAGEMENT
              POWER_BATTERY_NO;//�ص�﮵��
              #else
              //---------------------------------------------------------------------------
              //��λ
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
    //�������
    osMutexAcquire(ReadWriteRegHoldingMutexHandle, osWaitForever);   //һֱ�ȴ���ȡ������Դ 
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
    osMutexRelease(ReadWriteRegHoldingMutexHandle);                  //�ͷŻ�����Դ    
    //##############################################################################################  
    
    //##############################################################################################
    //�������
    osMutexAcquire(W25q64MutexHandle, osWaitForever);   //һֱ�ȴ���ȡ������Դ 
    //W25Q128_Erase_Sector(WQ_PAGE_SW5ROAD3); //�ϵ��Ѿ����������Ѿ�ȫΪ0XFFFF ���ò��� 
    W25Q128_Write(&save_Buffer[0],WQ_PAGE_SW5ROAD3,16); //ʱ��� ��8��16λ һ��16���ֽ�
    osMutexRelease(W25q64MutexHandle);                  //�ͷŻ�����Դ    
    //##############################################################################################    
 
}








