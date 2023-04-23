#ifndef __SPI_W25Q64_H
#define __SPI_W25Q64_H

#include "myinclude.h"
#include <stdio.h>

/* Private typedef -----------------------------------------------------------*/
//����оƬ��Flash ID

#define  W25Q16_sFLASH_ID                       0xEF3015    
#define  W25Q32_sFLASH_ID                       0xEF4015	   
#define  W25Q64_sFLASH_ID                       0XEF4017    
#define  W25Q128_sFLASH_ID                      0XEF4018   

//����ָ���
#define W25X_WriteEnable		   	0x06 	//дʹ��
#define W25X_WriteDisable		   	0x04 	//д��ֹ
#define W25X_ReadStatusReg1		 	0x05 	//��״̬�Ĵ���1
#define W25X_ReadStatusReg2		 	0x35 	//��״̬�Ĵ���2
#define W25X_ReadStatusReg3		 	0x15 	//��״̬�Ĵ���3
#define W25X_WriteStatusReg1   		0x01 	//д״̬�Ĵ���1
#define W25X_WriteStatusReg2   		0x31 	//д״̬�Ĵ���2
#define W25X_WriteStatusReg3   		0x11 	//д״̬�Ĵ���3
#define W25X_ReadData				0x03 	//������
#define W25X_FastReadData		   	0x0B 	//���
#define W25X_FastReadDual		   	0x3B    //˫������
#define W25X_PageProgram		   	0x02 	//ҳ���
#define W25X_BlockErase			   	0xD8 	//�������64K��
#define W25X_SectorErase		   	0x20    //����������4K��
#define W25X_ChipErase			   	0xC7    //оƬ����
#define W25X_PowerDown			   	0xB9    //����
#define W25X_ReleasePowerDown	 	0xAB    //�ͷŵ���
#define W25X_DeviceID			    0xAB    //����ID
#define W25X_ManufactDeviceID	 	0x90    //������+�豸ID
#define W25X_JedecDeviceID		 	0x9F 	//����Ԫ��ID

#define Dummy_Byte                0xFF



uint8_t W25Q128_ReadWriteByte(uint8_t TxData);//������װһ��

uint16_t W25Q128_ReadID(void);//��ȡоƬID
uint32_t W25Q128_ReadDeviceID(void);//��ȡ�豸ID

uint8_t W25Q128_ReadSR(void);//��ȡ״̬�Ĵ���
void W25Q128_WriteSR(uint8_t sr);//д״̬�Ĵ���

void W25Q128_Write_Enable(void);//дʹ��
void W25Q128_Write_Disable(void);//��ֹд��	

void W25Q128_Read(uint8_t* pBuffer,uint32_t ReadAddr,uint16_t NumByteToRead); //��ȡ����
void W25Q128_Write_Page(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite);//ҳд
void W25Q128_Write_NoCheck(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite);//�޼���д���ݣ����Զ���ҳ
void W25Q128_Write(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite);//д�����ݣ�����д����

void W25Q128_Erase_Sector(uint32_t Dst_Addr);//��������
void W25Q128_Erase_Chip(void);//��������оƬ	

void W25Q128_Wait_Busy(void);//�ȴ�����
void W25Q128_PowerDown(void); //�������ģʽ
void W25Q128_WAKEUP(void);//���绽��

//---------------------------------------------------------------------
//�����ǽ�NSS�Զ���ΪӲ������
#define SPI_FLASH_NSS_SELECT()       HAL_GPIO_WritePin(GPIOA,W25Q16_CS_Pin, GPIO_PIN_RESET)
#define SPI_FLASH_NSS_ABANDON()   	 HAL_GPIO_WritePin(GPIOA,W25Q16_CS_Pin, GPIO_PIN_SET)
//#define SPI_FLASH_NSS_ABANDON()   	HAL_GPIO_WritePin(GPIOA,W25Q16_CS_Pin, GPIO_PIN_RESET);

//---------------------------------------------------------------------
//�����ǽ�NSS�Զ���Ϊĳһ���ܽ� ����Ա�Լ�����
//ѡ�������ģʽ �����˸��ܽų�ʼ��
//#define SPI_FLASH_NSS_SELECT()       __HAL_SPI_ENABLE(&hspi1);  
//#define SPI_FLASH_NSS_ABANDON()      __HAL_SPI_DISABLE(&hspi1); 


uint8_t W25Q128_TypeTest(void);

#endif /* __SPI_FLASH_H */




