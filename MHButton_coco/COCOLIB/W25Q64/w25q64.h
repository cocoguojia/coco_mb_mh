#ifndef __SPI_W25Q64_H
#define __SPI_W25Q64_H

#include "myinclude.h"
#include <stdio.h>

/* Private typedef -----------------------------------------------------------*/
//各个芯片的Flash ID

#define  W25Q16_sFLASH_ID                       0xEF3015    
#define  W25Q32_sFLASH_ID                       0xEF4015	   
#define  W25Q64_sFLASH_ID                       0XEF4017    
#define  W25Q128_sFLASH_ID                      0XEF4018   

//操作指令表
#define W25X_WriteEnable		   	0x06 	//写使能
#define W25X_WriteDisable		   	0x04 	//写禁止
#define W25X_ReadStatusReg1		 	0x05 	//读状态寄存器1
#define W25X_ReadStatusReg2		 	0x35 	//读状态寄存器2
#define W25X_ReadStatusReg3		 	0x15 	//读状态寄存器3
#define W25X_WriteStatusReg1   		0x01 	//写状态寄存器1
#define W25X_WriteStatusReg2   		0x31 	//写状态寄存器2
#define W25X_WriteStatusReg3   		0x11 	//写状态寄存器3
#define W25X_ReadData				0x03 	//读数据
#define W25X_FastReadData		   	0x0B 	//快读
#define W25X_FastReadDual		   	0x3B    //双输出快读
#define W25X_PageProgram		   	0x02 	//页编程
#define W25X_BlockErase			   	0xD8 	//块擦除（64K）
#define W25X_SectorErase		   	0x20    //扇区擦除（4K）
#define W25X_ChipErase			   	0xC7    //芯片擦除
#define W25X_PowerDown			   	0xB9    //掉电
#define W25X_ReleasePowerDown	 	0xAB    //释放掉电
#define W25X_DeviceID			    0xAB    //器件ID
#define W25X_ManufactDeviceID	 	0x90    //制造商+设备ID
#define W25X_JedecDeviceID		 	0x9F 	//电子元件ID

#define Dummy_Byte                0xFF



uint8_t W25Q128_ReadWriteByte(uint8_t TxData);//函数包装一下

uint16_t W25Q128_ReadID(void);//读取芯片ID
uint32_t W25Q128_ReadDeviceID(void);//读取设备ID

uint8_t W25Q128_ReadSR(void);//读取状态寄存器
void W25Q128_WriteSR(uint8_t sr);//写状态寄存器

void W25Q128_Write_Enable(void);//写使能
void W25Q128_Write_Disable(void);//禁止写入	

void W25Q128_Read(uint8_t* pBuffer,uint32_t ReadAddr,uint16_t NumByteToRead); //读取数据
void W25Q128_Write_Page(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite);//页写
void W25Q128_Write_NoCheck(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite);//无检验写数据，可自动翻页
void W25Q128_Write(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite);//写入数据，带擦写功能

void W25Q128_Erase_Sector(uint32_t Dst_Addr);//擦除扇区
void W25Q128_Erase_Chip(void);//擦除整个芯片	

void W25Q128_Wait_Busy(void);//等待空闲
void W25Q128_PowerDown(void); //进入掉电模式
void W25Q128_WAKEUP(void);//掉电唤醒

//---------------------------------------------------------------------
//这里是将NSS自动归为硬件管理
#define SPI_FLASH_NSS_SELECT()       HAL_GPIO_WritePin(GPIOA,W25Q16_CS_Pin, GPIO_PIN_RESET)
#define SPI_FLASH_NSS_ABANDON()   	 HAL_GPIO_WritePin(GPIOA,W25Q16_CS_Pin, GPIO_PIN_SET)
//#define SPI_FLASH_NSS_ABANDON()   	HAL_GPIO_WritePin(GPIOA,W25Q16_CS_Pin, GPIO_PIN_RESET);

//---------------------------------------------------------------------
//这里是将NSS自动归为某一个管脚 程序员自己管理
//选择了这个模式 别忘了给管脚初始化
//#define SPI_FLASH_NSS_SELECT()       __HAL_SPI_ENABLE(&hspi1);  
//#define SPI_FLASH_NSS_ABANDON()      __HAL_SPI_DISABLE(&hspi1); 


uint8_t W25Q128_TypeTest(void);

#endif /* __SPI_FLASH_H */




