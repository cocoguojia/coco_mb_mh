



#include "atk_mw8266d.h"

#include "myinclude.h"

#include "atk_mw8266d_struct.h"



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//ȫ�ֱ���


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//�ڲ�ȫ����غ���
uint8_t AT_MW8266_InitSub(void);
uint8_t atk_mw8266d_monitor(void);
uint8_t atk_mw8266d_send_CIPSEND_cmd(char* cmd,char* ack,uint16_t timeout);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//�ڲ���̬����
#define AT_CIPSTATUS_N (g_at_cipstart_n+1)
char *CMDretFirst = NULL;
char *CMDret = NULL;

typedef struct
{
    
    char* cmd;                  //����Ҫ���͵�AT����
    const char* ack;            //AT���� ��ϣ���Ļظ�
    uint32_t timeout;           //AT���� һ�εȴ���ʱʱ��
    const char *okTable;        //AT���� �ɹ������ʾ
    const char *ErrTable;       //AT���� ʧ�ܺ����ʾ
    uint16_t afterDelayMs;      //�ɹ��� ������ʱ

    uint8_t special_n;          //�����õ��Ĳ������� 
    char  *special_char1;       //�����õ��Ĳ������� 
    char  *special_char2;       //�����õ��Ĳ������� 
    char  *special_char3;       //�����õ��Ĳ�������  
}AT_MW8266_strcutDef;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//�ڲ���̬����
AT_MW8266_strcutDef AT_MW8266_strcut[15];
uint8_t atk_mw8266dDoWhitOneAtCmd(AT_MW8266_strcutDef *t);
uint8_t atk_mw8266dSendAtCmd(AT_MW8266_strcutDef *t);

void AT_MW8266_strcut_init(void);

uint8_t atk_mw8266d_monitor_second(void);
uint8_t atk_mw8266dSendAtCmdStatus(AT_MW8266_strcutDef *t);





////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//ȫ�ֺ���
//----------------------------------------------------------------------------------------------------------------------------------
//wifiģ�� AT�����ʼ�� 
uint8_t g_atk_AT_MW8266_InitSub(void)
{
    uint8_t n;
    uint8_t ret=0;
    AT_MW8266_strcut_init();            //wifiģ�� AT����ṹ���ʼ����ֵ
    atinit_point:
    for(n=0;g_at_cipstart_n>=n;n++)     //����������һ��ȫ�ֱ�����ʾ AT��ʼ�����һ�������ֵ
    {
        if(0==ret)
        {
            ret+=atk_mw8266dDoWhitOneAtCmd(&AT_MW8266_strcut[n]);   //����AT���� ���Ҽ��ACK
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
//wifiģ�� ���м�� �������Ҫ��Ӧ�Ĵ���һ��
uint8_t g_atk_mw8266d_monitor(void)
{
    static uint8_t n=0;
    static uint8_t LastStatus=1;        //1=��״̬����  0=״̬������
    static uint8_t LastclinkServer=1;   //1=���ӷ���������  0=���ӷ�����������
    n++;
    uint8_t returnN=0;
    
    if(20<=n)
    {
        if(ATK_MW8266D_EOK!=atk_mw8266dSendAtCmdStatus(&AT_MW8266_strcut[AT_CIPSTATUS_N]))
        {            

            if(1==LastStatus)
            {
                App_Printf("$$STATUS is Err\r\n");
                LastStatus=0;
                LastclinkServer=0;
            }
            
             if(ATK_MW8266D_EOK==atk_mw8266dSendAtCmd(&AT_MW8266_strcut[g_at_cipstart_n]))  //����TCP������
             {
                 if(0==LastclinkServer)
                 {
                    App_Printf("$$clink SERVER CMD is OK\r\n");
                    LastclinkServer=1;
                 }
             }
             else
             {
                 if(1==LastclinkServer)
                 {
                    App_Printf("$$clink SERVER CMD is Err\r\n");
                    LastclinkServer=0;
                 }
             }
             
        }
        else
        {
            if(0==LastStatus)
            {
                App_Printf("$$STATUS is nice\r\n");
                LastStatus=1;
            }
            LastclinkServer=1;
            CMDret =CMDretFirst;
            returnN=atk_mw8266d_monitor_second();
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
//wifi����͸������ǰ Ҫ���� CIPSEND���� ˵��Ҫ���͵ĳ��� ������ǰҪ��AT����
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
//�ڲ���̬����
//---------------------------------------------------------------------
//����STATUS����ר�ú���

//--------------------------------------------------------------------------------------------
#define CMD_X_SECOND 2
#define CMD_Y_SECOND 20
const char  CMD_TABLE_SECOND[CMD_X_SECOND][CMD_Y_SECOND]={"CONNECT FAIL","Link type Error"};
uint8_t buffWifi[MB_TCP_BUF_SIZE+3];				                    /*����һ������*/

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
                //����Ƿ����ش���� ������Ҫ���³�ʼ��wifiģ��
                //"CONNECT FAIL"
                //"Link type Error"
                App_Printf("$$\r\n###WIFI Gross Mistake=%d###\r\n",n);//����
                return 1;
            }
        }  
 
        ret=strstr((const char *)CMDret,"+IPD,");
        if(NULL!=ret)
        {
            ret+=5;            
            ret1=strstr((const char *)ret,",");
            ret1++;
            
            //--------------------------------------
            //��λ����ֵ�ĵط� ȥ���ݳ���len 
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
                        App_Printf("$$the dat Error1=%c\r\n",*ret1);//����
                        lenError=1;
                    }
                }
                else
                {
                      App_Printf("$$the dat Error2=%c\r\n",*ret1);//����
                      lenError=1;
                }
            }
            else
            {
                App_Printf("$$dat Error0=%s\r\n",ret1);//����
                lenError=1;
            } 
        }
        else
        {
            lenError=1;//��Ҳ�� ����
        }                
        
        //-----------------------------------------------------------------------------
        //���Ȳ�Ϊ0 �� ��Ϊ���� �� ������MB_TCP_BUF_SIZE���ֽ�
        //�����������user cmd ������modbus data Ҳ���ܾ��Ǵ��������
        //���Ӹ� NetBridgeModbus ����ͳһ����
        if((0<len)&&(0==lenError)&&(MB_TCP_BUF_SIZE>=len))
        {
            for(i=0;i<len;i++)
            {
                buffWifi[i+3]=*retWifi;
                retWifi++;
            }
            
  
            
            buffWifi[0]=(uint8_t)(len>>8);  			//��λΪ��Ϣ��ʵ�ʳ��ȸ��ֽ�
            buffWifi[1]=(uint8_t)(len&0x00ff);   		//�ڶ�λΪ��Ϣ��ʵ�ʳ��ȵ��ֽ�
            buffWifi[2]=2;  							//����λΪ1��ʾ����  Ϊ2��ʾwifi
             

            //##############################################################################################
            //�������
            osMutexAcquire(PendingCachePointsMutexHandle, osWaitForever);   //һֱ�ȴ���ȡ������Դ 
 
            if(8>g_netBridgeModbusQueueN) 
            {
                msg=g_netBridgeModbusQueueN;
                g_netBridgeModbusQueueTable[msg]=&buffWifi[0];              //�����Ž�modbus����ָ��洢
                g_netBridgeModbusQueueN++;
            }
            else
            {
                g_netBridgeModbusQueueN=0;
                
                msg=g_netBridgeModbusQueueN;
                g_netBridgeModbusQueueTable[msg]=&buffWifi[0];              //�����Ž�modbus����ָ��洢
                g_netBridgeModbusQueueN++;
            }
            osMutexRelease(PendingCachePointsMutexHandle);                  //�ͷŻ�����Դ    
            //##############################################################################################           
            //App_Printf("$$WIFI msg=%d\r\n",msg);
            osDelay(1);
            osMessageQueuePut(netBridgeModbusQueueHandle,&msg, 0U, 100U);
            //App_Printf("$$WIFI PutQueue->netBridgeModbus msg=%d\r\n",msg);            
        }
    }
    return 0;
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
//���÷���AT����atk_mw8266dSendAtCmd���� �������Ϣ
//�ɹ�����0 ʧ�ܷ���1
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
//�������� ���ݽṹ���AT����
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
//ָ������֮ǰ��һ��Ҫ����ָ������Ϊ��
//"xxxxx" ����:"AT+RESTORE" ���̶ֹ����ַ�����keil�����ʱ�������flash������ָ���ǹ̶��ģ����Ա���
#define WIFI_WCMMODE_CMD_N 3
#define WIFI_ATE_CMD_N 0
//#define WIFI_ATE_CMD_N 1
char cmdTableStrAETX[8];
char cmdTableStrMODEX[15];
char cmdTable_CIPSTA[70];
char cmdTable_CIPAP[70];
char cmdTable_CWSAP[70];
char cmdTable_CWJAP[70];
char cmdTable_CIPSERVER[20];
char cmdTable_CIPSTART[70];


void AT_MW8266_strcut_init(void)
{
    uint8_t i=0;
   
    //------------------------------------------------------------------
    //����ָ��ָ�wifiģ�����Ĭ�ϲ���
    AT_MW8266_strcut[i].cmd="AT+RESTORE";
    AT_MW8266_strcut[i].ack="ready";
    AT_MW8266_strcut[i].timeout=3000;
    AT_MW8266_strcut[i].afterDelayMs=200;
    i++;
    
    //------------------------------------------------------------------
    //����AT��������
    AT_MW8266_strcut[i].cmd="AT";
    AT_MW8266_strcut[i].ack="OK";
    AT_MW8266_strcut[i].timeout=500;
    AT_MW8266_strcut[i].afterDelayMs=200;
    i++;
    
    //------------------------------------------------------------------
    //����wifi mode��ʽ����  
    AT_MW8266_strcut[i].ack="OK";
    AT_MW8266_strcut[i].timeout=500;
    AT_MW8266_strcut[i].afterDelayMs=200;
    switch(WIFI_WCMMODE_CMD_N)
    {
        case 1:
        strcpy(cmdTableStrMODEX,"AT+CWMODE=1");
        AT_MW8266_strcut[i].cmd=cmdTableStrMODEX;//Stationģʽ
        break;
 
        case 2:
        strcpy(cmdTableStrMODEX,"AT+CWMODE=2");
        AT_MW8266_strcut[i].cmd=cmdTableStrMODEX;//APģʽ
        break;
    
        case 3:
        strcpy(cmdTableStrMODEX,"AT+CWMODE=3");
        AT_MW8266_strcut[i].cmd=cmdTableStrMODEX;//AP+Stationģʽ
        break;
        
        default:
        strcpy(cmdTableStrMODEX,"AT+CWMODE=3");
        AT_MW8266_strcut[i].cmd=cmdTableStrMODEX;//AP+Stationģʽ
        break;
    }
    i++;
      
    
    //------------------------------------------------------------------
    //���� �����λ
    AT_MW8266_strcut[i].cmd="AT+RST";
    AT_MW8266_strcut[i].ack="OK";
    AT_MW8266_strcut[i].timeout=500;
    AT_MW8266_strcut[i].afterDelayMs=3000;
    i++;
    
    //------------------------------------------------------------------
    //���� ���� 0=��/1=�� ָ��
    AT_MW8266_strcut[i].ack="OK";
    AT_MW8266_strcut[i].timeout=500;
    AT_MW8266_strcut[i].afterDelayMs=1000;
    AT_MW8266_strcut[i].special_n=1;      
    switch(WIFI_ATE_CMD_N)
    {
        case 0:
        strcpy(cmdTableStrAETX,"ATE0");
        AT_MW8266_strcut[i].cmd=cmdTableStrAETX;//�ػ���
        break;

        case 1:
        strcpy(cmdTableStrAETX,"ATE1");
        AT_MW8266_strcut[i].cmd=cmdTableStrAETX;//������
        break;
        
        default:
        strcpy(cmdTableStrAETX,"ATE0");
        AT_MW8266_strcut[i].cmd=cmdTableStrAETX;//�ػ���
        break;
    }    
    i++;
    
    //------------------------------------------------------------------
    //���� ����sta���� ָ��
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


    
    //------------------------------------------------------------------
    //���� ����AP���� ָ��
    AT_MW8266_strcut[i].ack="OK";
    AT_MW8266_strcut[i].timeout=2000;
    AT_MW8266_strcut[i].afterDelayMs=1000;   
    AT_MW8266_strcut[i].special_char1=g_wifi_ap_ip_temp;
    AT_MW8266_strcut[i].special_char2=g_wifi_ap_gw_temp;
    AT_MW8266_strcut[i].special_char3=g_wifi_ap_sn_temp;    
    //�����������߲���:ssid,���� ͨ���� ���ܷ�ʽ
    sprintf(cmdTable_CIPAP,"AT+CIPAP=\"%d.%d.%d.%d\",\"%d.%d.%d.%d\",\"%d.%d.%d.%d\"", \
    AT_MW8266_strcut[i].special_char1[0],AT_MW8266_strcut[i].special_char1[1],AT_MW8266_strcut[i].special_char1[2],AT_MW8266_strcut[i].special_char1[3], \
    AT_MW8266_strcut[i].special_char2[0],AT_MW8266_strcut[i].special_char2[1],AT_MW8266_strcut[i].special_char2[2],AT_MW8266_strcut[i].special_char2[3], \
    AT_MW8266_strcut[i].special_char3[0],AT_MW8266_strcut[i].special_char3[1],AT_MW8266_strcut[i].special_char3[2],AT_MW8266_strcut[i].special_char3[3]);  
    AT_MW8266_strcut[i].cmd=cmdTable_CIPAP;  
    i++;
    
    //------------------------------------------------------------------
    //���� ����AP ָ��
    AT_MW8266_strcut[i].ack="OK";
    AT_MW8266_strcut[i].timeout=1000;
    AT_MW8266_strcut[i].afterDelayMs=1000;    
    AT_MW8266_strcut[i].special_char1=g_wifi_ap_sidd_temp;
    AT_MW8266_strcut[i].special_char2=g_wifi_ap_password_temp;  
    // ATK-MW8266D����WIFI �ȵ�  AP
    sprintf(cmdTable_CWSAP,"AT+CWSAP=\"%s\",\"%s\",5,3",AT_MW8266_strcut[i].special_char1,AT_MW8266_strcut[i].special_char2);  
    AT_MW8266_strcut[i].cmd=cmdTable_CWSAP;  
    i++;
    
    //------------------------------------------------------------------
    //���� ����AP ָ��
    AT_MW8266_strcut[i].ack="WIFI GOT IP";
    AT_MW8266_strcut[i].timeout=10000;
    AT_MW8266_strcut[i].afterDelayMs=5000;   
    AT_MW8266_strcut[i].special_char1=g_wifi_sta_sidd_temp;
    AT_MW8266_strcut[i].special_char2=g_wifi_sta_password_temp;  
    //�����������߲���:ssid,���� ͨ���� ���ܷ�ʽ
    sprintf(cmdTable_CWJAP,"AT+CWJAP=\"%s\",\"%s\"",AT_MW8266_strcut[i].special_char1,AT_MW8266_strcut[i].special_char2);  
    AT_MW8266_strcut[i].cmd=cmdTable_CWJAP;  
    i++;
    
    //------------------------------------------------------------------
    //����  ATK-MW8266����˫���� ָ��
    AT_MW8266_strcut[i].cmd="AT+CIPMUX=1";
    AT_MW8266_strcut[i].ack="OK";
    AT_MW8266_strcut[i].timeout=10000;
    AT_MW8266_strcut[i].afterDelayMs=500;
    i++;
    
    //------------------------------------------------------------------
    //����  �����������Ķ˿� ָ��
    AT_MW8266_strcut[i].ack="OK";
    AT_MW8266_strcut[i].timeout=200;
    AT_MW8266_strcut[i].afterDelayMs=500;
    AT_MW8266_strcut[i].special_char1=g_wifi_sta_sever_port_temp;
    sprintf(cmdTable_CIPSERVER,"AT+CIPSERVER=1,%s",AT_MW8266_strcut[i].special_char1);
    AT_MW8266_strcut[i].cmd=cmdTable_CIPSERVER; 
        
    i++;
    
    //------------------------------------------------------------------
    //����  ���÷�������ʱʱ�䣬��ʱ��Ͽ��ͻ��˵����� ָ�� ������10s
    AT_MW8266_strcut[i].cmd="AT+CIPSTO=10";
    AT_MW8266_strcut[i].ack="OK";
    AT_MW8266_strcut[i].timeout=200;
    AT_MW8266_strcut[i].afterDelayMs=500;
    
    g_at_cipstart_n=i; 
    App_Printf("$$ the g_at_cipstart_n=%d\r\n",g_at_cipstart_n);  
    i++;
    
//    //------------------------------------------------------------------
//    //���� ���ӷ����� ָ��
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
    //����  ��ѯ״̬ ָ��
    AT_MW8266_strcut[i].cmd="AT+CIPSTATUS";
    AT_MW8266_strcut[i].ack="+CIPSTATUS:";
    AT_MW8266_strcut[i].timeout=500;
    AT_MW8266_strcut[i].afterDelayMs=500;
    i++;
       
    atk_mw8266d_uart_rx_restart();    
}
