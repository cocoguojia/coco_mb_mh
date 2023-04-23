/*
**************************************************************************************************
* @file    		w5500_conf.c
* @author  		WIZnet Software Team 
* @version 		V1.0
* @date    		2015-02-14
* @brief  		����MCU����ֲW5500������Ҫ�޸ĵ��ļ�������W5500��MAC��IP��ַ
**************************************************************************************************
*/
#include <stdio.h> 
#include <string.h>

#include "w5500_conf.h"
//#include "bsp_i2c_ee.h"
#include "utility.h"
#include "w5500.h"
#include "dhcp.h"
//#include "bsp_TiMbase.h"


//--------------------------------------------------
//�ⲿ����
extern SPI_HandleTypeDef hspi2;
extern TIM_HandleTypeDef htim6;

CONFIG_MSG  ConfigMsg;														/*���ýṹ��*/
EEPROM_MSG_STR EEPROM_MSG;													/*EEPROM�洢��Ϣ�ṹ��*/



/*IP���÷���ѡ��������ѡ��*/
uint8	ip_from=IP_FROM_DEFINE;				

uint8   dhcp_ok   = 0;													   	/*dhcp�ɹ���ȡIP*/
uint32	ms        = 0;														/*�������*/
uint32	dhcp_time = 0;														/*DHCP���м���*/
vu8	    ntptimer  = 0;													    /*NPT�����*/

/**
*@brief		����W5500��IP��ַ
*@param		��
*@return	��
*/
void set_w5500_ip(void)
{	
		
   /*���ƶ����������Ϣ�����ýṹ��*/
	memcpy(ConfigMsg.mac, g_w5500_mac, 6);
	memcpy(ConfigMsg.lip,g_w5500_local_ip,4);
	memcpy(ConfigMsg.sub,g_w5500_subnet,4);
	memcpy(ConfigMsg.gw,g_w5500_gateway,4);
	memcpy(ConfigMsg.dns,g_w5500_dns_server,4);
	if(ip_from==IP_FROM_DEFINE)
    {        
//		App_Printf(" ʹ�ö����IP��Ϣ����W5500\r\n");
    }
	
    //-----------------------------------------------------------------
    //cocoguojia �˴��ȴ���flash��ʱ������Ӵ���
	/*ʹ��EEPROM�洢��IP����*/	
	if(ip_from==IP_FROM_EEPROM)
	{
    
		/*��EEPROM�ж�ȡIP������Ϣ*/
		read_config_from_eeprom();		
		
		/*�����ȡEEPROM��MAC��Ϣ,��������ã����ʹ��*/		
		if( *(EEPROM_MSG.mac)==0x00&& *(EEPROM_MSG.mac+1)==0x08&&*(EEPROM_MSG.mac+2)==0xdc)		
		{
			App_Printf("$$IP from EEPROM\r\n");
			/*����EEPROM������Ϣ�����õĽṹ�����*/
			memcpy(ConfigMsg.lip,EEPROM_MSG.lip, 4);				
			memcpy(ConfigMsg.sub,EEPROM_MSG.sub, 4);
			memcpy(ConfigMsg.gw, EEPROM_MSG.gw, 4);
		}
		else
		{
//			App_Printf(" EEPROMδ����,ʹ�ö����IP��Ϣ����W5500,��д��EEPROM\r\n");
			write_config_to_eeprom();	/*ʹ��Ĭ�ϵ�IP��Ϣ������ʼ��EEPROM������*/
		}			
	}

	/*ʹ��DHCP��ȡIP�����������DHCP�Ӻ���*/		
	if(ip_from==IP_FROM_DHCP)								
	{
		/*����DHCP��ȡ��������Ϣ�����ýṹ��*/
		if(dhcp_ok==1)
		{
			App_Printf("$$IP from DHCP\r\n");		 
			memcpy(ConfigMsg.lip,DHCP_GET.lip, 4);
			memcpy(ConfigMsg.sub,DHCP_GET.sub, 4);
			memcpy(ConfigMsg.gw,DHCP_GET.gw, 4);
			memcpy(ConfigMsg.dns,DHCP_GET.dns,4);
		}
		else
		{
			App_Printf("$$DHCP�ӳ���δ����,���߲��ɹ�\r\n");
			App_Printf("$$ʹ�ö����IP��Ϣ����W5500\r\n");
		}
	}
		
	/*����������Ϣ��������Ҫѡ��*/	
	ConfigMsg.sw_ver[0]=FW_VER_HIGH;
	ConfigMsg.sw_ver[1]=FW_VER_LOW;	

	/*��IP������Ϣд��W5500��Ӧ�Ĵ���*/	
	setSUBR(ConfigMsg.sub);
	setGAR(ConfigMsg.gw);
	setSIPR(ConfigMsg.lip);
	
	getSIPR (g_w5500_local_ip);			
	App_Printf("$$W5500 IP��ַ   : %d.%d.%d.%d\r\n", g_w5500_local_ip[0],g_w5500_local_ip[1],g_w5500_local_ip[2],g_w5500_local_ip[3]);
	getSUBR(g_w5500_subnet);
	App_Printf("$$W5500 �������� : %d.%d.%d.%d\r\n", g_w5500_subnet[0],g_w5500_subnet[1],g_w5500_subnet[2],g_w5500_subnet[3]);
	getGAR(g_w5500_gateway);
	App_Printf("$$W5500 ����     : %d.%d.%d.%d\r\n", g_w5500_gateway[0],g_w5500_gateway[1],g_w5500_gateway[2],g_w5500_gateway[3]);
}

/**
*@brief		����W5500��MAC��ַ
*@param		��
*@return	��
*/
void set_w5500_mac(void)
{
	memcpy(ConfigMsg.mac, g_w5500_mac, 6);
	setSHAR(ConfigMsg.mac);
	memcpy(DHCP_GET.mac, g_w5500_mac, 6);
}



//-----------------------------------------------------------------------------------------------------
//cocoguojia �ĳ�HAL����� W5500Ƭѡ�ź����ú���	
//val: Ϊ��0����ʾƬѡ�˿�Ϊ�ͣ�Ϊ��1����ʾƬѡ�˿�Ϊ��
void wiz_cs(uint8_t val)
{
	if (val == LOW) 
	{
	    HAL_GPIO_WritePin(W5500_NSS_GPIO_Port, W5500_NSS_Pin, GPIO_PIN_RESET);
	}
	else if (val == HIGH)
	{
	   HAL_GPIO_WritePin(W5500_NSS_GPIO_Port, W5500_NSS_Pin, GPIO_PIN_SET);
	}
}

/**
*@brief		����W5500��Ƭѡ�˿�SCSnΪ��
*@param		��
*@return	��
*/
void iinchip_csoff(void)
{
	wiz_cs(LOW);
}

/**
*@brief		����W5500��Ƭѡ�˿�SCSnΪ��
*@param		��
*@return	��
*/
void iinchip_cson(void)
{	
   wiz_cs(HIGH);
    
}

//-----------------------------------------------------------------------------------------------------
//cocoguojia �ĳ�HAL����� W5500��λ���ú��� ���2sȷ����λ������ʱ��ָ�
void reset_w5500(void)
{
	HAL_GPIO_WritePin(W5500_RESET_GPIO_Port, W5500_RESET_Pin, GPIO_PIN_RESET);
	osDelay(50);  
	HAL_GPIO_WritePin(W5500_RESET_GPIO_Port, W5500_RESET_Pin, GPIO_PIN_SET);
    osDelay(2000);//��λ���ٸ�2s�Ķ���׼�� 
}

//-----------------------------------------------------------------------------------------------------
//cocoguojia �ĳ�HAL����� SPI���͹����н��� 
//ֻ����hal��HAL_SPI_TransmitReceive������2����������(����ʱ����ʵҲ�Ӿ��Ǻ����� �յ�ʱ���ȷ���)
uint8_t SPI_SendByte(uint8_t byte)
{
    uint8_t receDat;

    
    //------------------------------------------------------------------------------
    //cocoguojia send
    HAL_SPI_TransmitReceive(&hspi2,&byte, &receDat,1, 10);
    
//     returnDat=HAL_SPI_TransmitReceive(&hspi2,&byte, &receDat,1, 10); 
    
//    do
//    {
//        returnDat=HAL_SPI_TransmitReceive(&hspi2,&byte, &receDat,1, 10); 
//        if(HAL_OK!=returnDat)
//        {
//            osDelay(10);
//        }        
//    }
//    while(HAL_OK!=returnDat);
    return receDat;
}

/**
*@brief		STM32 SPI1��д8λ����
*@param		dat��д���8λ����
*@return	��
*/
uint8  IINCHIP_SpiSendData(uint8 dat)
{
   return(SPI_SendByte(dat));
}

/**
*@brief		д��һ��8λ���ݵ�W5500
*@param		addrbsb: д�����ݵĵ�ַ
*@param   data��д���8λ����
*@return	��
*/
void IINCHIP_WRITE( uint32 addrbsb,  uint8 data)
{
   iinchip_csoff();                              		
   IINCHIP_SpiSendData( (addrbsb & 0x00FF0000)>>16);	
   IINCHIP_SpiSendData( (addrbsb & 0x0000FF00)>> 8);
   IINCHIP_SpiSendData( (addrbsb & 0x000000F8) + 4);  
   IINCHIP_SpiSendData(data);                   
   iinchip_cson();                            
}

/**
*@brief		��W5500����һ��8λ����
*@param		addrbsb: д�����ݵĵ�ַ
*@param   data����д��ĵ�ַ����ȡ����8λ����
*@return	��
*/
uint8 IINCHIP_READ(uint32 addrbsb)
{
   uint8 data = 0;
   iinchip_csoff();                            
   IINCHIP_SpiSendData( (addrbsb & 0x00FF0000)>>16);
   IINCHIP_SpiSendData( (addrbsb & 0x0000FF00)>> 8);
   IINCHIP_SpiSendData( (addrbsb & 0x000000F8))    ;
   data = IINCHIP_SpiSendData(0x00);            
   iinchip_cson();                               
   return data;    
}

/**
*@brief		��W5500д��len�ֽ�����
*@param		addrbsb: д�����ݵĵ�ַ
*@param   buf��д���ַ���
*@param   len���ַ�������
*@return	len�������ַ�������
*/
uint16 wiz_write_buf(uint32 addrbsb,uint8* buf,uint16 len)
{
   uint16 idx = 0;
   if(len == 0) App_Printf("Unexpected2 length 0\r\n");
   iinchip_csoff();                               
   IINCHIP_SpiSendData( (addrbsb & 0x00FF0000)>>16);
   IINCHIP_SpiSendData( (addrbsb & 0x0000FF00)>> 8);
   IINCHIP_SpiSendData( (addrbsb & 0x000000F8) + 4); 
   for(idx = 0; idx < len; idx++)
   {
     IINCHIP_SpiSendData(buf[idx]);
   }
   iinchip_cson();                           
   return len;  
}

/**
*@brief		��W5500����len�ֽ�����
*@param		addrbsb: ��ȡ���ݵĵ�ַ
*@param 	buf����Ŷ�ȡ����
*@param		len���ַ�������
*@return	len�������ַ�������
*/
uint16 wiz_read_buf(uint32 addrbsb, uint8* buf,uint16 len)
{
  uint16 idx = 0;
  if(len == 0)
  {
    App_Printf("Unexpected2 length 0\r\n");
  }
  iinchip_csoff();                                
  IINCHIP_SpiSendData( (addrbsb & 0x00FF0000)>>16);
  IINCHIP_SpiSendData( (addrbsb & 0x0000FF00)>> 8);
  IINCHIP_SpiSendData( (addrbsb & 0x000000F8));    
  for(idx = 0; idx < len; idx++)                   
  {
    buf[idx] = IINCHIP_SpiSendData(0x00);
  }
  iinchip_cson();                                  
  return len;
}

/**
*@brief		д������Ϣ��EEPROM
*@param		��
*@return	��
*/
void write_config_to_eeprom(void)
{
//	uint16 dAddr=0;
//	ee_WriteBytes(ConfigMsg.mac,dAddr,(uint8)EEPROM_MSG_LEN);				
//	delay_ms(10);																							
}

/**
*@brief		��EEPROM��������Ϣ
*@param		��
*@return	��
*/
void read_config_from_eeprom(void)
{
//	ee_ReadBytes(EEPROM_MSG.mac,0,EEPROM_MSG_LEN);
//	delay_us(10);
}


extern TIM_HandleTypeDef htim6;
//---------------------------------------------------------
//cocoguojia �ĳ�HAL����� SPI���͹����н��� 
//dhcp�õ��Ķ�ʱ����ʼ��
void dhcp_timer_init(void)
{
  __HAL_RCC_TIM6_CLK_ENABLE();	/* TIM6 ���¿�ʱ�ӣ���ʼ��ʱ */
//  HAL_TIM_Base_Start_IT(&htim6);    
}

//---------------------------------------------------------
//cocoguojia �ĳ�HAL����� SPI���͹����н��� 
//ntp�õ��Ķ�ʱ����ʼ��
void ntp_timer_init(void)
{
  __HAL_RCC_TIM6_CLK_ENABLE();	/* TIM6 ���¿�ʱ�ӣ���ʼ��ʱ */	
//    HAL_TIM_Base_Start_IT(&htim6);      
}

//------------------------------------------------------------
//��ʱ��6�жϻص�����
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance==TIM6)
    {
        ms++;	
        if(ms>=1000)
        {  
            ms=0;
            dhcp_time++;    //DHCP��ʱ��1S
            #ifndef	__NTP_H__
            ntptimer++;	    //NTP����ʱ���1S
            #endif
        }
    }
}


