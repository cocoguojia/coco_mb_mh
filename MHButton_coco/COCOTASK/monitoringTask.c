
//############################################################################################################################################
//cocoguojia ��װ2023.4.11
//����˵��
//
//############################################################################################################################################

////////////////////////////////////////////////////////////////////////
//����.h�ļ�
#include "monitoringTask.h"

//------------------------------------------------------------------------------
//TaskName	            Priority	TaskNumber	MinimumFreeStack(4Bity)	
//G:monitoringTask_N		19		    14		    96
//G:PowerDownDoTask_N		18		    12		    104
//G:eMBPollTask_N		    17		    13		    90
//G:KeyTask_N		        16		    4		    104
//G:DiabcTask_N		        15		    5		    102
//G:W5500Task_N		        9		    8		    94
//G:IDLE		            0		    2		    111
//G:defaultTask		        8		    1		    96
//G:WifiTask_N		        10		    10		    82
//G:KeyTimerTask_N		    14		    6		    100
//G:NetBridgeModbus_N		11		    7		    96
//G:W5500SendTask_N		    12		    9		    86
//G:Tmr Svc		            2		    3		    102
//G:WifiSendTask_N		    13		    11		    86

//////////////////////////////////////////////////////////////////////////////////////////
//������
osThreadId_t monitoringTask_NHandle;
//////////////////////////////////////////////////////////////////////////////////////////
//��������
const osThreadAttr_t monitoringTask_N_attributes = {
  .name = "monitoringTask_N",
  .stack_size = MONITORINGTASK_stack_size,
  .priority = (osPriority_t) MONITORINGTASK_Priority,
};

uint32_t theTaskNum=0;
TaskStatus_t * StatusArray;


void ShowTaskState(void);

   

//////////////////////////////////////////////////////////////////////////////////////////
//����
void monitoringTask(void *argument)
{  

    osDelay(10000);
    theTaskNum=osThreadGetCount();//��ȡ��������
     //Ϊ��������������Ա��� �ṹ��TaskStatus_t���ڴ�ռ�
    StatusArray=pvPortMalloc(theTaskNum*sizeof(TaskStatus_t));
    for(;;)
    {
        ShowTaskState();
        osDelay(5000);
    }
}

void ShowTaskState(void)
{
    uint32_t TotalRunTime;
    uint8_t x;
    static uint8_t n=0;
    TaskHandle_t TaskHandle;
//    TaskStatus_t TaskStatus;

    if(NULL!=StatusArray)//�������ɹ� �����Ϊ��
    {
        //ע��TotalRunTimeֵ��һֱΪ0 Ҫ����������ʱ�� ���Լ�д��ض�ʱ���Ĵ��� ������ٶ������Դ
        theTaskNum=uxTaskGetSystemState(StatusArray,theTaskNum,&TotalRunTime);
        
        //�û����ź�������֤printfҲ���Ǵ���3��Ӳ����Դ��͸����(ͬһʱ��Ӳ��ֻ��һ������ռ��)        

        App_Printf("TaskName\tPriority\tTaskNumber\tMinimumFreeStack(4Bity)\t\r\n");

        
        for(x=0;x<theTaskNum;x++)
        {
            //xTaskNumber�������� ��1��ʼ���� ���մ��������˳���Զ�����
            //��Ϊ�������� vTaskStartScheduler()֮ǰ������3������ ���Կ�������ı��Ϊ4            
            //usStackHighWaterMark�ĵ�λ����  

            App_Printf("G:%s\t\t%d\t\t%d\t\t%d\r\n",
                    StatusArray[x].pcTaskName,
                    (int)StatusArray[x].uxCurrentPriority,
                    (int)StatusArray[x].xTaskNumber,
                    (int)StatusArray[x].usStackHighWaterMark);
        }
    }
    
    //������ʾ4���������Ϣ
    switch(n)
    {
        case 0:
        TaskHandle=xTaskGetHandle("defaultTask");
        n++;
        break;
        
        case 1:
        TaskHandle=xTaskGetHandle("KeyTask_N");
        n++;
        break;
        
        case 2:
        TaskHandle=xTaskGetHandle("NetBridgeModbus_N");
        n++;
        break;
        
        case 3:
        TaskHandle=xTaskGetHandle("W5500SendTask_N");
        n++;
        break;
        
        case 4:
        TaskHandle=xTaskGetHandle("PowerDownDoTask_N");
        n++;
        break;
        
        case 5:
        TaskHandle=xTaskGetHandle("eMBPollTask_N");
        n++;
        break;
        
        case 6:
        TaskHandle=xTaskGetHandle("W5500SendTask_N");
        n++;
        break;
        
        case 7:
        TaskHandle=xTaskGetHandle("WifiSendTask_N");
        n++;
        break;
        
        case 8:
        TaskHandle=xTaskGetHandle("W5500Task_N");
        n++;
        break;
        
        case 9:
        TaskHandle=xTaskGetHandle("WifiTask_N");
        n++;
        break;
        
        case 10:
        TaskHandle=xTaskGetHandle("DiabcTask_N");
        n++;
        break;
        
        case 11:
        TaskHandle=xTaskGetHandle("KeyTimerTask_N");
        n++;
        break;
        
        case 12:
        TaskHandle=xTaskGetHandle("IDLE");  //��������� �������� ���ַ���:IDLE
        n=0;
        break;
        
        default:
        n=0;
        break;          
    }
    
    ////��ȡ������Ϣ 
    ////��ʽ�����ֱ�Ϊ:
    ////������ 
    ////ִ�б�������״̬�Ľṹ�� 
    ////ΪpdTRUEʱ,����ջ��ʷ��Сʣ��,��λ��
    ////������������eInvalid,�ѻ�ȡ����ǰ����״̬������TaskStatus.eCurrentState��ȡ
    //vTaskGetInfo(TaskHandle,&TaskStatus,pdTRUE,eInvalid);

    //App_Printf("��������:%s\r\n",TaskStatus.pcTaskName);
    //App_Printf("������:%d\r\n",TaskStatus.xTaskNumber);
    //App_Printf("����״̬:%d\r\n",TaskStatus.eCurrentState);
    //App_Printf("����ǰ���ȼ�:%d\r\n",TaskStatus.uxCurrentPriority);
    //App_Printf("����������ȼ�:%d\r\n",TaskStatus.uxBasePriority);
    ////��x�࣬�����ʱ��ǰ׺0x����0X ��Сдȡ����printf���x�Ĵ�Сд 
    //App_Printf("�����ջ����ַ:%#x\r\n",TaskStatus.pxStackBase); 
    //App_Printf("�����ջ��ʱʣ����Сֵ:%d\r\n",TaskStatus.usStackHighWaterMark);   

}
