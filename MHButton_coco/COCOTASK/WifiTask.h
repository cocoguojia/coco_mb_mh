
#ifndef __WIFITASK_H__
#define __WIFITASK_H__

#include "myinclude.h"


extern osThreadId_t WifiTask_NHandle;

extern const osThreadAttr_t WifiTask_N_attributes;
extern uint8_t g_atk_mw8266d_getApStaIp(void);

void WifiTask(void *argument);

#endif

