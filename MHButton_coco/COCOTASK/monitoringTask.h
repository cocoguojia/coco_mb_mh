
#ifndef __MONITORINGTASK_H__
#define __MONITORINGTASK_H__

#include "myinclude.h"

extern osThreadId_t monitoringTask_NHandle;
extern const osThreadAttr_t monitoringTask_N_attributes;

void monitoringTask(void *argument);


#endif

