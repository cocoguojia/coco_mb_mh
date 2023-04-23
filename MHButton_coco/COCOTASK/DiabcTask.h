
#ifndef __DIABCTASK_H__
#define __DIABCTASK_H__

#include "myinclude.h"


extern osThreadId_t DiabcTask_NHandle;
extern const osThreadAttr_t DiabcTask_N_attributes;

void DiabcTask(void *argument);

#endif

