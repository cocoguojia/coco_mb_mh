
#ifndef __POWERDOWNDOTASK_H__
#define __POWERDOWNDOTASK_H__

#include "myinclude.h"


extern osThreadId_t PowerDownDoTask_NHandle;
extern const osThreadAttr_t PowerDownDoTask_N_attributes;

void PowerDownDoTask(void *argument);





#endif

