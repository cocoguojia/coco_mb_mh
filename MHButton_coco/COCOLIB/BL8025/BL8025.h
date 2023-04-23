

#ifndef __BL8025_H__
#define __BL8025_H__

#include "stdint.h"
#include "myinclude.h"



#define RX8025T_SEC_REG		0x00	//��
#define RX8025T_MIN_REG 	0x01	//��
#define RX8025T_HOU_REG		0x02	//ʱ
#define RX8025T_WEE_REG	    0x03	//���ڣ�bit0~bit7��Ӧ�ա�һ�����������ġ��塢������ӦֵΪ0x01,0x02,0x04,0x08,0x10,0x20,0x40�����ɳ���2λΪ1�������
#define RX8025T_DAY_REG		0x04	//����
#define RX8025T_MON_REG 	0x05	//�·�
#define RX8025T_YEA_REG		0x06	//��
#define RX8025T_RAM_REG		0x07	//RAM

#define RX8025T_ALm_REG 	0x08	//���ӷ֣������ǿ���Ϊramʹ�á�
#define RX8025T_ALh_REG		0x09	//����ʱ�������ǿ���Ϊramʹ�á�

#define RX8025T_ALw_REG 	0x0a	//�������ڣ������ǿ���Ϊramʹ�á�
#define RX8025T_CYl_REG	    0x0b	//���ڶ�ʱ���ĵ�8λ
#define RX8025T_CYm_REG	    0x0c	//���ڶ�ʱ���ĸ�4λ�����ڶ�ʱ������12λ��

#define RX8025T_EXT_REG	    0x0d	//��չ�Ĵ�����bit7-TEST=�������ԣ���Ӧ��д0��bit6-WADA=���ڻ���������ѡ��λ��bit5-USEL=ѡ�������Ӹ��´��������жϣ�0=����£�1=���Ӹ��£�
																		        //bit4-TE=���ڶ�ʱʹ�ܣ�bit3\2-FSEL1\0=оƬFOUT�������Ƶ��ѡ��λ��bit1\0-TSEL1\0=�����趨�̶����ڵ��ڲ�ʱ��Դ��

#define RX8025T_FLAG_REG	0x0e	//��־�Ĵ�����bit5-UF��bit4-TF��bit3-AF���ֱ���ʱ������жϣ��̶����ڶ�ʱ�жϣ������жϵ��жϱ�־λ��bit1-VLF��ѹ�ͣ�bit0-VDET���ڵ�ѹ���²�ֹͣ������־λ��

#define RX8025T_CONT_REG 	0x0f	//���ƼĴ�����bit6~7(CSEL0��1)=�²�������ã�bit5(UIE)=ʱ������ж�ʹ��λ(����D�Ĵ�����USELλ����Ϊ1����»�1���Ӹ���)�� 
   

#define TESL1_0_CLOCK_4069			    0
#define TESL1_0_CLOCK_64				1
#define TESL1_0_CLOCK_S					2
#define TESL1_0_CLOCK_M					3


//TE λ  ��λ���������ƹ̶����ڶ�ʱ����ʹ�ܡ�
//�á�1����ѡ�����̶����ڶ�ʱ���ܡ�
//�á�0����ѡ��رչ̶����ڶ�ʱ���ܡ�
#define TE_1							1<<4	
#define TE_0							(~(1<<4))
	

#define TF_1							1<<4	
#define TF_0							(~(1<<4))


#define TF_FLAG							0x10


//��ʱ�ж�ʹ��λ
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

