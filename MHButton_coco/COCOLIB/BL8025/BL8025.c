

#include "BL8025.h"




/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//RX8025Tʵʱʱ����������(Ӳ��IIC)
//BL8025T �Ĵӵ�ַΪ 7bit �̶������ݣ�0110 010����ͨ��ʱ���ӵ�ַ�Ǹ����� R/W �� 8bit ���ݷ��͵ġ�
//0110 0100Ϊдģʽ��0110 0101Ϊ��ģʽ,��Ӧ16����Ϊ��0x64��0x65��
//BL8025T �е�ַ�Զ����ӹ��ܡ�ָ���Ĵӵ�ַһ����ʼ��֮��ֻ�������ֽڱ����͡�ÿ���ֽں�BL8025T �ĵ�ַ�Զ����ӡ�
		
#define  RX8025T_DEVICE_ADDRESS  0x64



extern I2C_HandleTypeDef hi2c1;

HAL_StatusTypeDef WriteI2cData(uint8_t addr, uint8_t reg, uint8_t *pBuffer, uint8_t len);
uint8_t ReadI2cData(uint8_t addr, uint8_t reg, uint8_t *buf,uint8_t len);

/*
*********************************************************************************************************
*	�� �� ��: bsp_InitRtc
*	����˵��: ��ʼRTC. �ú����� bsp_Init() ���á�
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
uint8_t bsp_InitRtc(void)
{
    HAL_StatusTypeDef ret;
    uint8_t returnN=0;

    //�̶�����ʱ��ԴΪ���Ӹ���
	uint8_t val[3]={0x03,0x00,0x40};  //0x0D��0x0E��0x0F�������Ĵ�����ֵ������ʱ�����Ϊ���롱���£��ر��������ӣ��²�ʱ��Ϊ2�룬��ʱ������жϣ��ر������жϡ�
		   
	
	ret = WriteI2cData(RX8025T_DEVICE_ADDRESS, RX8025T_EXT_REG,val,3);
    
    ret+=I2cIsDeviceReady(0x64,3);//��������Ƿ���� ����3��
    
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
* ������: bsp_SetRTCAlarm
* ����  : ����RX8025T������
* ����  : �洢ʱ��Ľṹ��
* ����ֵ: 0�ɹ�������ʧ�ܡ�
*******************************************************************************/
uint8_t bsp_SetRTCAlarm(uint16_t alarm)
{
	 	uint8_t  status;
		uint8_t  value = 0;
		uint8_t  reg_cy1[2];//�����̶����ڼĴ���
		uint8_t  reg[3];
//	uint8_t  buf[5];
/*
		�̶����ڶ�ʱ������

		�Ƚ�TIE�á�0�����Ա��������ù̶������жϵ�ͬʱ���������Ӳ���жϡ�
		��1�� �趨TSEL1,0��λѡ�񵹼�ʱ���ڡ�
		��2�� �趨B,C�Ĵ������Ӷ����ü����������ĳ�ֵ��Ȼ���ʼ��TF��־Ϊ��0����
		��3�� ����TIE,TEλΪ��1��

*/	
			value = 0x40;
			status = WriteI2cData(RX8025T_DEVICE_ADDRESS, RX8025T_CONT_REG, &value, 1);
	
			value = 0x00;
			status = WriteI2cData(RX8025T_DEVICE_ADDRESS, RX8025T_EXT_REG, &value, 1);			
			
			reg_cy1[0] = alarm & 0xFF;
			reg_cy1[1] = (alarm >> 8) & 0xFF;			
			status = WriteI2cData(RX8025T_DEVICE_ADDRESS, RX8025T_CYl_REG, reg_cy1, 2);		
			
			reg[0] = 0x13;		//���Ӹ���
			reg[1] = 0x10;
			reg[2] = 0x50;	
			status = WriteI2cData(RX8025T_DEVICE_ADDRESS, RX8025T_EXT_REG, reg, 3);		
	
			return status;
  
}

/*******************************************************************************
* ������: bsp_GetRtcTime
* ����  : ��RX8025T��ȡʱ�� 
* ����  : �洢ʱ��Ľṹ��
* ����ֵ: 0�ɹ���1ʧ�ܡ�
*******************************************************************************/
uint8_t bsp_GetRtcTime(TIME_T *t)
{
	uint8_t rtc_str[7];
  
	if(ReadI2cData(RX8025T_DEVICE_ADDRESS, RX8025T_SEC_REG, rtc_str,7) != 0)  //��ȡ������ʱ��
		return 1;  //��ȡ����

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
* ������: bsp_SetRtcTime
* ����  : ����RX8025Tʱ�� 
* ����  : �洢ʱ��Ľṹ��
* ����ֵ: 0�ɹ���1ʧ�ܡ�
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
//����weekֵҪת��һ�� 
//��RX8025�Ĵ����������ֵ ������=0X01 ����1~6�ֱ�=0x02 0x04 0x08 0x10 0x20 0x40
//�������ת��������1~7�ֱ��Ӧ��1~7����ֵ �����й���˼ά
//��������ֵ 1~7
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
//дweek֮ǰת��һ��
//�й���˼ά:����1~7�ֱ��Ӧ��1~7����ֵ
//��RX8025�Ĵ����������ֵ ������=0X01 ����1~6�ֱ�=0x02 0x04 0x08 0x10 0x20 0x40
//������д����֮ǰ ��1~7ת���ɶ�Ӧ�Ĵ������ֵ
//���� TIME_T *t ���ǽ�Ҫ��RX8025��д�Ľṹ��
//���� uint8_t China_weekValut ���ǵ�˼ά week��ֵ
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
* ������: WriteI2CTData
* ����  : д��I2C����
* ����  : addr�豸��ַ��reg �Ĵ�����ַ *bufд������ݣ�lenд��ĳ���
* ����ֵ: 
*******************************************************************************/
HAL_StatusTypeDef WriteI2cData(uint8_t addr, uint8_t reg, uint8_t *pBuffer, uint8_t len)
{
	HAL_StatusTypeDef status = HAL_OK;
  
  status = HAL_I2C_Mem_Write(&hi2c1, addr, (uint16_t)reg, I2C_MEMADD_SIZE_8BIT, pBuffer, len, 1000); 
       
  return status;
}

/*******************************************************************************
* ������: ReadI2cData
* ����  : ��RX8025T�Ĵ���
* ����  : addr�Ĵ�����ַ��*buf�洢λ�ã�len��ȡ�ĳ���
* ����ֵ: 1=����ʧ�ܣ�0=�����ɹ�
*******************************************************************************/
uint8_t ReadI2cData(uint8_t addr, uint8_t reg, uint8_t *buf,uint8_t len)
{
	HAL_StatusTypeDef status = HAL_OK;
	
	status = HAL_I2C_Mem_Read(&hi2c1, addr, (uint16_t)reg, I2C_MEMADD_SIZE_8BIT, buf, len, 1000);
	
	return status;
}

/*******************************************************************************
* ������: I2cIsDeviceReady
* ����  : ���I2C�豸�Ƿ���׼���ÿ���ͨ��״̬
* ����  : DevAddress �豸��ַ��Trials ���Բ��Դ���
* ����ֵ: 
*******************************************************************************/
HAL_StatusTypeDef I2cIsDeviceReady(uint16_t DevAddress, uint32_t Trials)
{ 
  return (HAL_I2C_IsDeviceReady(&hi2c1, DevAddress, Trials, 1000));
}



