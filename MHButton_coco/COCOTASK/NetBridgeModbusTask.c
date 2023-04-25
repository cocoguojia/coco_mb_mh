
//######################################################################################################################################################
//cocoguojia 封装2023.3.1
//功能说明
//等待网口和wifi任意一个初始化成功 则往下运行 否则原地等待
//初始化并使能 freemodbus
//
//这个任务负责接收并且解析 网口和wifi通过消息队列 来传递要 freemodbus 进行处理的原始数据 并触发freemodbus状态机----EV_FRAME_RECEIVED
//解析 前3个字节 分别为 数据长度高低字节 消息产生者标志
//将数据填装到      aucTCPBuf[MB_TCP_BUF_SIZE+3]	     modbus接收缓冲区     (初步接收 程序驱动入口缓冲区)
//将数据长度填装到  usTCPBufLen                       modbus接收缓冲区长度 (初步接收 程序驱动入口长度) 
//
//任务大循环里 放入 freemodbus循环检测函数 eMBPoll
//######################################################################################################################################################

////////////////////////////////////////////////////////////////////////
//本地.h文件
#include "NetBridgeModbusTask.h"

////////////////////////////////////////////////////////////////////////
//外部.h文件
#include "mb.h"
#include "BL8025.h"
#include "socket.h"

extern  TIME_T g_justTimerStruct;
extern TIME_T g_wantSetTimerStruct;

////////////////////////////////////////////////////////////////////////
//外部驱动模块全局变量 来自modbus模块porttcp.c文件
extern UCHAR    aucTCPBuf[MB_TCP_BUF_SIZE+3];	   //modbus接收缓冲区     (初步接收 程序驱动入口缓冲区)
extern USHORT   usTCPBufLen;                       //modbus接收缓冲区长度 (初步接收 程序驱动入口长度)    


//------------------------------------------------------------------------------------------------------------------------------------------------------------
//桥接实验数据备份
//读保持寄存器从内地址1之后的13个字节
//00 00 00 00 00 06 00 03 00 01 00 0D---00 00 00 00 00 1D 00 03 1A 00 01 00 00 00 01 00 01 00 01 00 00 00 00 00 00 00 9F 00 34 00 AC 00 96 00 92 
//00 00 00 00 00 06 00 TCP报文头 
//前两个字节 事务元标识符一般每发送一次就++ 防止重复 
//再两个字节 协议标识符 也是防止重复的 对应TCP固定为00 00
//再两个字节之后字节长度
//再两个字节 单元标识符 也是防止重复的
//03 00 01 00 0D
//再一个字节 功能
//再两个字节 是寄存器起始地址 注意要从1开始数 
//再两个字节 是要操作的寄存器数量

//读保持寄存器从内地址1之后的5个字节
//00 00 00 00 00 06 00 03 00 01 00 05------00 00 00 00 00 0D 00 03 0A 00 01 00 00 00 01 00 01 00 01 

//读保持寄存器地址13的寄存器
//00 00 00 00 00 06 00 03 00 0D 00 01--00 00 00 00 00 05 00 03 02 01 91 

//写保持单个寄存器地址12的寄存器为0X12 0X34
//00 00 00 00 00 06 00 06 00 0C 12 34--00 00 00 00 00 06 00 06 00 0C 12 34 


//写保持多个寄存器从地址06到地址08的寄存器为0X56 0X78 0X90 0XA1 0XB2 0XC3
//TCP字头 长度为13  功能码为0x10 起始地址为00 006 写3个寄存器 后面长度为6字节 然后分别是3个寄存器要写的值
//00 00 00 00 00 0D 00 10 00 06 00 03 06 56 78 90 A1 B2 C3--00 00 00 00 00 06 00 10 00 06 00 03 

//----------------------------------------------
//零点实验
//clk set 23 04 12 023 59 30 1

////////////////////////////////////////////////////////////////////////
//任务句柄
osThreadId_t NetBridgeModbus_NHandle;


char *comRet;
uint8_t charTable[12]={0,0,0,0,0,0,0,0,0,0,0,0};

////////////////////////////////////////////////////////////////////////
//任务属性
const osThreadAttr_t NetBridgeModbus_N_attributes = {
  .name = "NetBridgeModbus_N",
  .stack_size = NETBRIDGEMODBUSTASK_stack_size,
  .priority = (osPriority_t) NETBRIDGEMODBUSTASK_Priority,
};

uint8_t g_analysisUserCmdSub(uint16_t lenN,uint8_t msgN,uint8_t TcpOrWifi);
void cmd_wdt(uint8_t n,uint8_t TcpOrWifi);
void cmd_w5500(uint8_t n,uint8_t TcpOrWifi);
void cmd_wifi(uint8_t n,uint8_t TcpOrWifi);
void cmd_clk(uint8_t n,uint8_t TcpOrWifi);
void cmd_wdt_send_Data(char* str,uint16_t len,uint8_t TcpOrWifi);
void sendcmdH(const uint8_t *str,uint8_t g_tcpOrwifiFlag);
uint8_t strDoSubxxx(char *comRet,uint8_t n,uint8_t *WriteToTable,uint8_t maxlen,uint8_t lenstatic,uint8_t numFlag);

uint8_t charDoSubxxx(char *comRet,uint8_t n,uint8_t *WriteToTable);

uint8_t setTimerxxxxxxx(char *comRet,uint8_t n);

#define USER_CMD_X 32
#define USER_CMD_Y 50
const char  USER_CMD_TABLE[USER_CMD_X][USER_CMD_Y]= { \
"wdt h","wdt en 0","wdt en 1", \
"w5500 h","w5500 ip","w5500 sn","w5500 gw","w5500 info","w5500 clear","w5500 en","w5500 no", \
"wifi h","wifi apid","wifi apkey","wifi apip","wifi apsn","wifi apgw","wifi stassid","wifi stakey","wifi staip","wifi stasn","wifi stagw","wifi reip","wifi report","wifi info","wifi en","wifi no", "wifi dhcp en","wifi dhcp no",\
"clk h","clk set","clk view" \
};


////////////////////////////////////////////////////////////////////////
//任务
void NetBridgeModbus(void *argument)
{ 
    osStatus_t status;   
    uint16_t msg;
    uint16_t i;
    uint16_t lenNum;
    uint8_t haveBeenProcessedFlag=0;//1=已经处理完无需给modbus处理
    
    //-------------------------------------------------------------------
    //网口或者wifi 初始化成功则真正启动该任务 否则原地踏步等待
    if(0==g_openFreeModbusFlag)
    {
        osDelay(10);
    }
    osDelay(200);
    


  for(;;)
  {
      //----------------------------------------------------------------------------------------
      //获取网络桥接modbus消息队列
      status = osMessageQueueGet(netBridgeModbusQueueHandle, &msg, NULL, osWaitForever);
      
      //----------------------------------------------------------------------------------------
      //获取网络桥接modbus消息队列 则
      if (status == osOK)
      {   
            //这里用了一个指针数组 8个元素 保存消息队列的指针 填cmsis_os2 消息队列的坑
            //这样配合队列 真正实现了有8个大小 可以传递指针的缓冲队列 
            // msg在任务一开始定义 天然具有静态熟悉         
            if(8>msg)                                                           
            {
                //--------------------------------------------------------------------------------------------------------------------------------------------------
                //为了传递指针之外 还可以传递 消息长度 和消息产生者 我们在这里定义 见下文
                lenNum=*g_netBridgeModbusQueueTable[msg];                                               //第1字节为长度的高字节
                lenNum<<=8;
                lenNum+=*(g_netBridgeModbusQueueTable[msg]+1);                                          //第2字节为长度的低字节
                g_tcpOrwifiFlag=*(g_netBridgeModbusQueueTable[msg]+2);                                  //第3个字节为 数据来源标志 1=网口 2=wiif 其他=忽略
                
                //---------------------------------------------------------------------------------------------------------------------------------------------------
                //很重要 把接收缓存 有效数据后面添加0 有利于以后的字符串处理 否则 之前的命令会有残留
                if(MB_TCP_BUF_SIZE+3!=lenNum)
                {
                    *(g_netBridgeModbusQueueTable[msg]+3+lenNum)=0;
                }
                
                haveBeenProcessedFlag=g_analysisUserCmdSub(lenNum,msg,g_tcpOrwifiFlag);
                App_Printf("$$haveBeenProcessedFlag=%d\r\n",haveBeenProcessedFlag); 
                
                //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
                if(0==haveBeenProcessedFlag)
                {
                    if((0<lenNum)&&(MB_TCP_BUF_SIZE>=lenNum)&&(1<=g_tcpOrwifiFlag)&&(2>=g_tcpOrwifiFlag))   //再次限定 以防万一
                    {
                        for(i=0;i<lenNum;i++)
                        {
                            aucTCPBuf[i]=*(g_netBridgeModbusQueueTable[msg]+3+i);   //填装数据 真正的数据是从第4个字节开始的 modbus接收缓冲区(初步接收 程序驱动入口缓冲区)
                        }
                        usTCPBufLen=lenNum;                                         //modbus接收缓冲区长度 (初步接收 程序驱动入口长度) 
                        xMBPortEventPost( EV_FRAME_RECEIVED );                      //发送已接收到新数据事件 触发Modbus-TCP状态机 告诉modbus 有数据接收到 让其进行处理  来自modbus模块portevent.c文件
                    }  
                } 
                else
                {
                    haveBeenProcessedFlag=0; 
                }                     
            }        
      }
  }
}


//------------------------------------------------------------------------------------------------------------------
//解析 用户私有命令函数
//TcpOrWifi 1=网口 2=wiif 其他=忽略
//已经处理完无需modbus处理 则返回1
//否则返回0
uint8_t g_analysisUserCmdSub(uint16_t lenN,uint8_t msgN,uint8_t TcpOrWifi)
{

    uint8_t *cmdPoint;
    uint8_t n;
    cmdPoint=(g_netBridgeModbusQueueTable[msgN]+3);
    //-------------------------------------------------------------------------------------------------------------------------
    if(1==TcpOrWifi)                                                                        //消息的产生者标志为全局变量
    {
        App_Printf("$$W5500 GetQueue->netBridgeModbus len=%d msg=%d\r\n",lenN,msgN);        //消息来自于 网口 W5500
    }
    else if(2==TcpOrWifi)
    {
        App_Printf("$$WIFI GetQueue->netBridgeModbus len=%d msg=%d\r\n",lenN,msgN);         //消息来自于 WIFI ESP8266
    }
    else
    {
        return 1;
    }
        
    if (cmdPoint != NULL)
    {
        for(n=0;n<USER_CMD_X;n++)
        {
            comRet=strstr((const char *)cmdPoint, &(USER_CMD_TABLE[n][0])); 
            if(NULL!=comRet)
            {
                 App_Printf("$$the cmd N=%d \r\n",n); 
                 switch(n)
                 {
                     case 0:
                     cmd_wdt(0,TcpOrWifi);
                     break;
                     case 1:
                     cmd_wdt(1,TcpOrWifi);
                     break;
                     case 2:
                     cmd_wdt(2,TcpOrWifi);
                     break;
                  
                     case 3:
                     cmd_w5500(0,TcpOrWifi);
                     break;
                     case 4:
                     cmd_w5500(1,TcpOrWifi);
                     break;
                     case 5:
                     cmd_w5500(2,TcpOrWifi);
                     break;
                     case 6:
                     cmd_w5500(3,TcpOrWifi);
                     break;
                     case 7:
                     cmd_w5500(4,TcpOrWifi);
                     break;
                     case 8:
                     cmd_w5500(5,TcpOrWifi);
                     break;
                     case 9:
                     cmd_w5500(6,TcpOrWifi);
                     break;
                     case 10:
                     cmd_w5500(7,TcpOrWifi);
                     break;
                  
                                      
                     case 11:
                     cmd_wifi(0,TcpOrWifi);                 
                     break;
                     case 12:
                     cmd_wifi(1,TcpOrWifi);
                     break;
                     case 13:
                     cmd_wifi(2,TcpOrWifi);
                     break;
                     case 14:
                     cmd_wifi(3,TcpOrWifi);
                     break;
                     case 15:
                     cmd_wifi(4,TcpOrWifi);
                     break;
                     case 16:
                     cmd_wifi(5,TcpOrWifi);
                     break;
                     case 17:
                     cmd_wifi(6,TcpOrWifi);
                     break;
                     case 18:
                     cmd_wifi(7,TcpOrWifi);
                     break;
                     case 19:
                     cmd_wifi(8,TcpOrWifi);
                     break;
                     case 20:
                     cmd_wifi(9,TcpOrWifi);
                     break;
                     case 21:
                     cmd_wifi(10,TcpOrWifi);
                     break;
                     case 22:
                     cmd_wifi(11,TcpOrWifi);
                     break;
                     case 23:
                     cmd_wifi(12,TcpOrWifi);
                     break;
                     case 24:
                     cmd_wifi(13,TcpOrWifi);
                     break;
                     case 25:
                     cmd_wifi(14,TcpOrWifi);
                     break;
                     case 26:
                     cmd_wifi(15,TcpOrWifi);
                     break;
                     case 27:
                     cmd_wifi(16,TcpOrWifi);
                     break;
                     case 28:
                     cmd_wifi(17,TcpOrWifi);
                     break;
                       
                     
                     
                     case 29:
                     cmd_clk(0,TcpOrWifi);
                     break;
                     case 30:
                     cmd_clk(1,TcpOrWifi);
                     break;                 
                     case 31:
                     cmd_clk(2,TcpOrWifi);
                     break;
                     
                     default:
                     break;
                 }
                 return 1;
            }
            
        } 
    }
    else
    {
         return 1;
    }    
    
    return 0;    
}




//msg 特殊发送定义
//0xA1 发送 wdt h 查询结果

//0xB1 发送 w5500 h 查询结果
//0xB2 发送 w5500 info 查询结果


//0xC1 发送 wifi h 查询结果
//0xC2 发送 wifi info 查询结果

//0xD1 发送 clk h 查询结果
//0xD2 发送 clk view 查询结果




void cmd_wdt(uint8_t n,uint8_t TcpOrWifi)
{
    uint16_t w5500Msg;
    switch(n)
    {
        case 0:             //查看命令 wdt h
        w5500Msg=0xA1;
        App_Printf("$$cmd=wdt h\r\n");
        if(1==TcpOrWifi)
        {
             osMessageQueuePut(w5500SendQueueHandle,&w5500Msg, 0U, 100U);
        }
        else
        {
             osMessageQueuePut(wifiSendQueueHandle,&w5500Msg, 0U, 100U);
        }
        break;
        
        case 1:             //查看命令 wdt en 0  关狗 其实就是要求复位
        App_Printf("$$cmd=wdt en 0\r\n");
        osDelay(100);
        //---------------------------------------------------------------
        //复位
        taskENTER_CRITICAL();
        HAL_NVIC_SystemReset();
        taskEXIT_CRITICAL();
        osDelay(500);
        break;
        
        case 2:             //查看命令 wdt en 1
        App_Printf("$$cmd=wdt en 1\r\n");
        ;//暂时无意义
        break;
        
        default:
        break;
        
    }
}

void cmd_w5500(uint8_t n,uint8_t TcpOrWifi)
{
    uint16_t w5500Msg;
    uint16_t wifiMsg;
    
    uint8_t i;
    char table[80];
    uint8_t save_Buffer[20];
    switch(n)
    {
        case 0:             //查看命令 w5500 h
        w5500Msg=0xB1;
        wifiMsg=0XB1;
        App_Printf("$$cmd=w5500 h\r\n");
        if(1==TcpOrWifi)
        {
             osMessageQueuePut(w5500SendQueueHandle,&w5500Msg, 0U, 100U);
        }
        else
        {
             osMessageQueuePut(wifiSendQueueHandle,&wifiMsg, 0U, 100U);
        }
        break;
        
        case 1:             //设置W5500默认IP地址
            
        //设置ip
        if(0==charDoSubxxx(comRet,3,&g_w5500_local_ip[0]))
        {
            App_Printf("set w5500 ip=%d.%d.%d.%d\r\n",g_w5500_local_ip[0],g_w5500_local_ip[1],g_w5500_local_ip[2],g_w5500_local_ip[3]);
            
            sprintf(table,"set w5500 ip=%d.%d.%d.%d",g_w5500_local_ip[0],g_w5500_local_ip[1],g_w5500_local_ip[2],g_w5500_local_ip[3]);
            cmd_wdt_send_Data(table,0,TcpOrWifi);
            for(i=0;i<4;i++)
            {
                save_Buffer[i]=g_w5500_local_ip[i];
            }
            //##############################################################################################
            //互斥操作
            osMutexAcquire(W25q64MutexHandle, osWaitForever);   //一直等待获取互斥资源 
            W25Q128_Erase_Sector(WQ_PAGE_W5500_IP);  
            W25Q128_Write(&save_Buffer[0],WQ_PAGE_W5500_IP,4); 
            osMutexRelease(W25q64MutexHandle);                  //释放互斥资源    
            //##############################################################################################                
        }            
        break;
        
        case 2:           //设置W5500默认sn地址
        //设置sn
        if(0==charDoSubxxx(comRet,3,&g_w5500_subnet[0]))
        {
            App_Printf("set w5500 sn=%d.%d.%d.%d\r\n",g_w5500_subnet[0],g_w5500_subnet[1],g_w5500_subnet[2],g_w5500_subnet[3]);
            
            sprintf(table,"set w5500 sn=%d.%d.%d.%d",g_w5500_subnet[0],g_w5500_subnet[1],g_w5500_subnet[2],g_w5500_subnet[3]);
            cmd_wdt_send_Data(table,0,TcpOrWifi); 
            for(i=0;i<4;i++)
            {
                save_Buffer[i]=g_w5500_subnet[i];
            }
            //##############################################################################################
            //互斥操作
            osMutexAcquire(W25q64MutexHandle, osWaitForever);   //一直等待获取互斥资源 
            W25Q128_Erase_Sector(WQ_PAGE_W5500_SN);  
            W25Q128_Write(&save_Buffer[0],WQ_PAGE_W5500_SN,4); 
            osMutexRelease(W25q64MutexHandle);                  //释放互斥资源    
            //##############################################################################################                         
        }        
     
        break;
        
        case 3:           //设置W5500默认gw地址
        //设置gw
        if(0==charDoSubxxx(comRet,3,&g_w5500_gateway[0]))
        {
            App_Printf("set w5500 gw=%d.%d.%d.%d\r\n",g_w5500_gateway[0],g_w5500_gateway[1],g_w5500_gateway[2],g_w5500_gateway[3]);
            
            sprintf(table,"set w5500 gw=%d.%d.%d.%d",g_w5500_gateway[0],g_w5500_gateway[1],g_w5500_gateway[2],g_w5500_gateway[3]);
            cmd_wdt_send_Data(table,0,TcpOrWifi);
            for(i=0;i<4;i++)
            {
                save_Buffer[i]=g_w5500_gateway[i];
            }
            //##############################################################################################
            //互斥操作
            osMutexAcquire(W25q64MutexHandle, osWaitForever);   //一直等待获取互斥资源 
            W25Q128_Erase_Sector(WQ_PAGE_W5500_GW);  
            W25Q128_Write(&save_Buffer[0],WQ_PAGE_W5500_GW,4); 
            osMutexRelease(W25q64MutexHandle);                  //释放互斥资源    
            //##############################################################################################                        
        }   
        break;
        
        case 4:             //查看命令 info
        w5500Msg=0xB2;
        wifiMsg=0XB2;
        App_Printf("$$cmd=w5500 info\r\n");
        if(1==TcpOrWifi)
        {
             osMessageQueuePut(w5500SendQueueHandle,&w5500Msg, 0U, 100U);
        }
        else
        {
             osMessageQueuePut(wifiSendQueueHandle,&wifiMsg, 0U, 100U);
        }
        break;
        
        case 5:           //clear
        cmd_wdt_send_Data("w5500 clear all",0,TcpOrWifi);    
        break;
        
        case 6:           //en
        //使能w5500       
        cmd_wdt_send_Data("w5500 en",0,TcpOrWifi);
        save_Buffer[0]=1;
        //##############################################################################################
        //互斥操作
        osMutexAcquire(W25q64MutexHandle, osWaitForever);   //一直等待获取互斥资源 
        W25Q128_Erase_Sector(WQ_PAGE_W5500_ENNO);  
        W25Q128_Write(&save_Buffer[0],WQ_PAGE_W5500_ENNO,1); 
        osMutexRelease(W25q64MutexHandle);                  //释放互斥资源    
        //##############################################################################################    
        osDelay(100); 
        //---------------------------------------------------------------
        //复位
        taskENTER_CRITICAL();
        HAL_NVIC_SystemReset();
        taskEXIT_CRITICAL();
        osDelay(500);        
        break;
           
        case 7:           //no
        //禁止w5500
        cmd_wdt_send_Data("w5500 no",0,TcpOrWifi);
        save_Buffer[0]=0;
        //##############################################################################################
        //互斥操作
        osMutexAcquire(W25q64MutexHandle, osWaitForever);   //一直等待获取互斥资源 
        W25Q128_Erase_Sector(WQ_PAGE_W5500_GW);  
        W25Q128_Write(&save_Buffer[0],WQ_PAGE_W5500_ENNO,1); 
        osMutexRelease(W25q64MutexHandle);                  //释放互斥资源    
        //##############################################################################################    
        osDelay(100); 
        //---------------------------------------------------------------
        //复位
        taskENTER_CRITICAL();
        HAL_NVIC_SystemReset();
        taskEXIT_CRITICAL();
        osDelay(500);        
        break;
         
        
        default:
        break;
        
    }
}


void cmd_wifi(uint8_t n,uint8_t TcpOrWifi)
{
    uint16_t wifiMsg;
    uint16_t w5500Msg;
    char table[90];
    uint8_t i=0;
     uint8_t len=0;
    uint8_t save_Buffer[30];//22
    switch(n)
    {
        case 0:             //查看命令 w5500 h     
        w5500Msg=0XC1;
        wifiMsg=0xC1;
        App_Printf("$$cmd=wifi h\r\n");
        if(1==TcpOrWifi)
        {
             osMessageQueuePut(w5500SendQueueHandle,&w5500Msg, 0U, 100U);
        }
        else
        {
             osMessageQueuePut(wifiSendQueueHandle,&wifiMsg, 0U, 100U);
        }
        break;
        
        case 1: // ap ssid
        if(0==strDoSubxxx(comRet,3,(uint8_t*)g_wifi_ap_sidd_temp,8,0,0))
        {
            App_Printf("set wifi ap id=%s\r\n",&g_wifi_ap_sidd_temp[0]);
            
            len=strlen(&g_wifi_ap_sidd_temp[0]);
            if(8>=len)
            {
                for(i=0;i<len;i++)
                {
                    save_Buffer[i]=g_wifi_ap_sidd_temp[i];
                }
            }
            save_Buffer[len]=0;
            
            sprintf(&table[0],"set wifi ap id=%s\r\n",(const char*)&g_wifi_ap_sidd_temp[0]);
         
            cmd_wdt_send_Data(&table[0],0,TcpOrWifi);
            

            
            App_Printf("wifi ap id len=%d,%s\r\n",len,save_Buffer);
            //##############################################################################################
            //互斥操作
            osMutexAcquire(W25q64MutexHandle, osWaitForever);   //一直等待获取互斥资源 
            W25Q128_Erase_Sector(WQ_PAGE_WIFI_APID);  
            W25Q128_Write(&save_Buffer[0],WQ_PAGE_WIFI_APID,len+1); 
            osMutexRelease(W25q64MutexHandle);                  //释放互斥资源    
            //##############################################################################################      
         }            
  
        break;
        
        case 2:  // ap key
        if(0==strDoSubxxx(comRet,3,(uint8_t*)g_wifi_ap_password_temp,8,1,0))
        {
            App_Printf("set wifi ap key=%s\r\n",&g_wifi_ap_password_temp);
            len=8;
           
            for(i=0;i<len;i++)
            {
                save_Buffer[i]=g_wifi_ap_password_temp[i];
            }
            
            save_Buffer[9]=0;

            sprintf(table,"set wifi ap key=%s",(const char*)&g_wifi_ap_password_temp);
            cmd_wdt_send_Data(table,0,TcpOrWifi);
           
            App_Printf("wifi ap key len=%d,%s\r\n",len,save_Buffer);
            //##############################################################################################
            //互斥操作
            osMutexAcquire(W25q64MutexHandle, osWaitForever);   //一直等待获取互斥资源 
            W25Q128_Erase_Sector(WQ_PAGE_WIFI_APKEY);  
            W25Q128_Write(&save_Buffer[0],WQ_PAGE_WIFI_APKEY,len); 
            osMutexRelease(W25q64MutexHandle);                  //释放互斥资源    
            //##############################################################################################      
         }               
  
        break;
        
        case 3://设置wifi apip
        if(0==charDoSubxxx(comRet,3,(uint8_t*)&g_wifi_ap_ip_temp[0]))
        {
            App_Printf("set wifi apip=%d.%d.%d.%d\r\n",g_wifi_ap_ip_temp[0],g_wifi_ap_ip_temp[1],g_wifi_ap_ip_temp[2],g_wifi_ap_ip_temp[3]);

            sprintf(table,"set wifi apip=%d.%d.%d.%d",g_wifi_ap_ip_temp[0],g_wifi_ap_ip_temp[1],g_wifi_ap_ip_temp[2],g_wifi_ap_ip_temp[3]);
            cmd_wdt_send_Data(table,0,TcpOrWifi);
            for(i=0;i<4;i++)
            {
                save_Buffer[i]=g_wifi_ap_ip_temp[i];
            }
            //##############################################################################################
            //互斥操作
            osMutexAcquire(W25q64MutexHandle, osWaitForever);   //一直等待获取互斥资源 
            W25Q128_Erase_Sector(WQ_PAGE_WIFI_AP_IP);  
            W25Q128_Write(&save_Buffer[0],WQ_PAGE_WIFI_AP_IP,4); 
            osMutexRelease(W25q64MutexHandle);                  //释放互斥资源    
            //############################################################################################## 
        }            
        break;
             
        
        case 4://设置wifi apsn
        if(0==charDoSubxxx(comRet,3,(uint8_t*)&g_wifi_ap_sn_temp[0]))
        {
            App_Printf("set wifi apsn=%d.%d.%d.%d\r\n",g_wifi_ap_sn_temp[0],g_wifi_ap_sn_temp[1],g_wifi_ap_sn_temp[2],g_wifi_ap_sn_temp[3]);

            sprintf(table,"set wifi apsn=%d.%d.%d.%d",g_wifi_ap_sn_temp[0],g_wifi_ap_sn_temp[1],g_wifi_ap_sn_temp[2],g_wifi_ap_sn_temp[3]);
            cmd_wdt_send_Data(table,0,TcpOrWifi);
            for(i=0;i<4;i++)
            {
                save_Buffer[i]=g_wifi_ap_sn_temp[i];
            }
            //##############################################################################################
            //互斥操作
            osMutexAcquire(W25q64MutexHandle, osWaitForever);   //一直等待获取互斥资源 
            W25Q128_Erase_Sector(WQ_PAGE_WIFI_AP_SN);  
            W25Q128_Write(&save_Buffer[0],WQ_PAGE_WIFI_AP_SN,4); 
            osMutexRelease(W25q64MutexHandle);                  //释放互斥资源    
            //##############################################################################################    
        }            
        break;
        
        case 5://设置wifi apgw
        if(0==charDoSubxxx(comRet,3,(uint8_t*)&g_wifi_ap_gw_temp[0]))
        {
            App_Printf("set wifi apgw=%d.%d.%d.%d\r\n",g_wifi_ap_gw_temp[0],g_wifi_ap_gw_temp[1],g_wifi_ap_gw_temp[2],g_wifi_ap_gw_temp[3]);

            sprintf(table,"set wifi apgw=%d.%d.%d.%d",g_wifi_ap_gw_temp[0],g_wifi_ap_gw_temp[1],g_wifi_ap_gw_temp[2],g_wifi_ap_gw_temp[3]);
            cmd_wdt_send_Data(table,0,TcpOrWifi);
            for(i=0;i<4;i++)
            {
                save_Buffer[i]=g_wifi_ap_gw_temp[i];
            }
            //##############################################################################################
            //互斥操作
            osMutexAcquire(W25q64MutexHandle, osWaitForever);   //一直等待获取互斥资源 
            W25Q128_Erase_Sector(WQ_PAGE_WIFI_AP_GW);  
            W25Q128_Write(&save_Buffer[0],WQ_PAGE_WIFI_AP_GW,4); 
            osMutexRelease(W25q64MutexHandle);                  //释放互斥资源    
            //############################################################################################## 
        }            
        break;
        
        case 6:            
        if(0==strDoSubxxx(comRet,3,(uint8_t*)g_wifi_sta_sidd_temp,25,0,0))
        {
            App_Printf("set wifi sta ssid=%s\r\n",&g_wifi_sta_sidd_temp);
            len=strlen(&g_wifi_sta_sidd_temp[0]);
            if(25>=len)
            {
                for(i=0;i<len;i++)
                {
                    save_Buffer[i]=g_wifi_sta_sidd_temp[i];
                }
            }
            save_Buffer[len]=0;

            sprintf(table,"set wifi sta ssid=%s",(const char*)&g_wifi_sta_sidd_temp);
            cmd_wdt_send_Data(table,0,TcpOrWifi);
                
          
            App_Printf("wifi sta ssid len=%d,%s\r\n",len,save_Buffer);
            //##############################################################################################
            //互斥操作
            osMutexAcquire(W25q64MutexHandle, osWaitForever);   //一直等待获取互斥资源 
            W25Q128_Erase_Sector(WQ_PAGE_WIFI_STASSID);  
            W25Q128_Write(&save_Buffer[0],WQ_PAGE_WIFI_STASSID,len); 
            osMutexRelease(W25q64MutexHandle);                  //释放互斥资源    
            //##############################################################################################      
         }            
        break;
        
        case 7:            
        if(0==strDoSubxxx(comRet,3,(uint8_t*)g_wifi_sta_password_temp,12,0,0))
        {
            App_Printf("set wifi sta key=%s\r\n",&g_wifi_sta_password_temp);
            len=strlen(&g_wifi_sta_password_temp[0]);
            if(12>=len)
            {
                for(i=0;i<len;i++)
                {
                    save_Buffer[i]=g_wifi_sta_password_temp[i];
                }
            }
            save_Buffer[len]=0;
            sprintf(table,"set wifi sta key=%s",(const char*)&g_wifi_sta_password_temp);
            cmd_wdt_send_Data(table,0,TcpOrWifi);
          
            App_Printf("wifi sta key len=%d,%s\r\n",len,save_Buffer);
            //##############################################################################################
            //互斥操作
            osMutexAcquire(W25q64MutexHandle, osWaitForever);   //一直等待获取互斥资源 
            W25Q128_Erase_Sector(WQ_PAGE_WIFI_STAKEY);  
            W25Q128_Write(&save_Buffer[0],WQ_PAGE_WIFI_STAKEY,len); 
            osMutexRelease(W25q64MutexHandle);                  //释放互斥资源    
            //##############################################################################################      
         }            
        break;
        
        case 8://设置wifi staip
        if(0==charDoSubxxx(comRet,3,(uint8_t*)&g_wifi_sta_ip_temp[0]))
        {
            App_Printf("set wifi staip=%d.%d.%d.%d\r\n",g_wifi_sta_ip_temp[0],g_wifi_sta_ip_temp[1],g_wifi_sta_ip_temp[2],g_wifi_sta_ip_temp[3]);

            sprintf(table,"set wifi staip=%d.%d.%d.%d",g_wifi_sta_ip_temp[0],g_wifi_sta_ip_temp[1],g_wifi_sta_ip_temp[2],g_wifi_sta_ip_temp[3]);
            cmd_wdt_send_Data(table,0,TcpOrWifi);
            for(i=0;i<4;i++)
            {
                save_Buffer[i]=g_wifi_sta_ip_temp[i];
            }
            //##############################################################################################
            //互斥操作
            osMutexAcquire(W25q64MutexHandle, osWaitForever);   //一直等待获取互斥资源 
            W25Q128_Erase_Sector(WQ_PAGE_WIFI_STA_IP);  
            W25Q128_Write(&save_Buffer[0],WQ_PAGE_WIFI_STA_IP,4); 
            osMutexRelease(W25q64MutexHandle);                  //释放互斥资源    
            //##############################################################################################  
        }            
        break;
             
        
        case 9://设置wifi stasn
        if(0==charDoSubxxx(comRet,3,(uint8_t*)&g_wifi_sta_sn_temp[0]))
        {
            App_Printf("set wifi stasn=%d.%d.%d.%d\r\n",g_wifi_sta_sn_temp[0],g_wifi_sta_sn_temp[1],g_wifi_sta_sn_temp[2],g_wifi_sta_sn_temp[3]);

            sprintf(table,"set wifi stasn=%d.%d.%d.%d",g_wifi_sta_sn_temp[0],g_wifi_sta_sn_temp[1],g_wifi_sta_sn_temp[2],g_wifi_sta_sn_temp[3]);
            cmd_wdt_send_Data(table,0,TcpOrWifi);
            for(i=0;i<4;i++)
            {
                save_Buffer[i]=g_wifi_sta_sn_temp[i];
            }
            //##############################################################################################
            //互斥操作
            osMutexAcquire(W25q64MutexHandle, osWaitForever);   //一直等待获取互斥资源 
            W25Q128_Erase_Sector(WQ_PAGE_WIFI_STA_SN);  
            W25Q128_Write(&save_Buffer[0],WQ_PAGE_WIFI_STA_SN,4); 
            osMutexRelease(W25q64MutexHandle);                  //释放互斥资源    
            //############################################################################################## 
        }            
        break;
        
        case 10://设置wifi stagw
        if(0==charDoSubxxx(comRet,3,(uint8_t*)&g_wifi_sta_gw_temp[0]))
        {
            App_Printf("set wifi stagw=%d.%d.%d.%d\r\n",g_wifi_sta_gw_temp[0],g_wifi_sta_gw_temp[1],g_wifi_sta_gw_temp[2],g_wifi_sta_gw_temp[3]);

            sprintf(table,"set wifi stgw=%d.%d.%d.%d",g_wifi_sta_gw_temp[0],g_wifi_sta_gw_temp[1],g_wifi_sta_gw_temp[2],g_wifi_sta_gw_temp[3]);
            cmd_wdt_send_Data(table,0,TcpOrWifi);
            for(i=0;i<4;i++)
            {
                save_Buffer[i]=g_wifi_sta_gw_temp[i];
            }
            //##############################################################################################
            //互斥操作
            osMutexAcquire(W25q64MutexHandle, osWaitForever);   //一直等待获取互斥资源 
            W25Q128_Erase_Sector(WQ_PAGE_WIFI_STA_GW);  
            W25Q128_Write(&save_Buffer[0],WQ_PAGE_WIFI_STA_GW,4); 
            osMutexRelease(W25q64MutexHandle);                  //释放互斥资源    
            //##############################################################################################    
        }            
        break;
   
        case 11:            
//        if(0==charDoSubxxx(comRet,3,(uint8_t*)&g_wifi_sta_sever_ip_temp[0]))
//        {
//            App_Printf("set wifi reip=%d.%d.%d.%d\r\n",g_wifi_sta_sever_ip_temp[0],g_wifi_sta_sever_ip_temp[1],g_wifi_sta_sever_ip_temp[2],g_wifi_sta_sever_ip_temp[3]);
//            
//            sprintf(table,"set wifi reip=%d.%d.%d.%d",g_wifi_sta_sever_ip_temp[0],g_wifi_sta_sever_ip_temp[1],g_wifi_sta_sever_ip_temp[2],g_wifi_sta_sever_ip_temp[3]);
//            cmd_wdt_send_Data(table,0,TcpOrWifi);
//            for(i=0;i<4;i++)
//            {
//                save_Buffer[i]=g_wifi_sta_sever_ip_temp[i];
//            }
//            //##############################################################################################
//            //互斥操作
//            osMutexAcquire(W25q64MutexHandle, osWaitForever);   //一直等待获取互斥资源 
//            W25Q128_Erase_Sector(WQ_PAGE_WIFI_REIP);  
//            W25Q128_Write(&save_Buffer[0],WQ_PAGE_WIFI_REIP,4); 
//            osMutexRelease(W25q64MutexHandle);                  //释放互斥资源    
//            //##############################################################################################                
//        }            
        break;
        
        case 12:                  
        if(0==strDoSubxxx(comRet,3,(uint8_t*)g_wifi_sta_sever_port_temp,5,0,1))
        {
            App_Printf("set wifi report=%s\r\n",&g_wifi_sta_sever_port_temp);
            len=strlen(&g_wifi_sta_sever_port_temp[0]);
            if(5>=len)
            {
                for(i=0;i<len;i++)
                {
                    save_Buffer[i]=g_wifi_sta_sever_port_temp[i];
                }
            }
            save_Buffer[len]=0;
            sprintf(table,"set wifi report=%s",(const char*)&g_wifi_sta_sever_port_temp);
            cmd_wdt_send_Data(table,0,TcpOrWifi);
                
            len=strlen(table);
                
            if(5>=len)
            {
                for(i=0;i<len;i++)
                {
                    save_Buffer[i]=table[i];
                }
            }
            App_Printf("wifi report len=%d,%s\r\n",len,save_Buffer);
            //##############################################################################################
            //互斥操作
            osMutexAcquire(W25q64MutexHandle, osWaitForever);   //一直等待获取互斥资源 
            W25Q128_Erase_Sector(WQ_PAGE_WIFI_REPORT);  
            W25Q128_Write(&save_Buffer[0],WQ_PAGE_WIFI_REPORT,len); 
            osMutexRelease(W25q64MutexHandle);                  //释放互斥资源    
            //##############################################################################################      
         }
        break;
        
        case 13:            
        w5500Msg=0XC2;
        wifiMsg=0xC2;
        App_Printf("$$ cmd=wifi info\r\n");
        if(1==TcpOrWifi)
        {
             osMessageQueuePut(w5500SendQueueHandle,&w5500Msg, 0U, 100U);
        }
        else
        {
             osMessageQueuePut(wifiSendQueueHandle,&wifiMsg, 0U, 100U);
        }
        break;
        
        case 14://en
        //使能wifi       
        cmd_wdt_send_Data("wifi en",0,TcpOrWifi);
        save_Buffer[0]=1;
        //##############################################################################################
        //互斥操作
        osMutexAcquire(W25q64MutexHandle, osWaitForever);   //一直等待获取互斥资源 
        W25Q128_Erase_Sector(WQ_PAGE_WIFI_ENNO);  
        W25Q128_Write(&save_Buffer[0],WQ_PAGE_WIFI_ENNO,1); 
        osMutexRelease(W25q64MutexHandle);                  //释放互斥资源    
        //##############################################################################################    
        osDelay(100); 
        //---------------------------------------------------------------
        //复位
        taskENTER_CRITICAL();
        HAL_NVIC_SystemReset();
        taskEXIT_CRITICAL();
        osDelay(500);        
        break;
           
        case 15://no
        //禁止wifi
        cmd_wdt_send_Data("wifi no",0,TcpOrWifi);
        save_Buffer[0]=0;
        //##############################################################################################
        //互斥操作
        osMutexAcquire(W25q64MutexHandle, osWaitForever);   //一直等待获取互斥资源 
        W25Q128_Erase_Sector(WQ_PAGE_WIFI_ENNO);  
        W25Q128_Write(&save_Buffer[0],WQ_PAGE_WIFI_ENNO,1); 
        osMutexRelease(W25q64MutexHandle);                  //释放互斥资源    
        //##############################################################################################    
        osDelay(100); 
        //---------------------------------------------------------------
        //复位
        taskENTER_CRITICAL();
        HAL_NVIC_SystemReset();
        taskEXIT_CRITICAL();
        osDelay(500);        
        break;
        
        case 16://dhcp en
        //使能wifi dhcp       
        cmd_wdt_send_Data("wifi dhcp en",0,TcpOrWifi);
        save_Buffer[0]=1;
        //##############################################################################################
        //互斥操作
        osMutexAcquire(W25q64MutexHandle, osWaitForever);   //一直等待获取互斥资源 
        W25Q128_Erase_Sector(WQ_PAGE_WIFI_DHCP_ENNO);  
        W25Q128_Write(&save_Buffer[0],WQ_PAGE_WIFI_DHCP_ENNO,1); 
        osMutexRelease(W25q64MutexHandle);                  //释放互斥资源    
        //##############################################################################################    
        osDelay(100); 
        
//        //---------------------------------------------------------------
//        //复位
//        taskENTER_CRITICAL();
//        HAL_NVIC_SystemReset();
//        taskEXIT_CRITICAL();
//        osDelay(500);        
        break;
        
        case 17://dhcp no
        //禁止wifi dhcp
        cmd_wdt_send_Data("wifi dhcp no",0,TcpOrWifi);
        save_Buffer[0]=0;
        //##############################################################################################
        //互斥操作
        osMutexAcquire(W25q64MutexHandle, osWaitForever);   //一直等待获取互斥资源 
        W25Q128_Erase_Sector(WQ_PAGE_WIFI_DHCP_ENNO);  
        W25Q128_Write(&save_Buffer[0],WQ_PAGE_WIFI_DHCP_ENNO,1); 
        osMutexRelease(W25q64MutexHandle);                  //释放互斥资源    
        //##############################################################################################    
        osDelay(100); 
        
//        //---------------------------------------------------------------
//        //复位
//        taskENTER_CRITICAL();
//        HAL_NVIC_SystemReset();
//        taskEXIT_CRITICAL();
//        osDelay(500);        
        break;
  

        default:
        break;
        
    }
}



void cmd_clk(uint8_t n,uint8_t TcpOrWifi)
{
    uint16_t w5500Msg;
    uint16_t wifiMsg;
    uint8_t weekTemp;
    char table[100];
    switch(n)
    {
        case 0:             //查看命令 w5500 h
        w5500Msg=0xD1;
        wifiMsg=0xD1;
        App_Printf("$$ cmd=clk h\r\n");
        if(1==TcpOrWifi)
        {
             osMessageQueuePut(w5500SendQueueHandle,&w5500Msg, 0U, 100U);
        }
        else
        {
             osMessageQueuePut(wifiSendQueueHandle,&wifiMsg, 0U, 100U);
        }
        break;
        
        case 1:             //设置timer
        //待加 设置 timer
        setTimerxxxxxxx(comRet,3);
        weekTemp=bsp_ReadRtcWeekChange(&g_wantSetTimerStruct);
        sprintf(table,"set timer=%d-%d-%d %d:%d:%d week=%d\r\n",g_wantSetTimerStruct.year,g_wantSetTimerStruct.month,g_wantSetTimerStruct.day,g_wantSetTimerStruct.hour,g_wantSetTimerStruct.minute,g_wantSetTimerStruct.second,weekTemp);
        cmd_wdt_send_Data(table,0,TcpOrWifi); 
        break;
        
        case 2:             //查看命令 view
        w5500Msg=0xD2;
        wifiMsg=0xD2;
        App_Printf("$$ cmd=clk view\r\n");
        if(1==TcpOrWifi)
        {
             osMessageQueuePut(w5500SendQueueHandle,&w5500Msg, 0U, 100U);
        }
        else
        {
             osMessageQueuePut(wifiSendQueueHandle,&wifiMsg, 0U, 100U);
        }
        break;
      
        
        default:
        break;
        
    }
}



//------------------------------------------------------------------------
//len=0 的时候为发送的字符 按照字符串的长度计算
void cmd_wdt_send_Data(char* str,uint16_t len,uint8_t TcpOrWifi)
{
      uint16_t w5500Msg;
      uint16_t wifiMsg;
      uint8_t i;

      if(0==len)
      {
            len=strlen((const char*)str);
      }
      if(1==TcpOrWifi)
      {
            if(TCP_WIFI_PINGPONG_N<=g_w5500SendPingPongFLag)
            {
                g_w5500SendPingPongFLag=0;
            }
            w5500Msg=g_w5500SendPingPongFLag;  
            g_w5500Sendbuff[g_w5500SendPingPongFLag][0]=(uint8_t)(len>>8);
            g_w5500Sendbuff[g_w5500SendPingPongFLag][1]=(uint8_t)(len&0x00ff);
            for(i=0;i<len;i++)
            {
                g_w5500Sendbuff[g_w5500SendPingPongFLag][i+2]=*str;
                str++;
            }              
            osMessageQueuePut(w5500SendQueueHandle,&w5500Msg, 0U, 100U);
      }
      else
      {
            g_wifiSendPingPongFLag++;
            if(TCP_WIFI_PINGPONG_N<=g_wifiSendPingPongFLag)
            {
                g_wifiSendPingPongFLag=0;       
            }
            wifiMsg=g_wifiSendPingPongFLag;  
            g_wifiSendbuff[g_wifiSendPingPongFLag][0]=(uint8_t)(len>>8);
            g_wifiSendbuff[g_wifiSendPingPongFLag][1]=(uint8_t)(len&0x00ff);
            for(i=0;i<len;i++)
            {
                g_wifiSendbuff[g_wifiSendPingPongFLag][i+2]=*str;
                str++;
            }              
            osMessageQueuePut(wifiSendQueueHandle,&wifiMsg, 0U, 100U);
      }
}



uint16_t strtoNum255(char *str);

//------------------------------------------------------------------------------
//解析xxx xxx xxx xxx
//char *comRet:要解析的字符串指针
//uint8_t n : 第n个次切割的字符串开始有意义 n要大于等于1
//返回 0=成功 1=失败
uint8_t charDoSubxxx(char *comRet,uint8_t n,uint8_t *WriteToTable)
{
       char *cmdTemp=comRet;
       char *cmdTemp1;
       uint8_t m=0;
       uint8_t k=0;
       uint8_t i=0;
       uint16_t j=0;
       if(1<=n)
       {
            n--;
       }
       else
       {
          return 1;
       }
      
      //--------------------------------------------------------------------------
      //该函数返回
      //第一次切割后的字符串 切割后 该整体要切割的字符串已经在strtok函数里有了记录    
      App_Printf("$$%s\r\n",strtok((char *)cmdTemp,(const char*)" "));

      while(cmdTemp1=strtok(NULL,(const char*)" .,"))
      { 
         m++;          
         if(n<=m)
         {             
            App_Printf("$$%s\r\n",cmdTemp1);
             
            j=strtoNum255(cmdTemp1);
            if(0xffff==j)
            {
                return 1;
            }
            else
            {
                charTable[k]=j;
            }
            k++;
            if(4==k)
            {
                for(i=0;i<4;i++)
                {
                    WriteToTable[i]=charTable[i];
                }
              return 0;
            }
         }
        
      }  
      return 1;      
}

//--------------------------------------
//返回 0xffff表示失败 其他是数值
uint16_t strtoNum255(char *str)
{
    uint16_t ret=0xffff;
    uint8_t i;
    uint16_t num=0;
    for(i=0;i<3;i++)
    {
        if(('0'<=*str)&&('9'>=*str))
        {
            if(0!=i)
            {
                num*=10;
            }
            num+=(*str)-'0';          
        }
        else if((' '==*str)||('\0'>=*str))
        {
            i=100;           
        }
        
        else
        {
            return  ret;
        }
        str++;
    }
    if(255<num)
    {
        return  ret;
    }
    return  num;   
}

//------------------------------------------------------------------------------
//解析xxx xxx xxx xxx
//char *comRet:要解析的字符串指针
//uint8_t n : 第n个次切割的字符串开始有意义 n要大于等于1   n=3
//返回 0=成功 1=失败
uint8_t setTimerxxxxxxx(char *comRet,uint8_t n)
{
       char *cmdTemp=comRet;
       char *cmdTemp1;
    
       uint8_t m=0;
       uint8_t k=0;
       uint16_t j=0;
       if(1<=n)
       {
            n--;
       }
       else
       {
          return 1;
       }
      
      //--------------------------------------------------------------------------
      //该函数返回
      //第一次切割后的字符串 切割后 该整体要切割的字符串已经在strtok函数里有了记录    
      App_Printf("$$l%s\r\n",strtok((char *)cmdTemp,(const char*)" "));

      while(cmdTemp1=strtok(NULL,(const char*)" ,."))
      { 
         m++;          
         if(n<=m)
         {             
            App_Printf("$$%s\r\n",cmdTemp1);
             
            j=strtoNum255(cmdTemp1);
            switch(k)
            {
                case 0:
                if((0==j)||(99<j))
                {
                    return 1;
                }
                g_wantSetTimerStruct.year=j;
                break;  

                case 1:
                if((0==j)||(12<j))
                {
                    return 1;
                }
                g_wantSetTimerStruct.month=j;
                break;  

                case 2:
                if((0==j)||(31<j))
                {
                    return 1;
                }
                g_wantSetTimerStruct.day=j;
                break; 

                case 3:
                if(24<=j)
                {
                    return 1;
                }
                g_wantSetTimerStruct.hour=j;
                break; 

                case 4:
                if(60<=j)
                {
                    return 1;
                }
                g_wantSetTimerStruct.minute=j;
                break;  
                
                case 5:
                if(60<=j)
                {
                    return 1;
                }
                g_wantSetTimerStruct.second=j;
                break;  
                
                case 6:
                if(7<j)
                {
                    return 1;
                }
                bsp_WriteRtcWeekChange(&g_wantSetTimerStruct,j);
                break; 

                default:
                break;                    
            }                
            if(0xffff==j)
            {
                return 1;
            }
            else
            {
                charTable[k]=j;
            }
            k++;
            if(7==k)
            {
              g_writeRX8025TimerFlag=1;
              return 0;
            }
         }
        
      }  
      return 1;      
}


//------------------------------------------------------------------------------
//解析一个 xxxxx 字符串
//char *comRet:             要解析的字符串指针
//uint8_t n :               第n个次切割的字符串开始有意义 n要大于等于1
//uint8_t *WriteToTable :   要更新的缓冲区指针
//uint8_t maxlen :          最大长度
//uint8_t lenstatic :       1=定长 必须是最大长度    0=忽略 
//uint8_t numFlag :         1=纯数字 0=字符即可
//返回 0=成功 1=失败
uint8_t strDoSubxxx(char *comRet,uint8_t n,uint8_t *WriteToTable,uint8_t maxlen,uint8_t lenstatic,uint8_t numFlag)
{
       char *cmdTemp=comRet;
       char *cmdTemp1;
       char *cmdTemp2;
       uint8_t m=0;
       uint8_t i=0;
       uint16_t j=0;
       if(1<=n)
       {
            n--;
       }
       else
       {
          return 1;
       }
      
          App_Printf("$$ffs->%s\r\n",cmdTemp);
       
      //--------------------------------------------------------------------------
      //该函数返回
      //第一次切割后的字符串 切割后 该整体要切割的字符串已经在strtok函数里有了记录    
      App_Printf("$$%s\r\n",strtok((char *)cmdTemp,(const char*)" "));

      while(cmdTemp1=strtok(NULL,(const char*)" "))
      { 
         m++;          
         if(n<=m)
         {             
            App_Printf("$$%s\r\n",cmdTemp1);
             
            j=strlen(cmdTemp1);
             *(cmdTemp1+j)=0;
            App_Printf("$$strlen=%d\r\n",j);
             
            if(maxlen<j)
            {
                App_Printf("$$len is so long \r\n");
                for(i=0;i<j;i++)
                 {
                     *cmdTemp=0;
                    cmdTemp++;                     
                }
                return 1;
            }
            else if((1==lenstatic)&&(maxlen!=j))
            {
                App_Printf("$$len != maxlen\r\n");
                return 1;
            }
            else
            {
                if(1==numFlag)
                {
                    cmdTemp2=cmdTemp1;
                    for(i=0;i<j;i++)
                    {
                        if( ('0'>(*cmdTemp2) || ('9'<(*cmdTemp2)) ) )
                        {
                            App_Printf("$$%d is no num\r\n",i+1); 
                            for(i=0;i<j;i++)
                             {
                                 *cmdTemp=0;
                                cmdTemp++;                     
                            }                            
                            return 1;
                        }
                        cmdTemp2++;
                    }
                }
                strcpy((char *)WriteToTable,cmdTemp1);
                App_Printf("$$str=%s\r\n",WriteToTable);
                  for(i=0;i<j;i++)
                 {
                     *cmdTemp=0;
                    cmdTemp++;                     
                }
                return 0;
            }
         }
        
      }  

      return 1;      
}



//---------------------------------------------------------------
//1=g_tcpOrwifiFlag 网口   2=g_tcpOrwifiFlag wifi     
void g_showCmdH(uint8_t i,uint8_t g_tcpOrwifiFlag)
{
    uint8_t weekTemp;
    char table[90];
    if(0XA1==i)
    {
        sendcmdH(wdth_showTable0,g_tcpOrwifiFlag);
        sendcmdH(wdth_showTable1,g_tcpOrwifiFlag);
        sendcmdH(wdth_showTable2,g_tcpOrwifiFlag);
    }
    else if(0XB1==i)
    {
        sendcmdH(w5500h_showTable0,g_tcpOrwifiFlag);
        sendcmdH(w5500h_showTable1,g_tcpOrwifiFlag);
        sendcmdH(w5500h_showTable2,g_tcpOrwifiFlag);
        sendcmdH(w5500h_showTable3,g_tcpOrwifiFlag);
        sendcmdH(w5500h_showTable4,g_tcpOrwifiFlag);
        sendcmdH(w5500h_showTable5,g_tcpOrwifiFlag);
        sendcmdH(w5500h_showTable6,g_tcpOrwifiFlag);
        sendcmdH(w5500h_showTable7,g_tcpOrwifiFlag);
        sendcmdH(w5500h_showTable8,g_tcpOrwifiFlag);
    }
    else if(0XB2==i)
    {
        sprintf(table,"w5500 ip=%d.%d.%d.%d",g_w5500_local_ip[0],g_w5500_local_ip[1],g_w5500_local_ip[2],g_w5500_local_ip[3]);
        sendcmdH((const uint8_t*)table,g_tcpOrwifiFlag);   
        sprintf(table,"w5500 sn=%d.%d.%d.%d",g_w5500_subnet[0],g_w5500_subnet[1],g_w5500_subnet[2],g_w5500_subnet[3]);
        sendcmdH((const uint8_t*)table,g_tcpOrwifiFlag);            
        sprintf(table,"w5500 gw=%d.%d.%d.%d",g_w5500_gateway[0],g_w5500_gateway[1],g_w5500_gateway[2],g_w5500_gateway[3]);
        sendcmdH((const uint8_t*)table,g_tcpOrwifiFlag);
    }
    else if(0XC1==i)
    {
        sendcmdH(wifih_showTable0,g_tcpOrwifiFlag);
        sendcmdH(wifih_showTable1,g_tcpOrwifiFlag);
        sendcmdH(wifih_showTable2,g_tcpOrwifiFlag);   
        sendcmdH(wifih_showTable3,g_tcpOrwifiFlag);
        sendcmdH(wifih_showTable4,g_tcpOrwifiFlag); 
        sendcmdH(wifih_showTable5,g_tcpOrwifiFlag);
        sendcmdH(wifih_showTable6,g_tcpOrwifiFlag); 
        sendcmdH(wifih_showTable7,g_tcpOrwifiFlag);
        sendcmdH(wifih_showTable8,g_tcpOrwifiFlag); 
        sendcmdH(wifih_showTable9,g_tcpOrwifiFlag);
        sendcmdH(wifih_showTable10,g_tcpOrwifiFlag); 
        sendcmdH(wifih_showTable11,g_tcpOrwifiFlag);
        sendcmdH(wifih_showTable12,g_tcpOrwifiFlag); 
        sendcmdH(wifih_showTable13,g_tcpOrwifiFlag);
        sendcmdH(wifih_showTable14,g_tcpOrwifiFlag); 
        sendcmdH(wifih_showTable15,g_tcpOrwifiFlag);
        sendcmdH(wifih_showTable16,g_tcpOrwifiFlag); 
        
    }
    else if(0XC2==i)
    {
        //------------------------------------------------------
        //wifi AP自身产生的热点参数
        sprintf(table,"wifi apid=%s",(const char*)&g_wifi_ap_sidd_temp);
        sendcmdH((const uint8_t*)table,g_tcpOrwifiFlag);   
        sprintf(table,"wifi apkey=%s",(const char*)&g_wifi_ap_password_temp);
        sendcmdH((const uint8_t*)table,g_tcpOrwifiFlag);    
        sprintf(table,"wifi apip=%d.%d.%d.%d",g_wifi_ap_ip_temp[0],g_wifi_ap_ip_temp[1],g_wifi_ap_ip_temp[2],g_wifi_ap_ip_temp[3]);
        sendcmdH((const uint8_t*)table,g_tcpOrwifiFlag);   
        sprintf(table,"wifi apsn=%d.%d.%d.%d",g_wifi_ap_sn_temp[0],g_wifi_ap_sn_temp[1],g_wifi_ap_sn_temp[2],g_wifi_ap_sn_temp[3]);
        sendcmdH((const uint8_t*)table,g_tcpOrwifiFlag);            
        sprintf(table,"wifi apgw=%d.%d.%d.%d",g_wifi_ap_gw_temp[0],g_wifi_ap_gw_temp[1],g_wifi_ap_gw_temp[2],g_wifi_ap_gw_temp[3]);
        sendcmdH((const uint8_t*)table,g_tcpOrwifiFlag);
        
        if(1==g_dhcpip_staticip_flag)
        {
            sprintf(table,"NOTICE:wifi STA IP MODEL=[DHCP]");
            sendcmdH((const uint8_t*)table,g_tcpOrwifiFlag);  
        }
        else
        {
            sprintf(table,"NOTICE:wifi STA IP MODEL=[STATIC]");
            sendcmdH((const uint8_t*)table,g_tcpOrwifiFlag);  
        }
        
        //------------------------------------------------------
        //wifi STA 经过网络DHCP 分配的IP
        sprintf(table,"wifi staDHCP_IP=%d.%d.%d.%d",g_wifi_sta_dhcpIp_temp[0],g_wifi_sta_dhcpIp_temp[1],g_wifi_sta_dhcpIp_temp[2],g_wifi_sta_dhcpIp_temp[3]);
        sendcmdH((const uint8_t*)table,g_tcpOrwifiFlag);   

        //------------------------------------------------------
        //wifi STA连接网络参数
        sprintf(table,"wifi stassid=%s",(const char*)&g_wifi_sta_sidd_temp);
        sendcmdH((const uint8_t*)table,g_tcpOrwifiFlag);   
        sprintf(table,"wifi stakey=%s",(const char*)&g_wifi_sta_password_temp);
        sendcmdH((const uint8_t*)table,g_tcpOrwifiFlag);   
        sprintf(table,"wifi staip=%d.%d.%d.%d",g_wifi_sta_ip_temp[0],g_wifi_sta_ip_temp[1],g_wifi_sta_ip_temp[2],g_wifi_sta_ip_temp[3]);
        sendcmdH((const uint8_t*)table,g_tcpOrwifiFlag);   
        sprintf(table,"wifi stasn=%d.%d.%d.%d",g_wifi_sta_sn_temp[0],g_wifi_sta_sn_temp[1],g_wifi_sta_sn_temp[2],g_wifi_sta_sn_temp[3]);
        sendcmdH((const uint8_t*)table,g_tcpOrwifiFlag);            
        sprintf(table,"wifi stagw=%d.%d.%d.%d",g_wifi_sta_gw_temp[0],g_wifi_sta_gw_temp[1],g_wifi_sta_gw_temp[2],g_wifi_sta_gw_temp[3]);
        sendcmdH((const uint8_t*)table,g_tcpOrwifiFlag);
        
        //------------------------------------------------------
        //wifi STA连接远端sever参数
//        sprintf(table,"wifi reip=%d.%d.%d.%d",g_wifi_sta_sever_ip_temp[0],g_wifi_sta_sever_ip_temp[1],g_wifi_sta_sever_ip_temp[2],g_wifi_sta_sever_ip_temp[3]);
//        sendcmdH((const uint8_t*)table,g_tcpOrwifiFlag);   
        sprintf(table,"wifi report=%s",(const char*)&g_wifi_sta_sever_port_temp);
        sendcmdH((const uint8_t*)table,g_tcpOrwifiFlag);            
    }
    else if(0XD1==i)
    {
        sendcmdH(clkh_showTable0,g_tcpOrwifiFlag);
        sendcmdH(clkh_showTable1,g_tcpOrwifiFlag);
        sendcmdH(clkh_showTable2,g_tcpOrwifiFlag);
        sendcmdH(clkh_showTable3,g_tcpOrwifiFlag);           
    }
    else if(0XD2==i)
    {
        while(1==g_readingRX8025TimerFlag)
        {
            osDelay(100);
        }
        g_readingRX8025TimerFlag=1;
        weekTemp= bsp_ReadRtcWeekChange(&g_justTimerStruct);
        sprintf(table,"Y-M-D=%d-%d-%d H:M:S=%d:%d:%d week=%d\r\n",g_justTimerStruct.year,g_justTimerStruct.month,g_justTimerStruct.day,g_justTimerStruct.hour,g_justTimerStruct.minute,g_justTimerStruct.second,weekTemp);
        sendcmdH((const uint8_t*)table,g_tcpOrwifiFlag); 
        g_readingRX8025TimerFlag=0;
    }
    else
    {
        ;
    }
}

void sendcmdH(const uint8_t *str,uint8_t g_tcpOrwifiFlag)
{
//    HAL_StatusTypeDef statusDat=0;    

    uint16_t lenNum;
    char at_buff[20];

    
    lenNum=strlen((char*)str);
    
    if(1==g_tcpOrwifiFlag)
    {       
        send(SOCK_TCPS,&str[0], lenNum);     //向Server发送数据                
        osDelay(50);
    }
    else if(2==g_tcpOrwifiFlag)
    {
        sprintf((char*)at_buff,"AT+CIPSEND=0,%d",lenNum);
        g_atk_mw8266d_send_CIPSEND_cmd(at_buff,"OK",100);  //发送指定长度的数据
        osDelay(50); 
        
        HAL_UART_Transmit(&huart4,(&str[0]), lenNum, 10);
        osDelay(200);
        
//        do
//        {
//             statusDat=HAL_UART_Transmit(&huart4,(&str[0]), lenNum, HAL_MAX_DELAY);
//             
//             if(HAL_OK!=statusDat)
//             {
//                osDelay(200);
//             }
//        }
//        while(HAL_OK!=statusDat);
    }
    else
    {
        ;
    }
}







