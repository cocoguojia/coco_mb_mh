




////////////////////////////////////////////////////////////////////////
//本地.h文件
#include "W5500SendTask.h"

#include "socket.h"

////////////////////////////////////////////////////////////////////////
//任务句柄
osThreadId_t W5500SendTask_NHandle;
////////////////////////////////////////////////////////////////////////
//任务属性
const osThreadAttr_t W5500SendTask_N_attributes = {
  .name = "W5500SendTask_N",
  .stack_size = W5500SENDTASK_stack_size,
  .priority = (osPriority_t) W5500SENDTASK_Priority,
};

//msg 特殊发送定义
//0xA1 发送 wdt h 查询结果

//0xB1 发送 w5500 h 查询结果
//0xB2 发送 w5500 info 查询结果


//0xC1 发送 wifi h 查询结果
//0xC2 发送 wifi info 查询结果

//0xD1 发送 clk h 查询结果
//0xD2 发送 clk view 查询结果



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//任务
void W5500SendTask(void *argument)
{
    osStatus_t status;
 
    uint16_t msg;
    uint16_t lenNum;

    
    for(;;)
    {
        //----------------------------------------------------------------------------------------
        //w5500要发送数据的 消息队列 等待
        status = osMessageQueueGet(w5500SendQueueHandle, &msg, NULL,osWaitForever);

        //----------------------------------------------------------------------------------------
        //获取w5500要发送数据的消息队列 则
        if (status == osOK)
        {   
            //这里用了乒乓缓存  msg的值 在接收消息队列之前g_w5500SendPingPongFLag已经与msg同步 指示具体哪个乒乓 这里是3个乒乓球
            // msg在任务一开始定义 天然具有静态熟悉         
            if(TCP_WIFI_PINGPONG_N>msg)                                                           
            {  
                //--------------------------------------------------------------------------------------------------------------------------------------------------
                //为了传递指针之外 还可以传递 消息长度 和消息产生者 我们在这里定义 见下文
                lenNum=g_w5500Sendbuff[msg][0];                                               //第1字节为长度的高字节
                lenNum<<=8;
                lenNum+=g_w5500Sendbuff[msg][1];                                              //第2字节为长度的低字节
                
                //从第3个字节开始为 真正要发送的数据
                if(TCP_WIFI_SENDLEN>=lenNum)
                {   
                    send(SOCK_TCPS,(&g_w5500Sendbuff[msg][2]), lenNum);                       //向Server发送数据                   
                }
                else
                {
                    ;
                }   
            }
            else if((0XA1==msg)||(0XA2==msg)||(0XB1==msg)||(0XB2==msg)||(0XC1==msg)||(0XC2==msg)||(0XD1==msg)||(0XD2==msg))
            {
                g_showCmdH(msg,1);
            }
            else
            {
                ;
            }            
         }      
    }

}





