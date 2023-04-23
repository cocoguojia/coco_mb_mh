
#ifndef __NETBRIDGEMODBUSTASK_H__
#define __NETBRIDGEMODBUSTASK_H__

#include "myinclude.h"




////////////////////////////////////////////////////////////////////////
//ÈÎÎñ¾ä±ú
extern osThreadId_t NetBridgeModbus_NHandle;
extern const osThreadAttr_t NetBridgeModbus_N_attributes;

extern void NetBridgeModbus(void *argument);

extern void g_showCmdH(uint8_t i,uint8_t g_tcpOrwifiFlag);


#endif

