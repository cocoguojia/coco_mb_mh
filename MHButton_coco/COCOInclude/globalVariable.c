
#include "globalVariable.h"
#include "BL8025.h"

//------------------------------------------------------------------
//全局变量

////#######################################################################################################################
////W5500 网口 定义远端IP信息  
//uint8_t  g_w5500_remote_ip[4]={192,168,0,51};											/*远端IP地址*/
//uint16_t g_w5500_remote_port=6800;													    /*远端端口号*/
////#######################################################################################################################


uint16_t g_keyStateTalbe[6]={0,0,0,0,0,0};          //5个按钮状态 1=被按下 0=抬起
uint16_t g_keyTimeTalbe[6]={0,0,0,0,0,0};           //5个按钮被按下的累计时间

uint16_t g_diNumTalbe[4]={0,0,0,0};                 //3路开关量被按下的次数

uint8_t g_tcpOrwifiFlag=0;                          //消息的产生者标志 1=网口 2=wifi 其他=忽略

uint8_t g_openFreeModbusFlag=0;                     //1=开启FreeModbus功能   

uint8_t g_holdingChangeFlag=0;                     //holding 寄存器改版标志 1=改变过    




uint8_t g_at_cipstart_n=0;//at+cipstart 命令的最终序号

//---------------------------------------------------------------------------------------------------------------
//CMSIS OS2封装操作系统的坑 消息队列最大只能传递一个16位的值 指针还是32位 传递16位很脑残
uint8_t *g_netBridgeModbusQueueTable[8]={0,0,0,0,0,0,0,0};              //网络桥接modbus队列指针存储
uint8_t g_netBridgeModbusQueueN=0;                                      //网络桥接modbus队列指针 当前序号





//------------------------------------------------------
//wifi AP自身产生的热点参数
char g_wifi_ap_sidd_temp[20]={"MHAUTO_6"};
char g_wifi_ap_password_temp[20]={"12345678"};
char g_wifi_ap_ip_temp[4]={192,168,1,1};
char g_wifi_ap_gw_temp[4]={192,168,1,1};
char g_wifi_ap_sn_temp[4]={255,255,255,0};

////////////////////////////////////////////////////////////////////////////////////////////
//wifi模块采用 STA+AP方式
//------------------------------------------------------
//wifi STA连接网络参数
char g_wifi_sta_sidd_temp[30]={"HONOR990"};
char g_wifi_sta_password_temp[20]={"123400789"};

char g_wifi_sta_ip_temp[4]={192,168,43,2};
char g_wifi_sta_gw_temp[4]={192,168,43,1};
char g_wifi_sta_sn_temp[4]={255,255,255,0};


char g_wifi_sta_dhcpIp_temp[4]={0,0,0,0};


//------------------------------------------------------
//wifi STA连接远端sever参数
//char g_wifi_sta_sever_ip_temp[4]={192,168,43,52};
char g_wifi_sta_sever_port_temp[20]={"6800"};                                           //本机wifi服务器所开辟的端口


/*定义默认IP信息*/
uint8_t g_w5500_local_ip[4]  ={192,168,0,88};											/*定义W5500默认IP地址*/
uint8_t g_w5500_subnet[4]    ={255,255,255,0};										    /*定义W5500默认子网掩码*/
uint8_t g_w5500_gateway[4]   ={192,168,0,1};											/*定义W5500默认网关*/





/*定义MAC地址,如果多块W5500网络适配板在同一现场工作，请使用不同的MAC地址*/
//uint8 mac[6]={0x00,0x08,0xdc,0x11,0x11,0x11};
uint8_t g_w5500_mac[6]={0x36,0xD8,0xdc,0xAF,0x81,0x45};
uint8_t g_w5500_dns_server[4]={114,114,114,114};									    /*定义W5500默认DNS*/
uint16_t g_w5500_local_port=5000;	                       					            //定义本地端口 此端口在运行中如果断链 则会++ 网络不好的时候随时变化



uint8_t g_wifiSendPingPongFLag=0;   //wifi 发送乒乓缓存  0  1 2 指示标志 指示当前要发送的缓存 这里是3个乒乓球  在接收消息队列之前g_wifiSendPingPongFLag  已经与msg同步
uint8_t g_w5500SendPingPongFLag=0;  //w5500 发送乒乓缓存 0  1 2 指示标志 指示当前要发送的缓存 这里是3个乒乓球  在接收消息队列之前g_w5500SendPingPongFLag 已经与msg同步

//----------------------------------------------------------------------
//最后发送的数据要大 
uint8_t g_wifiSendbuff[TCP_WIFI_PINGPONG_N][TCP_WIFI_SENDLEN+2];
uint8_t g_w5500Sendbuff[TCP_WIFI_PINGPONG_N][TCP_WIFI_SENDLEN+2];	

TIME_T g_justTimerStruct={0,15,14,1,3,4,23};
TIME_T g_wantSetTimerStruct={0,15,14,1,3,4,23};

uint8_t g_writeRX8025TimerFlag=0;//1=写一次rx8025的时间	
uint8_t g_readingRX8025TimerFlag=0;//0=当前时间可以占用 注意占用就置位 然后释放 变量形式的互斥变量

uint8_t g_w5500EnFlag=1;//1=W5500模块使能
uint8_t g_wifiEnFlag=1;//1=Wifi模块使能

uint8_t g_dhcpip_staticip_flag=0;   //1=DHCP ip     0=STATIC ip

uint8_t g_wifi_receive_data_num=0; //接收到的新数据的次数  由g_atk_mw8266d_monitor函数 负责先关处理并且清零


const uint8_t wdth_showTable0[]="----------------wdt-----------------";
const uint8_t wdth_showTable1[]="wdt h------ show wdt cmd";
const uint8_t wdth_showTable2[]="wdt en <0/1> ------Reset(0-enable,1-disable)";

const uint8_t wdth_showTable3[]="wdt en 1-->Device reset.Network disconnection immediately";

const uint8_t w5500h_showTable0[]="---------------w5500-----------------";
const uint8_t w5500h_showTable1[]="w5500 h--------------- show w5500 cmd";
const uint8_t w5500h_showTable2[]="w5500 ip xxx.xxx.xxx.xxx------ set ip";
const uint8_t w5500h_showTable3[]="w5500 sn xxx.xxx.xxx.xxx------ set sn";
const uint8_t w5500h_showTable4[]="w5500 gw xxx.xxx.xxx.xxx------ set gw";
const uint8_t w5500h_showTable5[]="w5500 info------------show w5500 info";
const uint8_t w5500h_showTable6[]="w5500 clear-----------clear parameter";
const uint8_t w5500h_showTable7[]="w5500 en-------enable w5500 and Reset";
const uint8_t w5500h_showTable8[]="w5500 clear---disable w5500 and Reset";


const uint8_t wifih_showTable0[] ="-----------------wifi-----------------";
const uint8_t wifih_showTable1[] ="wifi h------------------ show wifi cmd";
const uint8_t wifih_showTable2[] ="wifi apid xxxx---------------max len=8";
const uint8_t wifih_showTable3[] ="wifi apkey xxxx-------------len must=8";
const uint8_t wifih_showTable4[] ="wifi apip xxx xxx xxx xxx---- set apip";
const uint8_t wifih_showTable5[] ="wifi apsn xxx xxx xxx xxx---- set apsn";
const uint8_t wifih_showTable6[] ="wifi apgw xxx xxx xxx xxx---- set apgw";
const uint8_t wifih_showTable7[] ="wifi stassid xxxx---------- max len=12";
const uint8_t wifih_showTable8[] ="wifi stakey xxxx----------- max len=12";
const uint8_t wifih_showTable9[] ="wifi staip xxx xxx xxx xxx-- set staip";
const uint8_t wifih_showTable10[]="wifi staip xxx xxx xxx xxx-- set staip";
const uint8_t wifih_showTable11[]="wifi staip xxx xxx xxx xxx-- set staip";
const uint8_t wifih_showTable12[]="wifi report xxxx- set report max len=8";
const uint8_t wifih_showTable13[]="wifi dhcp en--------- enable wifi dhcp";
const uint8_t wifih_showTable14[]="wifi dhcp no-------- disable wifi dhcp";
const uint8_t wifih_showTable15[]="wifi en------------------- enable wifi";
const uint8_t wifih_showTable16[]="wifi no------------------ disable wifi";



const uint8_t clkh_showTable0[]="------------------clk------------------";
const uint8_t clkh_showTable1[]="clk h--------------------- show clk cmd";
const uint8_t clkh_showTable2[]="clk set xxxxxxx------------ set clk cmd";
const uint8_t clkh_showTable3[]="clk view---------------------- clk info";

void globalVariable(void)
{
    ;
}


