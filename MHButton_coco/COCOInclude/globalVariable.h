
#ifndef _GLOBALBARIABLE_H__
#define _GLOBALBARIABLE_H__

#include "myinclude.h"

#define FIRST_PARAAMETER 0X56

//-----------------------------------------------------------------------------------------------------------------
//���ʹ��﮵���жϹ��� ��Ԥ����POWER_BATTERY_MANAGEMENT  ���� ����POWER_BATTERY_MANAGEMENT��Ԥ����
//#define  POWER_BATTERY_MANAGEMENT 1
#define  POWER_BATTERY_EN   HAL_GPIO_WritePin(GPIOB, POWER_CONTROL_Pin, GPIO_PIN_SET)     
#define  POWER_BATTERY_NO   HAL_GPIO_WritePin(GPIOB, POWER_CONTROL_Pin, GPIO_PIN_RESET)  

//��Щmodbus�Ǵӵ�ַ0��ʼ�ģ�����������������������,cocoguojia say
//�����ַ�Ǵ�0��ʼ�� ��ô��ʹ���������;�����ַ�Ǵ�1��ʼ�� ��ô������������䣬����˫б������-->//#define MODBUS_ADDR_0    1
#define MODBUS_ADDR_0    1



#define TCP_WIFI_SENDLEN    120
#define TCP_WIFI_PINGPONG_N    3

#define MB_TCP_BUF_SIZE     ( 100 + 7 )   /* Must hold a complete Modbus TCP frame. */




extern void globalVariable(void);

//------------------------------------------------------------------
//ȫ�ֱ���
extern uint16_t g_keyStateTalbe[6];  //5����ť״̬ 1=������ 0=̧��
extern uint16_t g_keyTimeTalbe[6];   //�����µ��ۼ�ʱ��
extern uint16_t g_diNumTalbe[4];     //3·�����������µĴ���

extern uint8_t g_tcpOrwifiFlag;                  //��Ϣ�Ĳ����߱�־ 1=���� 2=wifi ����=����


extern uint8_t g_openFreeModbusFlag;                 //1=����FreeModbus����    


//---------------------------------------------------------------------------------
extern uint8_t *g_netBridgeModbusQueueTable[8];         //�����Ž�modbus����ָ��洢
extern uint8_t g_netBridgeModbusQueueN;                 //�����Ž�modbus����ָ�� ��ǰ���


extern uint8_t g_holdingChangeFlag;                     //holding �Ĵ����İ��־ 1=�ı��    

extern uint8_t g_at_cipstart_n;//at+cipstart ������������


extern uint8_t g_dhcpip_staticip_flag;//1=DHCP ip 0=STATIC ip



////////////////////////////////////////////////////////////////////////////////////////////
//wifiģ����� STA+AP��ʽ
//------------------------------------------------------
//wifi STA�����������
extern char g_wifi_sta_sidd_temp[30];
extern char g_wifi_sta_password_temp[20];

extern char g_wifi_sta_ip_temp[4];
extern char g_wifi_sta_gw_temp[4];
extern char g_wifi_sta_sn_temp[4];

extern char g_wifi_sta_dhcpIp_temp[4];


//------------------------------------------------------
//wifi AP����������ȵ����
extern char g_wifi_ap_sidd_temp[20];
extern char g_wifi_ap_password_temp[20];
extern char g_wifi_ap_ip_temp[4];
extern char g_wifi_ap_gw_temp[4];
extern char g_wifi_ap_sn_temp[4];


//------------------------------------------------------
//wifi STA����Զ��sever����
//extern char g_wifi_sta_sever_ip_temp[4];
extern char g_wifi_sta_sever_port_temp[20];

/*����MAC��ַ,������W5500�����������ͬһ�ֳ���������ʹ�ò�ͬ��MAC��ַ*/
//uint8 mac[6]={0x00,0x08,0xdc,0x11,0x11,0x11};
extern uint8_t g_w5500_mac[6];

/*����Ĭ��IP��Ϣ*/
extern uint8_t g_w5500_local_ip[4];											/*����W5500Ĭ��IP��ַ*/
extern uint8_t g_w5500_subnet[4];										/*����W5500Ĭ����������*/
extern uint8_t g_w5500_gateway[4];											/*����W5500Ĭ������*/
extern uint8_t g_w5500_dns_server[4];									    /*����W5500Ĭ��DNS*/

extern uint16_t g_w5500_local_port;	                       					            /*���屾�ض˿�*/

///*����Զ��IP��Ϣ*/ //���� Զ��sever���� д�ڳ����ﲻ��������
//extern uint8_t  g_w5500_remote_ip[4];											/*Զ��IP��ַ*/
//extern uint16_t g_w5500_remote_port;													/*Զ�˶˿ں�*/

extern uint8_t g_wifiSendPingPongFLag;   //wifi ����ƹ�һ���  0  1 2 ָʾ��־ ָʾ��ǰҪ���͵Ļ��� ������3��ƹ����
extern uint8_t g_w5500SendPingPongFLag;  //w5500 ����ƹ�һ��� 0  1 2 ָʾ��־ ָʾ��ǰҪ���͵Ļ��� ������3��ƹ����

extern uint8_t g_wifiSendbuff[TCP_WIFI_PINGPONG_N][TCP_WIFI_SENDLEN+2];
extern uint8_t g_w5500Sendbuff[TCP_WIFI_PINGPONG_N][TCP_WIFI_SENDLEN+2];	


extern uint8_t g_writeRX8025TimerFlag;//1=дһ��rx8025��ʱ��	
extern uint8_t g_readingRX8025TimerFlag;//0=��ǰʱ�����ռ�� ע��ռ�þ���λ Ȼ���ͷ� ������ʽ�Ļ������

extern uint8_t g_w5500EnFlag;   //1=W5500ģ��ʹ��
extern uint8_t g_wifiEnFlag;     //1=Wifiģ��ʹ��

extern uint8_t g_wifi_receive_data_num; //���յ��������ݵĴ���  ��g_atk_mw8266d_monitor���� �����ȹش���������


//-------------------------------------------------------------------
//net�����Ž�Modbus�� ��Ϣ���о��
extern osMessageQueueId_t netBridgeModbusQueueHandle;
//-------------------------------------------------------------------
//wifiҪ���͵����ݶ��� ��Ϣ���о��
extern osMessageQueueId_t wifiSendQueueHandle;
//-------------------------------------------------------------------
//w5500Ҫ���͵����ݶ��� ��Ϣ���о��
extern osMessageQueueId_t w5500SendQueueHandle;
//-----------------------------------------------------------------------------------------------------------------------------------
//���ڻ���wifi��������ָ�봦���������� ���Ǹ�NetBridgeModbus������Ϣ����֮ǰ ��������
extern osMutexId_t PendingCachePointsMutexHandle;
//-----------------------------------------------------------------------
//��д ���ּĴ���Ȩ�� �����ź���������� ע�Ᵽ�ּĴ����Ĳ�����Ҫ������޶�
extern osMutexId_t ReadWriteRegHoldingMutexHandle;
//-----------------------------------------------------------------------
//��д W25q64�Ļ����ź������
extern osMutexId_t W25q64MutexHandle;


//��ַ=0 ��1���� ����5����ť��3·�����������Ϣ

//��ַ=4096  ��2���� w5500 ip�����Ϣ
//��ַ=4096*2 ��3���� w5500 sn�����Ϣ
//��ַ=4096*3 ��4���� w5500 gw�����Ϣ
//��ַ=4096*4 ��5���� w5500 en/no�����Ϣ

//��ַ=4096*10 ��11���� wifi apid�����Ϣ
//��ַ=4096*11 ��12���� wifi apkey �����Ϣ
//��ַ=4096*12 ��13���� wifi apip�����Ϣ
//��ַ=4096*13 ��14���� wifi apsn�����Ϣ
//��ַ=4096*14 ��15���� wifi apgw �����Ϣ

//��ַ=4096*15 ��16���� wifi stassid�����Ϣ
//��ַ=4096*16 ��17���� wifi stakey �����Ϣ
//��ַ=4096*17 ��18���� wifi staip�����Ϣ
//��ַ=4096*18 ��19���� wifi stasn�����Ϣ
//��ַ=4096*19 ��20���� wifi stagw �����Ϣ

//��ַ=4096*20 ��21���� wifi reip�����Ϣ Զ��IP
//��ַ=4096*21 ��22���� wifi report�����Ϣ Զ�˶˿�

//��ַ=4096*22 ��23���� wifi en/no�����Ϣ
//��ַ=4096*23 ��24���� wifi dhcp en/no�����Ϣ


//��ַ=4096*29 ��30���� �������ù����� ����Ĭ�ϲ���




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
//�����ù����� ����Ĭ�ϲ���
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

