
//###############################################################################################################################################################################
//cocoguojia 还没封装2023.3.1
//功能说明

//初始化并 W5500
//置位g_openFreeModbusFlag全局标志 开启FreeModbus功能  

//这个任务负责接收网口数据并解析 分为用户命令和freemodbus数据命令 
//如果是freemodbus数据命令 则正确填装 前3个字节 分别为 数据长度高低字节 消息产生者标志 网口为1 然后实际数据从第4个字节填装 并且发送消息队列给 NetBridgeModbusTask 桥接任务
//如果是用户命令 则解析处理
//
//任务大循环里 放入 W5500循环检测函数 TcpServerDoWith
//###############################################################################################################################################################################

////////////////////////////////////////////////////////////////////////
//本地.h文件
#include "W5500Task.h"

////////////////////////////////////////////////////////////////////////
//任务句柄
osThreadId_t W5500Task_NHandle;
////////////////////////////////////////////////////////////////////////
//任务属性
const osThreadAttr_t W5500Task_N_attributes = {
  .name = "W5500Task_N",
  .stack_size = W5500TASK_stack_size,
  .priority = (osPriority_t) W5500TASK_Priority,
};

////////////////////////////////////////////////////////////////////////
//外部.h文件
#include "w5500.h"
#include "W5500_conf.h"
#include "socket.h"
#include "utility.h"
#include "tcp_dowithSub.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//任务
void W5500Task(void *argument)
{
    //-------------------------------------------------------------------------------------------------------
    reset_w5500();											//硬复位W5500
	set_w5500_mac();										//配置MAC地址
	set_w5500_ip();											//配置IP地址
	
	socket_buf_init(txsize, rxsize);		                //初始化8个Socket的发送接收缓存大小
	
    App_Printf("$$PC=TCP Server W5500=TCP Server\r\n");
// 	App_Printf("$$Server IP:%d.%d.%d.%d\r\n",g_w5500_remote_ip[0],g_w5500_remote_ip[1],g_w5500_remote_ip[2],g_w5500_remote_ip[3]);
//	App_Printf("$$Listen Port:%d\r\n",g_w5500_remote_port);
    osDelay(100);   
    g_openFreeModbusFlag=1;//1=开启FreeModbus功能  
    
  for(;;)
  {
        TcpServerDoWith();
        osDelay(100);
  }
}



