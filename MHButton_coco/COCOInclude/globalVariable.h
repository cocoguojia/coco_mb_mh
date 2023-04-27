
#ifndef _GLOBALBARIABLE_H__
#define _GLOBALBARIABLE_H__

#include "myinclude.h"

#define FIRST_PARAAMETER 0X56

//-----------------------------------------------------------------------------------------------------------------
//如果使能锂电池切断功能 请预定义POWER_BATTERY_MANAGEMENT  否则 屏蔽POWER_BATTERY_MANAGEMENT的预定义
//#define  POWER_BATTERY_MANAGEMENT 1
#define  POWER_BATTERY_EN   HAL_GPIO_WritePin(GPIOB, POWER_CONTROL_Pin, GPIO_PIN_SET)     
#define  POWER_BATTERY_NO   HAL_GPIO_WritePin(GPIOB, POWER_CONTROL_Pin, GPIO_PIN_RESET)  

//有些modbus是从地址0开始的，所以我们在这里做个兼容,cocoguojia say
//如果地址是从0开始算 那么请使能下面这句;如果地址是从1开始算 那么请屏蔽下面这句，请用双斜杠屏蔽-->//#define MODBUS_ADDR_0    1
#define MODBUS_ADDR_0    1



#define TCP_WIFI_SENDLEN    120
#define TCP_WIFI_PINGPONG_N    3

#define MB_TCP_BUF_SIZE     ( 100 + 7 )   /* Must hold a complete Modbus TCP frame. */




extern void globalVariable(void);

//------------------------------------------------------------------
//全局变量
extern uint16_t g_keyStateTalbe[6];  //5个按钮状态 1=被按下 0=抬起
extern uint16_t g_keyTimeTalbe[6];   //被按下的累计时间
extern uint16_t g_diNumTalbe[4];     //3路开关量被按下的次数

extern uint8_t g_tcpOrwifiFlag;                  //消息的产生者标志 1=网口 2=wifi 其他=忽略


extern uint8_t g_openFreeModbusFlag;                 //1=开启FreeModbus功能    


//---------------------------------------------------------------------------------
extern uint8_t *g_netBridgeModbusQueueTable[8];         //网络桥接modbus队列指针存储
extern uint8_t g_netBridgeModbusQueueN;                 //网络桥接modbus队列指针 当前序号


extern uint8_t g_holdingChangeFlag;                     //holding 寄存器改版标志 1=改变过    

extern uint8_t g_at_cipstart_n;//at+cipstart 命令的最终序号


extern uint8_t g_dhcpip_staticip_flag;//1=DHCP ip 0=STATIC ip



////////////////////////////////////////////////////////////////////////////////////////////
//wifi模块采用 STA+AP方式
//------------------------------------------------------
//wifi STA连接网络参数
extern char g_wifi_sta_sidd_temp[30];
extern char g_wifi_sta_password_temp[20];

extern char g_wifi_sta_ip_temp[4];
extern char g_wifi_sta_gw_temp[4];
extern char g_wifi_sta_sn_temp[4];

extern char g_wifi_sta_dhcpIp_temp[4];


//------------------------------------------------------
//wifi AP自身产生的热点参数
extern char g_wifi_ap_sidd_temp[20];
extern char g_wifi_ap_password_temp[20];
extern char g_wifi_ap_ip_temp[4];
extern char g_wifi_ap_gw_temp[4];
extern char g_wifi_ap_sn_temp[4];


//------------------------------------------------------
//wifi STA连接远端sever参数
//extern char g_wifi_sta_sever_ip_temp[4];
extern char g_wifi_sta_sever_port_temp[20];

/*定义MAC地址,如果多块W5500网络适配板在同一现场工作，请使用不同的MAC地址*/
//uint8 mac[6]={0x00,0x08,0xdc,0x11,0x11,0x11};
extern uint8_t g_w5500_mac[6];

/*定义默认IP信息*/
extern uint8_t g_w5500_local_ip[4];											/*定义W5500默认IP地址*/
extern uint8_t g_w5500_subnet[4];										/*定义W5500默认子网掩码*/
extern uint8_t g_w5500_gateway[4];											/*定义W5500默认网关*/
extern uint8_t g_w5500_dns_server[4];									    /*定义W5500默认DNS*/

extern uint16_t g_w5500_local_port;	                       					            /*定义本地端口*/

///*定义远端IP信息*/ //其中 远端sever参数 写在程序里不可已设置
//extern uint8_t  g_w5500_remote_ip[4];											/*远端IP地址*/
//extern uint16_t g_w5500_remote_port;													/*远端端口号*/

extern uint8_t g_wifiSendPingPongFLag;   //wifi 发送乒乓缓存  0  1 2 指示标志 指示当前要发送的缓存 这里是3个乒乓球
extern uint8_t g_w5500SendPingPongFLag;  //w5500 发送乒乓缓存 0  1 2 指示标志 指示当前要发送的缓存 这里是3个乒乓球

extern uint8_t g_wifiSendbuff[TCP_WIFI_PINGPONG_N][TCP_WIFI_SENDLEN+2];
extern uint8_t g_w5500Sendbuff[TCP_WIFI_PINGPONG_N][TCP_WIFI_SENDLEN+2];	


extern uint8_t g_writeRX8025TimerFlag;//1=写一次rx8025的时间	
extern uint8_t g_readingRX8025TimerFlag;//0=当前时间可以占用 注意占用就置位 然后释放 变量形式的互斥变量

extern uint8_t g_w5500EnFlag;   //1=W5500模块使能
extern uint8_t g_wifiEnFlag;     //1=Wifi模块使能

extern uint8_t g_wifi_receive_data_num; //接收到的新数据的次数  由g_atk_mw8266d_monitor函数 负责先关处理并且清零


//-------------------------------------------------------------------
//net数据桥接Modbus的 消息队列句柄
extern osMessageQueueId_t netBridgeModbusQueueHandle;
//-------------------------------------------------------------------
//wifi要发送的数据队列 消息队列句柄
extern osMessageQueueId_t wifiSendQueueHandle;
//-------------------------------------------------------------------
//w5500要发送的数据队列 消息队列句柄
extern osMessageQueueId_t w5500SendQueueHandle;
//-----------------------------------------------------------------------------------------------------------------------------------
//网口或者wifi待处理缓存指针处理互斥变量句柄 就是给NetBridgeModbus发送消息队列之前 用来互斥
extern osMutexId_t PendingCachePointsMutexHandle;
//-----------------------------------------------------------------------
//读写 保持寄存器权杖 互斥信号量变量句柄 注意保持寄存器的操作都要用这个限定
extern osMutexId_t ReadWriteRegHoldingMutexHandle;
//-----------------------------------------------------------------------
//读写 W25q64的互斥信号量句柄
extern osMutexId_t W25q64MutexHandle;


//地址=0 第1扇区 保存5个按钮和3路开关量相关信息

//地址=4096  第2扇区 w5500 ip相关信息
//地址=4096*2 第3扇区 w5500 sn相关信息
//地址=4096*3 第4扇区 w5500 gw相关信息
//地址=4096*4 第5扇区 w5500 en/no相关信息

//地址=4096*10 第11扇区 wifi apid相关信息
//地址=4096*11 第12扇区 wifi apkey 相关信息
//地址=4096*12 第13扇区 wifi apip相关信息
//地址=4096*13 第14扇区 wifi apsn相关信息
//地址=4096*14 第15扇区 wifi apgw 相关信息

//地址=4096*15 第16扇区 wifi stassid相关信息
//地址=4096*16 第17扇区 wifi stakey 相关信息
//地址=4096*17 第18扇区 wifi staip相关信息
//地址=4096*18 第19扇区 wifi stasn相关信息
//地址=4096*19 第20扇区 wifi stagw 相关信息

//地址=4096*20 第21扇区 wifi reip相关信息 远端IP
//地址=4096*21 第22扇区 wifi report相关信息 远端端口

//地址=4096*22 第23扇区 wifi en/no相关信息
//地址=4096*23 第24扇区 wifi dhcp en/no相关信息


//地址=4096*29 第30扇区 则是设置过参数 否则默认参数




#define WQ_PAGE_SW5ROAD3         ((uint32_t)(4096*0))

#define WQ_PAGE_W5500_IP         ((uint32_t)(4096*1))
#define WQ_PAGE_W5500_SN         ((uint32_t)(4096*2))
#define WQ_PAGE_W5500_GW         ((uint32_t)(4096*3))
#define WQ_PAGE_W5500_ENNO       ((uint32_t)(4096*4))

#define WQ_PAGE_WIFI_APID        ((uint32_t)(4096*10))
#define WQ_PAGE_WIFI_APKEY       ((uint32_t)(4096*11))
#define WQ_PAGE_WIFI_AP_IP       ((uint32_t)(4096*12))
#define WQ_PAGE_WIFI_AP_SN       ((uint32_t)(4096*13))
#define WQ_PAGE_WIFI_AP_GW       ((uint32_t)(4096*14))

#define WQ_PAGE_WIFI_STASSID    ((uint32_t)(4096*15))
#define WQ_PAGE_WIFI_STAKEY     ((uint32_t)(4096*16))
#define WQ_PAGE_WIFI_STA_IP     ((uint32_t)(4096*17))
#define WQ_PAGE_WIFI_STA_SN     ((uint32_t)(4096*18))
#define WQ_PAGE_WIFI_STA_GW     ((uint32_t)(4096*19))

#define WQ_PAGE_WIFI_REIP       ((uint32_t)(4096*20))
#define WQ_PAGE_WIFI_REPORT     ((uint32_t)(4096*21))

#define WQ_PAGE_WIFI_ENNO       ((uint32_t)(4096*22))
#define WQ_PAGE_WIFI_DHCP_ENNO  ((uint32_t)(4096*23))

//---------------------------------------------------
//是设置过参数 否则默认参数
#define WQ_PAGE_DEFAULT        ((uint32_t)(4096*29))


extern const uint8_t wdth_showTable0[];
extern const uint8_t wdth_showTable1[];
extern const uint8_t wdth_showTable2[];
extern const uint8_t wdth_showTable3[];

extern const uint8_t w5500h_showTable0[];
extern const uint8_t w5500h_showTable1[];
extern const uint8_t w5500h_showTable2[];
extern const uint8_t w5500h_showTable3[];
extern const uint8_t w5500h_showTable4[];
extern const uint8_t w5500h_showTable5[];
extern const uint8_t w5500h_showTable6[];
extern const uint8_t w5500h_showTable7[];
extern const uint8_t w5500h_showTable8[];


extern const uint8_t wifih_showTable0[];
extern const uint8_t wifih_showTable1[];
extern const uint8_t wifih_showTable2[];
extern const uint8_t wifih_showTable3[];
extern const uint8_t wifih_showTable4[];
extern const uint8_t wifih_showTable5[];
extern const uint8_t wifih_showTable6[];
extern const uint8_t wifih_showTable7[];
extern const uint8_t wifih_showTable8[];
extern const uint8_t wifih_showTable9[];
extern const uint8_t wifih_showTable10[];
extern const uint8_t wifih_showTable11[];
extern const uint8_t wifih_showTable12[];
extern const uint8_t wifih_showTable13[];
extern const uint8_t wifih_showTable14[];
extern const uint8_t wifih_showTable15[];
extern const uint8_t wifih_showTable16[];



extern const uint8_t clkh_showTable0[];
extern const uint8_t clkh_showTable1[];
extern const uint8_t clkh_showTable2[];
extern const uint8_t clkh_showTable3[];




#endif

