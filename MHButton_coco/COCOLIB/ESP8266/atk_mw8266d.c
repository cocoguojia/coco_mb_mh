



#include "atk_mw8266d.h"

#include "myinclude.h"

#include "atk_mw8266d_struct.h"



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//全局变量


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//内部全局相关函数
uint8_t AT_MW8266_InitSub(void);
uint8_t atk_mw8266d_monitor(void);
uint8_t atk_mw8266d_send_CIPSEND_cmd(char* cmd,char* ack,uint16_t timeout);
uint16_t strIptoNum255(char *str);
uint8_t strIpSub(char *comRet,uint8_t n);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//内部静态变量
#define AT_CIPSTATUS_N (g_at_cipstart_n+1)
#define AT_CIFSR_N (g_at_cipstart_n+2)

char *CMDretFirst = NULL;
char *CMDret = NULL;

typedef struct
{
    
    char* cmd;                  //最终要发送的AT命令
    const char* ack;            //AT命令 所希望的回复
    uint32_t timeout;           //AT命令 一次等待超时时间
    const char *okTable;        //AT命令 成功后的提示
    const char *ErrTable;       //AT命令 失败后的提示
    uint16_t afterDelayMs;      //成功后 额外延时

    uint8_t special_n;          //可能用到的参数变量 
    char  *special_char1;       //可能用到的参数变量 
    char  *special_char2;       //可能用到的参数变量 
    char  *special_char3;       //可能用到的参数变量  
}AT_MW8266_strcutDef;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//内部静态函数
AT_MW8266_strcutDef AT_MW8266_strcut[15];
uint8_t atk_mw8266dDoWhitOneAtCmd(AT_MW8266_strcutDef *t);
uint8_t atk_mw8266dSendAtCmd(AT_MW8266_strcutDef *t);

void AT_MW8266_strcut_init(void);

uint8_t atk_mw8266d_monitor_second(void);
uint8_t atk_mw8266dSendAtCmdStatus(AT_MW8266_strcutDef *t);

uint8_t charIpDoSubxxx(char *comRet,uint8_t n,uint8_t *WriteToTable);

uint8_t atk_mw8266dSendAtCmdIP(AT_MW8266_strcutDef *t);



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//全局函数
//----------------------------------------------------------------------------------------------------------------------------------
//wifi模块 AT命令初始化 
uint8_t g_atk_AT_MW8266_InitSub(void)
{
    uint8_t n;
    uint8_t ret=0;
    AT_MW8266_strcut_init();            //wifi模块 AT命令结构体初始化赋值
    atinit_point:
    ret=0;
    for(n=0;g_at_cipstart_n>=n;n++)     //我们这里用一个全局变量表示 AT初始化最后一步的序号值
    {
        if(0==ret)
        {
            ret+=atk_mw8266dDoWhitOneAtCmd(&AT_MW8266_strcut[n]);   //发送AT命令 并且检测ACK
        }
        else
        {
            osDelay(2000);
            App_Printf("\r\n$$#### wifi is wrong goto AT int ####\r\n");
            goto atinit_point;
        }
        osDelay(10);
    } 
    return ret;
}

//----------------------------------------------------------------------------------------------------------------------------------
//wifi模块 获取IP程序段 cocoguojia 待加
uint8_t g_atk_mw8266d_getApStaIp(void)
{
    App_Printf("$$//########################\r\n"); 
    if(1==g_dhcpip_staticip_flag)
    {
        App_Printf("$$IP is DHCP IP\r\n"); 
        atk_mw8266d_uart_rx_restart();
        if(ATK_MW8266D_EOK!=atk_mw8266dSendAtCmdIP(&AT_MW8266_strcut[AT_CIFSR_N]))
        {            
            App_Printf("$$IP show is Err\r\n");                        
        }
        else
        {      
            App_Printf("$$IP show is OK\r\n");         
        }
    }
    else
    {
         
         App_Printf("$$IP is static IP\r\n"); 
    }
}


//----------------------------------------------------------------------------------------------------------------------------------
//wifi模块 运行监控 网络断链要相应的处理一下
uint8_t g_atk_mw8266d_monitor(void)
{
    static uint8_t n=0;
//    static uint8_t LastStatus=1;        //1=读状态正常  0=状态不正常
//    static uint8_t LastclinkServer=1;   //1=连接服务器正常  0=连接服务器不正常    
    uint8_t returnN=0;
    n++;
    
    if(50<=n)//50  5秒一检测
    { 
        if(0==g_wifi_receive_data_num)
        {
            if(ATK_MW8266D_EOK!=atk_mw8266dSendAtCmdStatus(&AT_MW8266_strcut[AT_CIPSTATUS_N]))
            {            

//                if(1==LastStatus)
                {
                    App_Printf("$$STATUS is Err\r\n");
//                    LastStatus=0;
//                    LastclinkServer=0;
                }
                
                 if(ATK_MW8266D_EOK==atk_mw8266dSendAtCmd(&AT_MW8266_strcut[g_at_cipstart_n]))  //连接TCP服务器
                 {
//                     if(0==LastclinkServer)
                     {
                        App_Printf("$$clink SERVER CMD is OK\r\n");
//                        LastclinkServer=1;
                     }
                 }
                 else
                 {
//                     if(1==LastclinkServer)
                     {
                        App_Printf("$$clink SERVER CMD is Err\r\n");
//                        LastclinkServer=0;
                     }
                 }
                 
            }
            else
            {
//                if(0==LastStatus)
                {
                    App_Printf("$$STATUS is nice\r\n");
//                    LastStatus=1;
                }
//                LastclinkServer=1;
                CMDret =CMDretFirst;
                returnN=atk_mw8266d_monitor_second();
            }
        }
        else
        {
            App_Printf("$$STATUS is nice,receive data in 5s\r\n");
            g_wifi_receive_data_num=0;
        }
        n=0;       
        
    }
    else
    {
        CMDret = (char*)atk_mw8266d_uart_rx_get_frame();
        atk_mw8266d_uart_rx_restart();
        returnN=atk_mw8266d_monitor_second();
    }
    return returnN;    
}


//------------------------------------------------------------------------------------------------------
//wifi发送透传数据前 要发送 CIPSEND命令 说明要发送的长度 即发送前要用AT命令
uint8_t g_atk_mw8266d_send_CIPSEND_cmd(char* cmd,char* ack,uint16_t timeout)
{
    uint8_t *ret = NULL;
    
    atk_mw8266d_uart_rx_restart();
    
    atk_mw8266d_uart_printf("%s\r\n", cmd);
    
    if ((ack == NULL) || (timeout == 0))
    {
        return ATK_MW8266D_EOK;
    }
    else
    {
        while (timeout  > 0)
        {
            ret = atk_mw8266d_uart_rx_get_frame();
            if (ret != NULL)
            {
                if (strstr((const char *)ret, ack) != NULL)
                {
                    return ATK_MW8266D_EOK;
                }
                else
                {
                    atk_mw8266d_uart_rx_restart();
                }
            }
            timeout --;
            osDelay(1);
        }
        
        return ATK_MW8266D_ETIMEOUT;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//内部静态函数
//---------------------------------------------------------------------
//发送STATUS命令专用函数

//--------------------------------------------------------------------------------------------
#define CMD_X_SECOND 2
#define CMD_Y_SECOND 20
const char  CMD_TABLE_SECOND[CMD_X_SECOND][CMD_Y_SECOND]={"CONNECT FAIL","Link type Error"};
uint8_t buffWifi[MB_TCP_BUF_SIZE+3];				                    /*定义一个缓存*/

uint8_t atk_mw8266d_monitor_second(void)
{
    char *ret = NULL;
    char *ret1 = NULL;
    char *retWifi = NULL;
    uint8_t lenError = 0;
    uint8_t n=0;
    uint16_t len=0;
    uint16_t i=0;
    uint16_t msg;

    if (CMDret != NULL)
    {
        for(n=0;n<CMD_X_SECOND;n++)
        {
            ret=strstr((const char *)CMDret, &(CMD_TABLE_SECOND[n][0])); 
            if(NULL!=ret)
            {
                //检测是否有重大错误 有则需要重新初始化wifi模块
                //"CONNECT FAIL"
                //"Link type Error"
                g_wifi_receive_data_num++;
                App_Printf("$$\r\n###WIFI Gross Mistake=%d###\r\n",n);
                return 1;
            }
        }  
 
        ret=strstr((const char *)CMDret,"+IPD,");
        if(NULL!=ret)
        {
            g_wifi_receive_data_num++;
            ret+=5;            
            ret1=strstr((const char *)ret,",");
            ret1++;
            
            //--------------------------------------
            //定位到数值的地方 去数据长度len 
            if(('0'<=*ret1)&&('9'>=*ret1))
            {
                len=(*ret1)-'0';
                ret1++;
                if(':'==(*ret1))
                {
                      ret1++;
                      retWifi=ret1;
                      //App_Printf("$$ modbus cmd len=%d\r\n",len);//modbus
                      lenError=0;
                }
                else if(('0'<=(*ret1))&&('9'>=(*ret1)))
                {
                    len*=10;
                    len+=((*ret1)-'0');
                    ret1++;
                    if(':'==(*ret1))
                    {
                        ret1++;
                        retWifi=ret1;
                        //App_Printf("$$ modbus cmd len=%d\r\n",len);//modbus
                        lenError=0;
                    }
                    else
                    {
                        App_Printf("$$the dat Error1=%c\r\n",*ret1);//乱码
                        lenError=1;
                    }
                }
                else
                {
                      App_Printf("$$the dat Error2=%c\r\n",*ret1);//乱码
                      lenError=1;
                }
            }
            else
            {
                App_Printf("$$dat Error0=%s\r\n",ret1);//乱码
                lenError=1;
            } 
        }
        else
        {
            lenError=1;//空也是 乱码
        }                
        
        //-----------------------------------------------------------------------------
        //长度不为0 且 不为乱码 且 不超过MB_TCP_BUF_SIZE个字节
        //这里面可能是user cmd 可能是modbus data 也可能就是错误的数据
        //都扔给 NetBridgeModbus 任务统一处理
        if((0<len)&&(0==lenError)&&(MB_TCP_BUF_SIZE>=len))
        {
            for(i=0;i<len;i++)
            {
                buffWifi[i+3]=*retWifi;
                retWifi++;
            }
            
  
            
            buffWifi[0]=(uint8_t)(len>>8);  			//首位为消息的实际长度高字节
            buffWifi[1]=(uint8_t)(len&0x00ff);   		//第二位为消息的实际长度低字节
            buffWifi[2]=2;  							//第三位为1表示网口  为2表示wifi
             

            //##############################################################################################
            //互斥操作
            osMutexAcquire(PendingCachePointsMutexHandle, osWaitForever);   //一直等待获取互斥资源 
 
            if(8>g_netBridgeModbusQueueN) 
            {
                msg=g_netBridgeModbusQueueN;
                g_netBridgeModbusQueueTable[msg]=&buffWifi[0];              //网络桥接modbus队列指针存储
                g_netBridgeModbusQueueN++;
            }
            else
            {
                g_netBridgeModbusQueueN=0;
                
                msg=g_netBridgeModbusQueueN;
                g_netBridgeModbusQueueTable[msg]=&buffWifi[0];              //网络桥接modbus队列指针存储
                g_netBridgeModbusQueueN++;
            }
            osMutexRelease(PendingCachePointsMutexHandle);                  //释放互斥资源    
            //##############################################################################################           
            //App_Printf("$$WIFI msg=%d\r\n",msg);
            osDelay(1);
            osMessageQueuePut(netBridgeModbusQueueHandle,&msg, 0U, 100U);
            //App_Printf("$$WIFI PutQueue->netBridgeModbus msg=%d\r\n",msg);            
        }
    }
    return 0;
}

//+CIFSR:APIP,"192.168.1.1"
//+CIFSR:APMAC,"b6:8a:0a:ed:5b:27"
//+CIFSR:STAIP,"192.168.43.182"
//+CIFSR:STAMAC,"b4:8a:0a:ed:5b:27"
uint8_t atk_mw8266dSendAtCmdIP(AT_MW8266_strcutDef *t)
{   
    atk_mw8266d_uart_printf("%s\r\n", t->cmd);
    osDelay(200);
    CMDretFirst = (char *)atk_mw8266d_uart_rx_get_frame();
    if (strstr((const char *)CMDretFirst, t->ack) != NULL)
    {
        charIpDoSubxxx(CMDretFirst,3,(uint8_t*)&g_wifi_sta_dhcpIp_temp[0]);
        App_Printf("$$####STA IP->%d,%d,%d,%d\r\n",g_wifi_sta_dhcpIp_temp[0],g_wifi_sta_dhcpIp_temp[1],g_wifi_sta_dhcpIp_temp[2],g_wifi_sta_dhcpIp_temp[3]);
        atk_mw8266d_uart_rx_restart();
        return ATK_MW8266D_EOK;
    }
    else
    {
        ;
    }
    return ATK_MW8266D_ETIMEOUT;
}

uint8_t atk_mw8266dSendAtCmdStatus(AT_MW8266_strcutDef *t)
{   
    atk_mw8266d_uart_printf("%s\r\n", t->cmd);
    osDelay(200);
    CMDretFirst = (char *)atk_mw8266d_uart_rx_get_frame();
    if (strstr((const char *)CMDretFirst, t->ack) != NULL)
    {
        atk_mw8266d_uart_rx_restart();
        return ATK_MW8266D_EOK;
    }
    else
    {
        ;
    }
    return ATK_MW8266D_ETIMEOUT;
}

//---------------------------------------------------------------
//调用发送AT命令atk_mw8266dSendAtCmd函数 输出调信息
//成功返回0 失败返回1
uint8_t atk_mw8266dDoWhitOneAtCmd(AT_MW8266_strcutDef *t)
{
    uint8_t n=0;
    do{
        if(ATK_MW8266D_EOK==atk_mw8266dSendAtCmd(t))
        {
            n=100;
            App_Printf("%s -->ok\r\n",t->cmd);
            osDelay(t->afterDelayMs);
            atk_mw8266d_uart_rx_restart();
            return 0;
        }
        else
        {
            n++;
            osDelay(1000);
        }
    }while(3>n);
    App_Printf("%s -->err\r\n",t->cmd);
    return 1;
}

//---------------------------------------------------------------
//发送命令 根据结构体的AT参数
uint8_t atk_mw8266dSendAtCmd(AT_MW8266_strcutDef *t)
{
    uint8_t *ret = NULL;
    
    atk_mw8266d_uart_rx_restart();
    
    atk_mw8266d_uart_printf("%s\r\n", t->cmd);
    
    if ((t->ack == NULL) || (t->timeout == 0))
    {
        return ATK_MW8266D_EOK;
    }
    else
    {
        while (t->timeout  > 0)
        {
            ret = atk_mw8266d_uart_rx_get_frame();
            if (ret != NULL)
            {
                if (strstr((const char *)ret, t->ack) != NULL)
                {
                    return ATK_MW8266D_EOK;
                }
                else
                {
                    atk_mw8266d_uart_rx_restart();
                }
            }
            t->timeout --;
            osDelay(1);
        }
        
        return ATK_MW8266D_ETIMEOUT;
    }
}



//---------------------------------------------------------------------------------------------------------------------
//指针在用之前，一定要有所指，不能为空
//"xxxxx" 例如:"AT+RESTORE" 这种固定的字符串，keil编译的时候会分配给flash，所以指针是固定的，得以保存
#define WIFI_WCMMODE_CMD_N 3
#define WIFI_ATE_CMD_N 0
//#define WIFI_ATE_CMD_N 1
char cmdTableStrAETX[8];
char cmdTableStrMODEX[15];
char cmdTable_CIPSTA[70];
char cmdTable_CIPAP[70];
char cmdTable_CWSAP[70];
char cmdTable_CWJAP[85];
char cmdTable_CIPSERVER[20];
char cmdTable_CIPSTART[70];
char cmdTable_CWDHCP[20];



void AT_MW8266_strcut_init(void)
{
    uint8_t i=0;
   
    //------------------------------------------------------------------
    //发送指令恢复wifi模块出厂默认参数
    AT_MW8266_strcut[i].cmd="AT+RESTORE";
    AT_MW8266_strcut[i].ack="ready";
    AT_MW8266_strcut[i].timeout=3000;
    AT_MW8266_strcut[i].afterDelayMs=200;
    i++;
    
    //------------------------------------------------------------------
    //发送AT测试命令
    AT_MW8266_strcut[i].cmd="AT";
    AT_MW8266_strcut[i].ack="OK";
    AT_MW8266_strcut[i].timeout=500;
    AT_MW8266_strcut[i].afterDelayMs=200;
    i++;
    
    //------------------------------------------------------------------
    //发送wifi mode方式命令  
    AT_MW8266_strcut[i].ack="OK";
    AT_MW8266_strcut[i].timeout=500;
    AT_MW8266_strcut[i].afterDelayMs=200;
    switch(WIFI_WCMMODE_CMD_N)
    {
        case 1:
        strcpy(cmdTableStrMODEX,"AT+CWMODE=1");
        AT_MW8266_strcut[i].cmd=cmdTableStrMODEX;//Station模式
        break;
 
        case 2:
        strcpy(cmdTableStrMODEX,"AT+CWMODE=2");
        AT_MW8266_strcut[i].cmd=cmdTableStrMODEX;//AP模式
        break;
    
        case 3:
        strcpy(cmdTableStrMODEX,"AT+CWMODE=3");
        AT_MW8266_strcut[i].cmd=cmdTableStrMODEX;//AP+Station模式
        break;
        
        default:
        strcpy(cmdTableStrMODEX,"AT+CWMODE=3");
        AT_MW8266_strcut[i].cmd=cmdTableStrMODEX;//AP+Station模式
        break;
    }
    i++;
      
    
    //------------------------------------------------------------------
    //发送 软件复位
    AT_MW8266_strcut[i].cmd="AT+RST";
    AT_MW8266_strcut[i].ack="OK";
    AT_MW8266_strcut[i].timeout=500;
    AT_MW8266_strcut[i].afterDelayMs=3000;
    i++;
    
    //------------------------------------------------------------------
    //发送 回显 0=关/1=开 指令
    AT_MW8266_strcut[i].ack="OK";
    AT_MW8266_strcut[i].timeout=500;
    AT_MW8266_strcut[i].afterDelayMs=1000;
//    AT_MW8266_strcut[i].special_n=1;      
    switch(WIFI_ATE_CMD_N)
    {
        case 0:
        strcpy(cmdTableStrAETX,"ATE0");
        AT_MW8266_strcut[i].cmd=cmdTableStrAETX;//关回显
        break;

        case 1:
        strcpy(cmdTableStrAETX,"ATE1");
        AT_MW8266_strcut[i].cmd=cmdTableStrAETX;//开回显
        break;
        
        default:
        strcpy(cmdTableStrAETX,"ATE0");
        AT_MW8266_strcut[i].cmd=cmdTableStrAETX;//关回显
        break;
    }    
    i++;
    
    if(0==g_dhcpip_staticip_flag)//WIFI 自身为静态IP
    {
        //------------------------------------------------------------------
        //发送 设置sta参数 指令
        AT_MW8266_strcut[i].ack="OK";
        AT_MW8266_strcut[i].timeout=1000;
        AT_MW8266_strcut[i].afterDelayMs=2000;
        AT_MW8266_strcut[i].special_char1=g_wifi_sta_ip_temp;
        AT_MW8266_strcut[i].special_char2=g_wifi_sta_gw_temp;
        AT_MW8266_strcut[i].special_char3=g_wifi_sta_sn_temp;
        sprintf(cmdTable_CIPSTA,"AT+CIPSTA=\"%d.%d.%d.%d\",\"%d.%d.%d.%d\",\"%d.%d.%d.%d\"", \
        AT_MW8266_strcut[i].special_char1[0],AT_MW8266_strcut[i].special_char1[1],AT_MW8266_strcut[i].special_char1[2],AT_MW8266_strcut[i].special_char1[3], \
        AT_MW8266_strcut[i].special_char2[0],AT_MW8266_strcut[i].special_char2[1],AT_MW8266_strcut[i].special_char2[2],AT_MW8266_strcut[i].special_char2[3], \
        AT_MW8266_strcut[i].special_char3[0],AT_MW8266_strcut[i].special_char3[1],AT_MW8266_strcut[i].special_char3[2],AT_MW8266_strcut[i].special_char3[3]);  
        AT_MW8266_strcut[i].cmd=cmdTable_CIPSTA; 
        i++;
    }
    else//WIFI DHCP 模式开启
    {
        //------------------------------------------------------------------
        //发送 设置sta DHCP 指令
        AT_MW8266_strcut[i].ack="OK";
        AT_MW8266_strcut[i].timeout=2000;
        AT_MW8266_strcut[i].afterDelayMs=1000;
        sprintf(cmdTable_CWDHCP,"AT+CWDHCP=1,1"); 
        AT_MW8266_strcut[i].cmd=cmdTable_CWDHCP; 
        i++;
    }
    
    //------------------------------------------------------------------
    //发送 设置AP参数 指令
    AT_MW8266_strcut[i].ack="OK";
    AT_MW8266_strcut[i].timeout=2000;
    AT_MW8266_strcut[i].afterDelayMs=1000;   
    AT_MW8266_strcut[i].special_char1=g_wifi_ap_ip_temp;
    AT_MW8266_strcut[i].special_char2=g_wifi_ap_gw_temp;
    AT_MW8266_strcut[i].special_char3=g_wifi_ap_sn_temp;    
    //设置自身无线参数:ssid,密码 通道号 加密方式
    sprintf(cmdTable_CIPAP,"AT+CIPAP=\"%d.%d.%d.%d\",\"%d.%d.%d.%d\",\"%d.%d.%d.%d\"", \
    AT_MW8266_strcut[i].special_char1[0],AT_MW8266_strcut[i].special_char1[1],AT_MW8266_strcut[i].special_char1[2],AT_MW8266_strcut[i].special_char1[3], \
    AT_MW8266_strcut[i].special_char2[0],AT_MW8266_strcut[i].special_char2[1],AT_MW8266_strcut[i].special_char2[2],AT_MW8266_strcut[i].special_char2[3], \
    AT_MW8266_strcut[i].special_char3[0],AT_MW8266_strcut[i].special_char3[1],AT_MW8266_strcut[i].special_char3[2],AT_MW8266_strcut[i].special_char3[3]);  
    AT_MW8266_strcut[i].cmd=cmdTable_CIPAP;  
    i++;
    
    //------------------------------------------------------------------
    //发送 产生AP 指令
    AT_MW8266_strcut[i].ack="OK";
    AT_MW8266_strcut[i].timeout=1000;
    AT_MW8266_strcut[i].afterDelayMs=1000;    
    AT_MW8266_strcut[i].special_char1=g_wifi_ap_sidd_temp;
    AT_MW8266_strcut[i].special_char2=g_wifi_ap_password_temp;  
    // ATK-MW8266D建立WIFI 热点  AP
    sprintf(cmdTable_CWSAP,"AT+CWSAP=\"%s\",\"%s\",5,3",AT_MW8266_strcut[i].special_char1,AT_MW8266_strcut[i].special_char2);  
    AT_MW8266_strcut[i].cmd=cmdTable_CWSAP;  
    i++;
    
    //------------------------------------------------------------------
    //发送 加入AP 指令
    AT_MW8266_strcut[i].ack="WIFI GOT IP";
    AT_MW8266_strcut[i].timeout=10000;
    AT_MW8266_strcut[i].afterDelayMs=5000;   
    AT_MW8266_strcut[i].special_char1=g_wifi_sta_sidd_temp;
    AT_MW8266_strcut[i].special_char2=g_wifi_sta_password_temp;  
    //设置自身无线参数:ssid,密码 通道号 加密方式
    sprintf(cmdTable_CWJAP,"AT+CWJAP=\"%s\",\"%s\"",AT_MW8266_strcut[i].special_char1,AT_MW8266_strcut[i].special_char2);  
    AT_MW8266_strcut[i].cmd=cmdTable_CWJAP;  
    i++;
    
    //---------------------------------------------------------------------------------------
    //发送  ATK-MW8266开启双链接 指令
    AT_MW8266_strcut[i].cmd="AT+CIPMUX=1";
    AT_MW8266_strcut[i].ack="OK";
    AT_MW8266_strcut[i].timeout=10000;
    AT_MW8266_strcut[i].afterDelayMs=500;
    i++;
    
    //-------------------------------------------------------------------------------------
    //发送  建立服务器的端口 指令
    AT_MW8266_strcut[i].ack="OK";
    AT_MW8266_strcut[i].timeout=200;
    AT_MW8266_strcut[i].afterDelayMs=500;
    AT_MW8266_strcut[i].special_char1=g_wifi_sta_sever_port_temp;
    sprintf(cmdTable_CIPSERVER,"AT+CIPSERVER=1,%s",AT_MW8266_strcut[i].special_char1);
    AT_MW8266_strcut[i].cmd=cmdTable_CIPSERVER;         
    i++;
    
    //-------------------------------------------------------------------------------------
    //发送  设置服务器超时时间，超时后断开客户端的连接 指    
    //WIFI作为服务器多长了时间clink都没发数据 则会把它踢掉 单位秒  目前默认6分钟即360秒
    AT_MW8266_strcut[i].cmd="AT+CIPSTO=360";
    AT_MW8266_strcut[i].ack="OK";
    AT_MW8266_strcut[i].timeout=200;
    AT_MW8266_strcut[i].afterDelayMs=500;
    g_at_cipstart_n=i; 
    App_Printf("$$ the g_at_cipstart_n=%d\r\n",g_at_cipstart_n);  
    i++;
    
//    //-------------------------------------------------------------------------------------
//    //发送 连接服务器 指令
//    AT_MW8266_strcut[i].ack="OK";
//    AT_MW8266_strcut[i].timeout=2000;
//    AT_MW8266_strcut[i].afterDelayMs=1000;   
//    AT_MW8266_strcut[i].special_char1=g_wifi_sta_sever_ip_temp;
//    AT_MW8266_strcut[i].special_char2=g_wifi_sta_sever_port_temp;  
//    sprintf(cmdTable_CIPSTART,"AT+CIPSTART=0,\"TCP\",\"%d.%d.%d.%d\",%s,%s", \
//    AT_MW8266_strcut[i].special_char1[0],AT_MW8266_strcut[i].special_char1[1],AT_MW8266_strcut[i].special_char1[2],AT_MW8266_strcut[i].special_char1[3], \
//    AT_MW8266_strcut[i].special_char2,"30");  
//    AT_MW8266_strcut[i].cmd=cmdTable_CIPSTART; 
//    i++;
    
    //------------------------------------------------------------------
    //发送  查询状态 指令
    AT_MW8266_strcut[i].cmd="AT+CIPSTATUS";
    AT_MW8266_strcut[i].ack="+CIPSTATUS:";
    AT_MW8266_strcut[i].timeout=500;
    AT_MW8266_strcut[i].afterDelayMs=500;
    i++;
    
    //------------------------------------------------------------------
    //发送  查询当前IP
    AT_MW8266_strcut[i].cmd="AT+CIFSR";
    AT_MW8266_strcut[i].ack="OK";
//    AT_MW8266_strcut[i].special_n=0x0A;//取IP专属 值
    AT_MW8266_strcut[i].timeout=2000;
    AT_MW8266_strcut[i].afterDelayMs=500;
    i++;
       
    atk_mw8266d_uart_rx_restart();    
}



//---------------------------------------------------------------------------------------------------------
//解析xxx xxx xxx xxx 这里是对获取到的IP进行了切割和处理 最终取值赋值给g_wifi_sta_dhcpIp_temp
//char *comRet:要解析的字符串指针
//uint8_t n : 第n个次切割的字符串开始有意义 n要大于等于1
//返回 0=成功 1=失败
uint8_t charIpDoSubxxx(char *comRet,uint8_t n,uint8_t *WriteToTable)
{
       uint8_t charTable[5]={0,0,0,0,0};
       char *cmdTemp=comRet;
       char *cmdTemp1;
       char *cmdTemp2;
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
      App_Printf("$$%s\r\n",strtok((char *)cmdTemp,(const char*)"+"));

      while(cmdTemp1=strtok(NULL,(const char*)"+"))
      { 
         m++;          
         if(n==m)
         {             
            App_Printf("$$%s\r\n",cmdTemp1);
            App_Printf("$$%s\r\n",strtok((char *)cmdTemp1,(const char*)"\""));
            while(cmdTemp2=strtok(NULL,(const char*)"\"."))
            { 
                j=strIptoNum255(cmdTemp2);
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
        
      }  
      return 1;      
}

//--------------------------------------
//返回 0xffff表示失败 其他是数值
uint16_t strIptoNum255(char *str)
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

