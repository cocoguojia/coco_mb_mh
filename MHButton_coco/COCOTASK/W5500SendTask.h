
#ifndef __W5500SEND_TASK_H__
#define __W5500SEND_TASK_H__

#include "myinclude.h"


extern osThreadId_t W5500SendTask_NHandle;
extern const osThreadAttr_t W5500SendTask_N_attributes;

void W5500SendTask(void *argument);

#endif

