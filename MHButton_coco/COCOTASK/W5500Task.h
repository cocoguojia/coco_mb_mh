
#ifndef __W5500TASK_H__
#define __W5500TASK_H__

#include "myinclude.h"

////////////////////////////////////////////////////////////////////////
//������
extern osThreadId_t W5500Task_NHandle;
////////////////////////////////////////////////////////////////////////
//��������
extern const osThreadAttr_t W5500Task_N_attributes;

extern void W5500Task(void *argument);

#endif

