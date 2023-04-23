
#ifndef _COCO_STARTTASK_H__
#define _COCO_STARTTASK_H__

#include "myinclude.h"


extern void COCO_MX_FREERTOS_Init(void);

////////////////////////////////////////////////////////////////////////////////////////
//优先级规划
#define MONITORINGTASK_Priority                  osPriorityBelowNormal3
#define POWERDOWNDTASK_Priority                  osPriorityBelowNormal2


#define EMBPOLLTASK_Priority                     osPriorityBelowNormal1
#define KEYTASK_Priority                         osPriorityBelowNormal
#define DIABCTASK_Priority                       osPriorityLow7
#define EKYTIMERTASK_Priority                    osPriorityLow6

#define WIFISENDTASK_Priority                    osPriorityLow5
#define W5500SENDTASK_Priority                   osPriorityLow4

#define NETBRIDGEMODBUSTASK_Priority             osPriorityLow3
#define WIFITASK_Priority                        osPriorityLow2
#define W5500TASK_Priority                       osPriorityLow1

//mxcube自动生成的StartDefaultTask任务 优先级     osPriorityLow0

////////////////////////////////////////////////////////////////////////////////////////
//动态栈分配
#define MONITORINGTASK_stack_size               (256*4)
#define POWERDOWNDOTASK_stack_size              (128*4)
#define EMBPOLLTASK_stack_size                  (256*4)
#define WIFISENDTASK_stack_size                 (128*4+70*4)
#define W5500SENDTASK_stack_size                (128*4+128*4+40*4)

#define KEYTASK_stack_size                      (128*4)
#define DIABCTASK_stack_size                    (128*4)
#define EKYTIMERTASK_stack_size                 (128*4)

#define NETBRIDGEMODBUSTASK_stack_size          (256*4+40*4)
#define WIFITASK_stack_size                     (256*4)
#define W5500TASK_stack_size                    (256*4)
//mxcube自动生成的StartDefaultTask stack_size    (256*4)

#endif



