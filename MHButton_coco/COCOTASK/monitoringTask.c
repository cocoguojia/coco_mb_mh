
//############################################################################################################################################
//cocoguojia 封装2023.4.11
//功能说明
//
//############################################################################################################################################

////////////////////////////////////////////////////////////////////////
//本地.h文件
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
//任务句柄
osThreadId_t monitoringTask_NHandle;
//////////////////////////////////////////////////////////////////////////////////////////
//任务属性
const osThreadAttr_t monitoringTask_N_attributes = {
  .name = "monitoringTask_N",
  .stack_size = MONITORINGTASK_stack_size,
  .priority = (osPriority_t) MONITORINGTASK_Priority,
};

uint32_t theTaskNum=0;
TaskStatus_t * StatusArray;


void ShowTaskState(void);

   

//////////////////////////////////////////////////////////////////////////////////////////
//任务
void monitoringTask(void *argument)
{  

    osDelay(10000);
    theTaskNum=osThreadGetCount();//获取任务数量
     //为所有任务都申请可以保存 结构体TaskStatus_t的内存空间
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

    if(NULL!=StatusArray)//如果申请成功 句柄不为空
    {
        //注意TotalRunTime值会一直为0 要保存运行总时间 得自己写相关定时器的代码 具体请百度相关资源
        theTaskNum=uxTaskGetSystemState(StatusArray,theTaskNum,&TotalRunTime);
        
        //用互斥信号量来保证printf也就是串口3的硬件资源的透明性(同一时间硬件只给一个任务占用)        

        App_Printf("TaskName\tPriority\tTaskNumber\tMinimumFreeStack(4Bity)\t\r\n");

        
        for(x=0;x<theTaskNum;x++)
        {
            //xTaskNumber是任务编号 从1开始计数 按照创建任务的顺序自动递增
            //因为我们是在 vTaskStartScheduler()之前创建了3个任务 所以空闲任务的编号为4            
            //usStackHighWaterMark的单位是字  

            App_Printf("G:%s\t\t%d\t\t%d\t\t%d\r\n",
                    StatusArray[x].pcTaskName,
                    (int)StatusArray[x].uxCurrentPriority,
                    (int)StatusArray[x].xTaskNumber,
                    (int)StatusArray[x].usStackHighWaterMark);
        }
    }
    
    //轮流显示4个任务的信息
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
        TaskHandle=xTaskGetHandle("IDLE");  //空闲任务的 任务名称 是字符串:IDLE
        n=0;
        break;
        
        default:
        n=0;
        break;          
    }
    
    ////获取任务信息 
    ////形式参数分别为:
    ////任务句柄 
    ////执行保存任务状态的结构体 
    ////为pdTRUE时,检测堆栈历史最小剩余,单位字
    ////我们在这里用eInvalid,把获取任务当前运行状态交给从TaskStatus.eCurrentState获取
    //vTaskGetInfo(TaskHandle,&TaskStatus,pdTRUE,eInvalid);

    //App_Printf("任务名称:%s\r\n",TaskStatus.pcTaskName);
    //App_Printf("任务编号:%d\r\n",TaskStatus.xTaskNumber);
    //App_Printf("任务状态:%d\r\n",TaskStatus.eCurrentState);
    //App_Printf("任务当前优先级:%d\r\n",TaskStatus.uxCurrentPriority);
    //App_Printf("任务基础优先级:%d\r\n",TaskStatus.uxBasePriority);
    ////对x类，在输出时加前缀0x或者0X 大小写取决于printf里的x的大小写 
    //App_Printf("任务堆栈基地址:%#x\r\n",TaskStatus.pxStackBase); 
    //App_Printf("任务堆栈历时剩余最小值:%d\r\n",TaskStatus.usStackHighWaterMark);   

}
