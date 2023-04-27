
#include "globalVariable.h"
#include "BL8025.h"

//------------------------------------------------------------------
//ȫ�ֱ���

////#######################################################################################################################
////W5500 ���� ����Զ��IP��Ϣ  
//uint8_t  g_w5500_remote_ip[4]={192,168,0,51};											/*Զ��IP��ַ*/
//uint16_t g_w5500_remote_port=6800;													    /*Զ�˶˿ں�*/
////#######################################################################################################################


uint16_t g_keyStateTalbe[6]={0,0,0,0,0,0};          //5����ť״̬ 1=������ 0=̧��
uint16_t g_keyTimeTalbe[6]={0,0,0,0,0,0};           //5����ť�����µ��ۼ�ʱ��

uint16_t g_diNumTalbe[4]={0,0,0,0};                 //3·�����������µĴ���

uint8_t g_tcpOrwifiFlag=0;                          //��Ϣ�Ĳ����߱�־ 1=���� 2=wifi ����=����

uint8_t g_openFreeModbusFlag=0;                     //1=����FreeModbus����   

uint8_t g_holdingChangeFlag=0;                     //holding �Ĵ����İ��־ 1=�ı��    




uint8_t g_at_cipstart_n=0;//at+cipstart ������������

//---------------------------------------------------------------------------------------------------------------
//CMSIS OS2��װ����ϵͳ�Ŀ� ��Ϣ�������ֻ�ܴ���һ��16λ��ֵ ָ�뻹��32λ ����16λ���Բ�
uint8_t *g_netBridgeModbusQueueTable[8]={0,0,0,0,0,0,0,0};              //�����Ž�modbus����ָ��洢
uint8_t g_netBridgeModbusQueueN=0;                                      //�����Ž�modbus����ָ�� ��ǰ���





//------------------------------------------------------
//wifi AP����������ȵ����
char g_wifi_ap_sidd_temp[20]={"MHAUTO_6"};
char g_wifi_ap_password_temp[20]={"12345678"};
char g_wifi_ap_ip_temp[4]={192,168,1,1};
char g_wifi_ap_gw_temp[4]={192,168,1,1};
char g_wifi_ap_sn_temp[4]={255,255,255,0};

////////////////////////////////////////////////////////////////////////////////////////////
//wifiģ����� STA+AP��ʽ
//------------------------------------------------------
//wifi STA�����������
char g_wifi_sta_sidd_temp[30]={"HONOR990"};
char g_wifi_sta_password_temp[20]={"123400789"};

char g_wifi_sta_ip_temp[4]={192,168,43,2};
char g_wifi_sta_gw_temp[4]={192,168,43,1};
char g_wifi_sta_sn_temp[4]={255,255,255,0};


char g_wifi_sta_dhcpIp_temp[4]={0,0,0,0};


//------------------------------------------------------
//wifi STA����Զ��sever����
//char g_wifi_sta_sever_ip_temp[4]={192,168,43,52};
char g_wifi_sta_sever_port_temp[20]={"6800"};                                           //����wifi�����������ٵĶ˿�


/*����Ĭ��IP��Ϣ*/
uint8_t g_w5500_local_ip[4]  ={192,168,0,88};											/*����W5500Ĭ��IP��ַ*/
uint8_t g_w5500_subnet[4]    ={255,255,255,0};										    /*����W5500Ĭ����������*/
uint8_t g_w5500_gateway[4]   ={192,168,0,1};											/*����W5500Ĭ������*/





/*����MAC��ַ,������W5500�����������ͬһ�ֳ���������ʹ�ò�ͬ��MAC��ַ*/
//uint8 mac[6]={0x00,0x08,0xdc,0x11,0x11,0x11};
uint8_t g_w5500_mac[6]={0x36,0xD8,0xdc,0xAF,0x81,0x45};
uint8_t g_w5500_dns_server[4]={114,114,114,114};									    /*����W5500Ĭ��DNS*/
uint16_t g_w5500_local_port=5000;	                       					            //���屾�ض˿� �˶˿���������������� ���++ ���粻�õ�ʱ����ʱ�仯



uint8_t g_wifiSendPingPongFLag=0;   //wifi ����ƹ�һ���  0  1 2 ָʾ��־ ָʾ��ǰҪ���͵Ļ��� ������3��ƹ����  �ڽ�����Ϣ����֮ǰg_wifiSendPingPongFLag  �Ѿ���msgͬ��
uint8_t g_w5500SendPingPongFLag=0;  //w5500 ����ƹ�һ��� 0  1 2 ָʾ��־ ָʾ��ǰҪ���͵Ļ��� ������3��ƹ����  �ڽ�����Ϣ����֮ǰg_w5500SendPingPongFLag �Ѿ���msgͬ��

//----------------------------------------------------------------------
//����͵�����Ҫ�� 
uint8_t g_wifiSendbuff[TCP_WIFI_PINGPONG_N][TCP_WIFI_SENDLEN+2];
uint8_t g_w5500Sendbuff[TCP_WIFI_PINGPONG_N][TCP_WIFI_SENDLEN+2];	

TIME_T g_justTimerStruct={0,15,14,1,3,4,23};
TIME_T g_wantSetTimerStruct={0,15,14,1,3,4,23};

uint8_t g_writeRX8025TimerFlag=0;//1=дһ��rx8025��ʱ��	
uint8_t g_readingRX8025TimerFlag=0;//0=��ǰʱ�����ռ�� ע��ռ�þ���λ Ȼ���ͷ� ������ʽ�Ļ������

uint8_t g_w5500EnFlag=1;//1=W5500ģ��ʹ��
uint8_t g_wifiEnFlag=1;//1=Wifiģ��ʹ��

uint8_t g_dhcpip_staticip_flag=0;   //1=DHCP ip     0=STATIC ip

uint8_t g_wifi_receive_data_num=0; //���յ��������ݵĴ���  ��g_atk_mw8266d_monitor���� �����ȹش���������


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


