


#ifndef __WIFISENDTASK_H__
#define __WIFISENDTASK_H__

#include "myinclude.h"


extern osThreadId_t WifiSendTask_NHandle;

extern const osThreadAttr_t WifiSendTask_N_attributes;

void WifiSendTask(void *argument);

#endif


