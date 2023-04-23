
#ifndef __KEYTIMERTASK_H__
#define __KEYTIMERTASK_H__

#include "myinclude.h"


extern osThreadId_t KeyTimerTask_NHandle;
extern const osThreadAttr_t KeyTimerTask_N_attributes; 
void KeyTimerTask(void *argument);

#endif

