/*
**************************************************************************************************
* @file    		w5500_conf.c
* @author  		WIZnet Software Team 
* @version 		V1.0
* @date    		2015-02-14
* @brief  		配置MCU，移植W5500程序需要修改的文件，配置W5500的MAC和IP地址
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
//外部变量
extern SPI_HandleTypeDef hspi2;
extern TIM_HandleTypeDef htim6;

CONFIG_MSG  ConfigMsg;														/*配置结构体*/
EEPROM_MSG_STR EEPROM_MSG;													/*EEPROM存储信息结构体*/



/*IP配置方法选择，请自行选择*/
uint8	ip_from=IP_FROM_DEFINE;				

uint8   dhcp_ok   = 0;													   	/*dhcp成功获取IP*/
uint32	ms        = 0;														/*毫秒计数*/
uint32	dhcp_time = 0;														/*DHCP运行计数*/
vu8	    ntptimer  = 0;													    /*NPT秒计数*/

/**
*@brief		配置W5500的IP地址
*@param		无
*@return	无
*/
void set_w5500_ip(void)
{	
		
   /*复制定义的配置信息到配置结构体*/
	memcpy(ConfigMsg.mac, g_w5500_mac, 6);
	memcpy(ConfigMsg.lip,g_w5500_local_ip,4);
	memcpy(ConfigMsg.sub,g_w5500_subnet,4);
	memcpy(ConfigMsg.gw,g_w5500_gateway,4);
	memcpy(ConfigMsg.dns,g_w5500_dns_server,4);
	if(ip_from==IP_FROM_DEFINE)
    {        
//		App_Printf(" 使用定义的IP信息配置W5500\r\n");
    }
	
    //-----------------------------------------------------------------
    //cocoguojia 此处等处理flash的时候再添加代码
	/*使用EEPROM存储的IP参数*/	
	if(ip_from==IP_FROM_EEPROM)
	{
    
		/*从EEPROM中读取IP配置信息*/
		read_config_from_eeprom();		
		
		/*如果读取EEPROM中MAC信息,如果已配置，则可使用*/		
		if( *(EEPROM_MSG.mac)==0x00&& *(EEPROM_MSG.mac+1)==0x08&&*(EEPROM_MSG.mac+2)==0xdc)		
		{
			App_Printf("$$IP from EEPROM\r\n");
			/*复制EEPROM配置信息到配置的结构体变量*/
			memcpy(ConfigMsg.lip,EEPROM_MSG.lip, 4);				
			memcpy(ConfigMsg.sub,EEPROM_MSG.sub, 4);
			memcpy(ConfigMsg.gw, EEPROM_MSG.gw, 4);
		}
		else
		{
//			App_Printf(" EEPROM未配置,使用定义的IP信息配置W5500,并写入EEPROM\r\n");
			write_config_to_eeprom();	/*使用默认的IP信息，并初始化EEPROM中数据*/
		}			
	}

	/*使用DHCP获取IP参数，需调用DHCP子函数*/		
	if(ip_from==IP_FROM_DHCP)								
	{
		/*复制DHCP获取的配置信息到配置结构体*/
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
			App_Printf("$$DHCP子程序未运行,或者不成功\r\n");
			App_Printf("$$使用定义的IP信息配置W5500\r\n");
		}
	}
		
	/*以下配置信息，根据需要选用*/	
	ConfigMsg.sw_ver[0]=FW_VER_HIGH;
	ConfigMsg.sw_ver[1]=FW_VER_LOW;	

	/*将IP配置信息写入W5500相应寄存器*/	
	setSUBR(ConfigMsg.sub);
	setGAR(ConfigMsg.gw);
	setSIPR(ConfigMsg.lip);
	
	getSIPR (g_w5500_local_ip);			
	App_Printf("$$W5500 IP地址   : %d.%d.%d.%d\r\n", g_w5500_local_ip[0],g_w5500_local_ip[1],g_w5500_local_ip[2],g_w5500_local_ip[3]);
	getSUBR(g_w5500_subnet);
	App_Printf("$$W5500 子网掩码 : %d.%d.%d.%d\r\n", g_w5500_subnet[0],g_w5500_subnet[1],g_w5500_subnet[2],g_w5500_subnet[3]);
	getGAR(g_w5500_gateway);
	App_Printf("$$W5500 网关     : %d.%d.%d.%d\r\n", g_w5500_gateway[0],g_w5500_gateway[1],g_w5500_gateway[2],g_w5500_gateway[3]);
}

/**
*@brief		配置W5500的MAC地址
*@param		无
*@return	无
*/
void set_w5500_mac(void)
{
	memcpy(ConfigMsg.mac, g_w5500_mac, 6);
	setSHAR(ConfigMsg.mac);
	memcpy(DHCP_GET.mac, g_w5500_mac, 6);
}



//-----------------------------------------------------------------------------------------------------
//cocoguojia 改成HAL库代码 W5500片选信号设置函数	
//val: 为“0”表示片选端口为低，为“1”表示片选端口为高
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
*@brief		设置W5500的片选端口SCSn为低
*@param		无
*@return	无
*/
void iinchip_csoff(void)
{
	wiz_cs(LOW);
}

/**
*@brief		设置W5500的片选端口SCSn为高
*@param		无
*@return	无
*/
void iinchip_cson(void)
{	
   wiz_cs(HIGH);
    
}

//-----------------------------------------------------------------------------------------------------
//cocoguojia 改成HAL库代码 W5500复位设置函数 多加2s确保复位后充足的时间恢复
void reset_w5500(void)
{
	HAL_GPIO_WritePin(W5500_RESET_GPIO_Port, W5500_RESET_Pin, GPIO_PIN_RESET);
	osDelay(50);  
	HAL_GPIO_WritePin(W5500_RESET_GPIO_Port, W5500_RESET_Pin, GPIO_PIN_SET);
    osDelay(2000);//复位后再给2s的额外准备 
}

//-----------------------------------------------------------------------------------------------------
//cocoguojia 改成HAL库代码 SPI发送过程中接收 
//只能用hal的HAL_SPI_TransmitReceive不能用2个单发单收(发的时候其实也接就是忽略了 收的时候先发了)
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
*@brief		STM32 SPI1读写8位数据
*@param		dat：写入的8位数据
*@return	无
*/
uint8  IINCHIP_SpiSendData(uint8 dat)
{
   return(SPI_SendByte(dat));
}

/**
*@brief		写入一个8位数据到W5500
*@param		addrbsb: 写入数据的地址
*@param   data：写入的8位数据
*@return	无
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
*@brief		从W5500读出一个8位数据
*@param		addrbsb: 写入数据的地址
*@param   data：从写入的地址处读取到的8位数据
*@return	无
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
*@brief		向W5500写入len字节数据
*@param		addrbsb: 写入数据的地址
*@param   buf：写入字符串
*@param   len：字符串长度
*@return	len：返回字符串长度
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
*@brief		从W5500读出len字节数据
*@param		addrbsb: 读取数据的地址
*@param 	buf：存放读取数据
*@param		len：字符串长度
*@return	len：返回字符串长度
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
*@brief		写配置信息到EEPROM
*@param		无
*@return	无
*/
void write_config_to_eeprom(void)
{
//	uint16 dAddr=0;
//	ee_WriteBytes(ConfigMsg.mac,dAddr,(uint8)EEPROM_MSG_LEN);				
//	delay_ms(10);																							
}

/**
*@brief		从EEPROM读配置信息
*@param		无
*@return	无
*/
void read_config_from_eeprom(void)
{
//	ee_ReadBytes(EEPROM_MSG.mac,0,EEPROM_MSG_LEN);
//	delay_us(10);
}


extern TIM_HandleTypeDef htim6;
//---------------------------------------------------------
//cocoguojia 改成HAL库代码 SPI发送过程中接收 
//dhcp用到的定时器初始化
void dhcp_timer_init(void)
{
  __HAL_RCC_TIM6_CLK_ENABLE();	/* TIM6 重新开时钟，开始计时 */
//  HAL_TIM_Base_Start_IT(&htim6);    
}

//---------------------------------------------------------
//cocoguojia 改成HAL库代码 SPI发送过程中接收 
//ntp用到的定时器初始化
void ntp_timer_init(void)
{
  __HAL_RCC_TIM6_CLK_ENABLE();	/* TIM6 重新开时钟，开始计时 */	
//    HAL_TIM_Base_Start_IT(&htim6);      
}

//------------------------------------------------------------
//定时器6中断回调函数
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance==TIM6)
    {
        ms++;	
        if(ms>=1000)
        {  
            ms=0;
            dhcp_time++;    //DHCP定时加1S
            #ifndef	__NTP_H__
            ntptimer++;	    //NTP重试时间加1S
            #endif
        }
    }
}


