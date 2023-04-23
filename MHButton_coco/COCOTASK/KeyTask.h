
#ifndef __KEYTASK_H__
#define __KEYTASK_H__

#include "myinclude.h"

extern osThreadId_t KeyTask_NHandle;
extern const osThreadAttr_t KeyTask_N_attributes;

void KeyTask(void *argument);

#endif

