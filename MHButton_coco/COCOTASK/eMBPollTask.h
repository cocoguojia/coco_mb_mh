
#ifndef __EMBPOLLTASK_H__
#define __EMBPOLLTASK_H__

#include "myinclude.h"

extern osThreadId_t eMBPollTask_NHandle;
extern const osThreadAttr_t eMBPollTask_N_attributes;

void eMBPollTask(void *argument);

#endif

