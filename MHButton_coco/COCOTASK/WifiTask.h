
#ifndef __WIFITASK_H__
#define __WIFITASK_H__

#include "myinclude.h"


extern osThreadId_t WifiTask_NHandle;

extern const osThreadAttr_t WifiTask_N_attributes;

void WifiTask(void *argument);

#endif

