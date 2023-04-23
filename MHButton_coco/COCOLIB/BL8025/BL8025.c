

#include "BL8025.h"




/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//RX8025T实时时钟驱动程序(硬件IIC)
//BL8025T 的从地址为 7bit 固定的数据（0110 010）在通信时，从地址是附加上 R/W 以 8bit 数据发送的。
//0110 0100为写模式，0110 0101为读模式,对应16进制为：0x64、0x65。
//BL8025T 有地址自动增加功能。指定的从地址一旦开始，之后只有数据字节被发送。每个字节后，BL8025T 的地址自动增加。
		
#define  RX8025T_DEVICE_ADDRESS  0x64



extern I2C_HandleTypeDef hi2c1;

HAL_StatusTypeDef WriteI2cData(uint8_t addr, uint8_t reg, uint8_t *pBuffer, uint8_t len);
uint8_t ReadI2cData(uint8_t addr, uint8_t reg, uint8_t *buf,uint8_t len);

/*
*********************************************************************************************************
*	函 数 名: bsp_InitRtc
*	功能说明: 初始RTC. 该函数被 bsp_Init() 调用。
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
uint8_t bsp_InitRtc(void)
{
    HAL_StatusTypeDef ret;
    uint8_t returnN=0;

    //固定周期时钟源为分钟更新
	uint8_t val[3]={0x03,0x00,0x40};  //0x0D、0x0E、0x0F、三个寄存器的值，设置时间更新为“秒”更新，关闭所有闹钟，温补时间为2秒，打开时间更新中断，关闭其他中断。
		   
	
	ret = WriteI2cData(RX8025T_DEVICE_ADDRESS, RX8025T_EXT_REG,val,3);
    
    ret+=I2cIsDeviceReady(0x64,3);//检测器件是否完好 测试3次
    
    if(HAL_OK==ret)
    {
        App_Printf("$$rx8025 is OK\r\n");
        returnN=0;
    }
    else
    {
        App_Printf("$$rx8025 is Error\r\n");
        returnN=1;
    }
    return returnN;
}



/*******************************************************************************
* 函数名: bsp_SetRTCAlarm
* 描述  : 设置RX8025T的闹钟
* 参数  : 存储时间的结构体
* 返回值: 0成功，其它失败。
*******************************************************************************/
uint8_t bsp_SetRTCAlarm(uint16_t alarm)
{
	 	uint8_t  status;
		uint8_t  value = 0;
		uint8_t  reg_cy1[2];//报警固定周期寄存器
		uint8_t  reg[3];
//	uint8_t  buf[5];
/*
		固定周期定时器配置

		先将TIE置“0”，以避免在配置固定周期中断的同时发生意外的硬件中断。
		（1） 设定TSEL1,0两位选择倒计时周期。
		（2） 设定B,C寄存器，从而设置减法计数器的初值，然后初始化TF标志为“0”。
		（3） 设置TIE,TE位为“1”

*/	
			value = 0x40;
			status = WriteI2cData(RX8025T_DEVICE_ADDRESS, RX8025T_CONT_REG, &value, 1);
	
			value = 0x00;
			status = WriteI2cData(RX8025T_DEVICE_ADDRESS, RX8025T_EXT_REG, &value, 1);			
			
			reg_cy1[0] = alarm & 0xFF;
			reg_cy1[1] = (alarm >> 8) & 0xFF;			
			status = WriteI2cData(RX8025T_DEVICE_ADDRESS, RX8025T_CYl_REG, reg_cy1, 2);		
			
			reg[0] = 0x13;		//分钟更新
			reg[1] = 0x10;
			reg[2] = 0x50;	
			status = WriteI2cData(RX8025T_DEVICE_ADDRESS, RX8025T_EXT_REG, reg, 3);		
	
			return status;
  
}

/*******************************************************************************
* 函数名: bsp_GetRtcTime
* 描述  : 从RX8025T获取时间 
* 参数  : 存储时间的结构体
* 返回值: 0成功，1失败。
*******************************************************************************/
uint8_t bsp_GetRtcTime(TIME_T *t)
{
	uint8_t rtc_str[7];
  
	if(ReadI2cData(RX8025T_DEVICE_ADDRESS, RX8025T_SEC_REG, rtc_str,7) != 0)  //获取日期与时间
		return 1;  //读取出错

	t->second = ((rtc_str[0]>>4)*10) + (rtc_str[0] & 0x0f);
	t->minute = ((rtc_str[1]>>4)*10) + (rtc_str[1] & 0x0f);
	t->hour   = ((rtc_str[2]>>4)*10) + (rtc_str[2] & 0x0f);
	t->week		= rtc_str[3];	
	t->day    = ((rtc_str[4]>>4)*10) + (rtc_str[4] & 0x0f);
	t->month  = ((rtc_str[5]>>4)*10) + (rtc_str[5] & 0x0f);
    t->year   = ((rtc_str[6]>>4)*10) + (rtc_str[6] & 0x0f);
	
	return 0;
}

/*******************************************************************************
* 函数名: bsp_SetRtcTime
* 描述  : 设置RX8025T时间 
* 参数  : 存储时间的结构体
* 返回值: 0成功，1失败。
*******************************************************************************/
uint8_t bsp_SetRtcTime(TIME_T *t)
{
		uint8_t  status;
		uint8_t rtc_str[7];
	
		rtc_str[0] = ((t->second/10)<<4) | (t->second%10);
		rtc_str[1] = ((t->minute/10)<<4) | (t->minute%10);
		rtc_str[2] = ((t->hour/10)<<4) | (t->hour%10);
		rtc_str[3] = t->week;
		rtc_str[4] = ((t->day/10)<<4) | (t->day%10);
		rtc_str[5] = ((t->month/10)<<4) | (t->month%10);
		rtc_str[6] = ((t->year/10)<<4) | (t->year%10);

		
		status = WriteI2cData(RX8025T_DEVICE_ADDRESS, RX8025T_SEC_REG, rtc_str, 7);		
		return status;
}

//------------------------------------------------------------------------------------------------
//读出week值要转换一下 
//在RX8025寄存器里的星期值 星期天=0X01 星期1~6分别=0x02 0x04 0x08 0x10 0x20 0x40
//这个函数转换成星期1~7分别对应的1~7的数值 符合中国人思维
//返回星期值 1~7
uint8_t bsp_ReadRtcWeekChange(TIME_T *t)
{
    uint8_t weekTemp;
    weekTemp=t->week;
    switch(weekTemp)
    {
      case 0x01:weekTemp=7;break;
      case 0x02:weekTemp=1;break;
      case 0x04:weekTemp=2;break;
      case 0x08:weekTemp=3;break;
      case 0x10:weekTemp=4;break;
      case 0x20:weekTemp=5;break;
      case 0x30:weekTemp=6;break;
      default:break;
    }
    return weekTemp;
}

//------------------------------------------------------------------------------------------------
//写week之前转换一下
//中国人思维:星期1~7分别对应的1~7的数值
//在RX8025寄存器里的星期值 星期天=0X01 星期1~6分别=0x02 0x04 0x08 0x10 0x20 0x40
//所以在写星期之前 把1~7转换成对应寄存器里的值
//输入 TIME_T *t 我们将要往RX8025里写的结构体
//输入 uint8_t China_weekValut 我们的思维 week的值
void bsp_WriteRtcWeekChange(TIME_T *t,uint8_t China_weekValut)
{    
    switch(China_weekValut)
    {
      case 7:t->week=0X01;break;
      case 1:t->week=0X02;break;
      case 2:t->week=0X04;break;
      case 3:t->week=0X08;break;
      case 4:t->week=0X10;break;
      case 5:t->week=0X20;break;
      case 6:t->week=0X40;break;
      default:break;
    }
}



/*******************************************************************************
* 函数名: WriteI2CTData
* 描述  : 写入I2C数据
* 参数  : addr设备地址，reg 寄存器地址 *buf写入的数据，len写入的长度
* 返回值: 
*******************************************************************************/
HAL_StatusTypeDef WriteI2cData(uint8_t addr, uint8_t reg, uint8_t *pBuffer, uint8_t len)
{
	HAL_StatusTypeDef status = HAL_OK;
  
  status = HAL_I2C_Mem_Write(&hi2c1, addr, (uint16_t)reg, I2C_MEMADD_SIZE_8BIT, pBuffer, len, 1000); 
       
  return status;
}

/*******************************************************************************
* 函数名: ReadI2cData
* 描述  : 读RX8025T寄存器
* 参数  : addr寄存器地址，*buf存储位置，len读取的长度
* 返回值: 1=操作失败，0=操作成功
*******************************************************************************/
uint8_t ReadI2cData(uint8_t addr, uint8_t reg, uint8_t *buf,uint8_t len)
{
	HAL_StatusTypeDef status = HAL_OK;
	
	status = HAL_I2C_Mem_Read(&hi2c1, addr, (uint16_t)reg, I2C_MEMADD_SIZE_8BIT, buf, len, 1000);
	
	return status;
}

/*******************************************************************************
* 函数名: I2cIsDeviceReady
* 描述  : 检测I2C设备是否处于准备好可以通信状态
* 参数  : DevAddress 设备地址，Trials 尝试测试次数
* 返回值: 
*******************************************************************************/
HAL_StatusTypeDef I2cIsDeviceReady(uint16_t DevAddress, uint32_t Trials)
{ 
  return (HAL_I2C_IsDeviceReady(&hi2c1, DevAddress, Trials, 1000));
}



