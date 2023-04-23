
#ifndef __W5500TASK_H__
#define __W5500TASK_H__

#include "myinclude.h"

////////////////////////////////////////////////////////////////////////
//任务句柄
extern osThreadId_t W5500Task_NHandle;
////////////////////////////////////////////////////////////////////////
//任务属性
extern const osThreadAttr_t W5500Task_N_attributes;

extern void W5500Task(void *argument);

#endif

