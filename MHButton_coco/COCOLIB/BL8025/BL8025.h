

#ifndef __BL8025_H__
#define __BL8025_H__

#include "stdint.h"
#include "myinclude.h"



#define RX8025T_SEC_REG		0x00	//秒
#define RX8025T_MIN_REG 	0x01	//分
#define RX8025T_HOU_REG		0x02	//时
#define RX8025T_WEE_REG	    0x03	//星期，bit0~bit7对应日、一、二、三、四、五、六，对应值为0x01,0x02,0x04,0x08,0x10,0x20,0x40，不可出现2位为1的情况。
#define RX8025T_DAY_REG		0x04	//日期
#define RX8025T_MON_REG 	0x05	//月份
#define RX8025T_YEA_REG		0x06	//年
#define RX8025T_RAM_REG		0x07	//RAM

#define RX8025T_ALm_REG 	0x08	//闹钟分，不用是可做为ram使用。
#define RX8025T_ALh_REG		0x09	//闹钟时，不用是可做为ram使用。

#define RX8025T_ALw_REG 	0x0a	//闹钟星期，不用是可做为ram使用。
#define RX8025T_CYl_REG	    0x0b	//周期定时器的低8位
#define RX8025T_CYm_REG	    0x0c	//周期定时器的高4位，周期定时器共计12位。

#define RX8025T_EXT_REG	    0x0d	//扩展寄存器，bit7-TEST=工厂测试，总应该写0；bit6-WADA=星期或日历报警选择位；bit5-USEL=选择秒或分钟更新触发更新中断，0=秒更新，1=分钟更新；
																		        //bit4-TE=周期定时使能；bit3\2-FSEL1\0=芯片FOUT引脚输出频率选择位；bit1\0-TSEL1\0=用来设定固定周期的内部时钟源。

#define RX8025T_FLAG_REG	0x0e	//标志寄存器，bit5-UF，bit4-TF，bit3-AF，分别是时间更新中断，固定周期定时中断，闹钟中断的中断标志位；bit1-VLF电压低，bit0-VDET由于电压低温补停止工作标志位。

#define RX8025T_CONT_REG 	0x0f	//控制寄存器，bit6~7(CSEL0、1)=温补间隔设置；bit5(UIE)=时间更新中断使能位(可由D寄存器的USEL位配置为1秒更新或1分钟更新)； 
   

#define TESL1_0_CLOCK_4069			    0
#define TESL1_0_CLOCK_64				1
#define TESL1_0_CLOCK_S					2
#define TESL1_0_CLOCK_M					3


//TE 位  此位是用来控制固定周期定时功能使能。
//置“1”是选择开启固定周期定时功能。
//置“0”是选择关闭固定周期定时功能。
#define TE_1							1<<4	
#define TE_0							(~(1<<4))
	

#define TF_1							1<<4	
#define TF_0							(~(1<<4))


#define TF_FLAG							0x10


//定时中断使能位
#define TIE_1							1<<4	
#define TIE_0							(~(1<<4))


typedef struct  // _TIME  
{
	uint8_t second;
	uint8_t minute;
	uint8_t hour;
	uint8_t week;
	uint8_t day;
	uint8_t month;
	uint8_t year;
	uint8_t reserve;
}TIME_T;


uint8_t bsp_InitRtc(void);
uint8_t bsp_SetRTCAlarm(uint16_t alarm);
uint8_t bsp_SetRtcTime(TIME_T *t);
uint8_t bsp_GetRtcTime(TIME_T *t);
HAL_StatusTypeDef I2cIsDeviceReady(uint16_t DevAddress, uint32_t Trials);

uint8_t bsp_ReadRtcWeekChange(TIME_T *t);
void bsp_WriteRtcWeekChange(TIME_T *t,uint8_t China_weekValut);

#endif

