 /**
  ******************************************************************************
  * @file    bsp_spi_flash.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   spi flash 底层应用函数bsp 
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火STM32 F103 开发板
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "w25q64.h"


extern SPI_HandleTypeDef hspi1;


//-----------------------------------------------------------------------------------------------------
//cocoguojia 改成HAL库代码 SPI发送过程中接收 
//只能用hal的HAL_SPI_TransmitReceive不能用2个单发单收(发的时候其实也接就是忽略了 收的时候先发了)
uint8_t SPI_FLASH_SendByte(uint8_t byte)
{
    uint8_t receDat;

    
    //------------------------------------------------------------------------------
    //cocoguojia send    
    HAL_SPI_TransmitReceive(&hspi1,&byte, &receDat,1, 10);
    
//    returnDat=HAL_SPI_TransmitReceive(&hspi1,&byte, &receDat,1, 10); 
    
//    do
//    {
//        returnDat=HAL_SPI_TransmitReceive(&hspi1,&byte, &receDat,1, 10); 
//        if(HAL_OK!=returnDat)
//        {
//            osDelay(10);
//        }        
//    }
//    while(HAL_OK!=returnDat);
    return receDat;
}




u8 SPI1_ReadWriteByte(uint8_t byte)
{
    return SPI_FLASH_SendByte( byte);    
}


uint8_t W25Q128_ReadWriteByte(uint8_t TxData)//函数包装一下
{     
 	return SPI1_ReadWriteByte(TxData); 		
}


uint16_t W25Q128_ReadID(void)//读取芯片ID=0XEF17
{
	uint16_t Temp = 0;	  
	SPI_FLASH_NSS_SELECT() ; 				    
	W25Q128_ReadWriteByte(W25X_ManufactDeviceID);//发送读取ID命令	    
	W25Q128_ReadWriteByte(0x00); 	    
	W25Q128_ReadWriteByte(0x00); 
    W25Q128_ReadWriteByte(0x00); 
    
	Temp=W25Q128_ReadWriteByte(Dummy_Byte);    
    Temp<<=8; 
	Temp+=(uint16_t)W25Q128_ReadWriteByte(Dummy_Byte); 
	SPI_FLASH_NSS_ABANDON();				    
	return Temp;
}

uint32_t W25Q128_ReadDeviceID(void) //读取设备ID
{
	uint32_t Temp = 0;	  
	SPI_FLASH_NSS_SELECT() ; 				    
	W25Q128_ReadWriteByte(W25X_JedecDeviceID);//发送读取ID命令	    
	Temp=(uint32_t)W25Q128_ReadWriteByte(Dummy_Byte); 
    Temp<<=8;    
	Temp+=(uint32_t)W25Q128_ReadWriteByte(Dummy_Byte);
    Temp<<=8;     
	Temp+=(uint32_t)W25Q128_ReadWriteByte(Dummy_Byte);
    
	SPI_FLASH_NSS_ABANDON();				    
	return Temp;
}


   

//读取W25Q128的状态寄存器
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
//SPR:默认0,状态寄存器保护位,配合WP使用
//TB,BP2,BP1,BP0:FLASH区域写保护设置
//WEL:写使能锁定
//BUSY:忙标记位(1,忙;0,空闲)
//默认:0x00
uint8_t W25Q128_ReadSR(void)//读取状态寄存器
{
    uint8_t byte=0;
    SPI_FLASH_NSS_SELECT() ;                               //使能器件
    W25Q128_ReadWriteByte(W25X_ReadStatusReg1);  //发送读取状态寄存器命令
    byte=W25Q128_ReadWriteByte(0Xff);            //读取一个字节
    SPI_FLASH_NSS_ABANDON();                               //取消片选
    return byte;
}

//写W25Q128状态寄存器
//只有SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)可以写!!!
void W25Q128_WriteSR(uint8_t sr)//写状态寄存器
{
    SPI_FLASH_NSS_SELECT() ;                           		//使能器件
    W25Q128_ReadWriteByte(W25X_WriteStatusReg1);	//发送写取状态寄存器命令
    W25Q128_ReadWriteByte(sr);               		//写入一个字节
    SPI_FLASH_NSS_ABANDON();                            		//取消片选
}

void W25Q128_Write_Enable(void) //写使能 
{
  SPI_FLASH_NSS_SELECT() ;	
	W25Q128_ReadWriteByte(W25X_WriteEnable);
	SPI_FLASH_NSS_ABANDON();	
}

void W25Q128_Write_Disable(void) //禁止写入	
{
   	SPI_FLASH_NSS_SELECT() ;	
	W25Q128_ReadWriteByte(W25X_WriteDisable);
	SPI_FLASH_NSS_ABANDON();	
}

void W25Q128_Read(uint8_t* pBuffer,uint32_t ReadAddr,uint16_t NumByteToRead)   
{ 						    
	SPI_FLASH_NSS_SELECT() ;	                        //使能器件   
    W25Q128_ReadWriteByte(W25X_ReadData);               //发送读取命令  
    W25Q128_ReadWriteByte((uint8_t)((ReadAddr)>>16));   //发送24bit地址    
    W25Q128_ReadWriteByte((uint8_t)((ReadAddr)>>8));   
    W25Q128_ReadWriteByte((uint8_t)ReadAddr);   
    for(uint16_t i=0;i<NumByteToRead;i++)
	{ 
       pBuffer[i]=W25Q128_ReadWriteByte(0XFF);    //循环读数  
    }
	SPI_FLASH_NSS_ABANDON();				    	      
}  


void W25Q128_Write_Page(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)
{
  
	W25Q128_Write_Enable();                  //SET WEL 
	SPI_FLASH_NSS_SELECT() ;                            //使能器件   
	W25Q128_ReadWriteByte(W25X_PageProgram);   //发送写页命令   

	W25Q128_ReadWriteByte((uint8_t)((WriteAddr)>>16)); //发送24bit地址    
	W25Q128_ReadWriteByte((uint8_t)((WriteAddr)>>8));   
	W25Q128_ReadWriteByte((uint8_t)WriteAddr);   
  for(uint16_t i=0;i<NumByteToWrite;i++)
	{
		W25Q128_ReadWriteByte(pBuffer[i]);//循环写数  
	}
	SPI_FLASH_NSS_ABANDON();                            //取消片选 
	W25Q128_Wait_Busy();					   //等待写入结束
} 

//无检验写SPI FLASH 
//必须确保所写的地址范围内的数据全部为0XFF,否则在非0XFF处写入的数据将失败!
//具有自动换页功能 
void W25Q128_Write_NoCheck(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)   
{ 			 		 
	uint16_t pageremain=256-WriteAddr%256; //单页剩余的字节数		 	    
	if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;//不大于256个字节
	while(1)
	{	   
		W25Q128_Write_Page(pBuffer,WriteAddr,pageremain);
		if(NumByteToWrite==pageremain)	break;//写入结束了
	 	else 
		{
			pBuffer+=pageremain;
			WriteAddr+=pageremain;	
			NumByteToWrite-=pageremain;			  //减去已经写入了的字节数
			if(NumByteToWrite>256)pageremain=256; //一次可以写入256个字节
			else pageremain=NumByteToWrite; 	  //不够256个字节了
		}
	}	    
} 

//写SPI FLASH  
//在指定地址开始写入指定长度的数据
//该函数带擦除操作!
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)						
//NumByteToWrite:要写入的字节数(最大65535)   
uint8_t W25Q128_BUFFER[4096];		 
void W25Q128_Write(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)   
{ 
 	uint16_t i;    
	uint8_t * W25Q128_BUF;	  
    W25Q128_BUF=W25Q128_BUFFER;	     
 	uint32_t secpos = WriteAddr/4096;//扇区地址  
	uint16_t secoff = WriteAddr%4096;//在扇区内的偏移
	uint16_t secremain = 4096-secoff;//扇区剩余空间大小   

 	if(NumByteToWrite<=secremain)		secremain=NumByteToWrite;//不大于4096个字节
	while(1) 
	{	
		W25Q128_Read(W25Q128_BUF,secpos*4096,4096);//读出整个扇区的内容
		for(i=0;i<secremain;i++)//校验数据
		{
			if(W25Q128_BUF[secoff+i]!=0XFF)	break;//需要擦除  	  
		}
		if(i<secremain)//需要擦除
		{
			W25Q128_Erase_Sector(secpos*4096);//擦除这个扇区
			for(i=0;i<secremain;i++)	   //复制
			{
				W25Q128_BUF[i+secoff]=pBuffer[i];	  
			}
			W25Q128_Write_NoCheck(W25Q128_BUF,secpos*4096,4096);//写入整个扇区  

		}else W25Q128_Write_NoCheck(pBuffer,WriteAddr,secremain);//写已经擦除了的,直接写入扇区剩余区间. 				   
		if(NumByteToWrite==secremain)	break;//写入结束了
		else//写入未结束
		{
			secpos++;//扇区地址增1
			secoff=0;//偏移位置为0 	 

		    pBuffer+=secremain;  //指针偏移
			WriteAddr+=secremain;//写地址偏移	   
		    NumByteToWrite-=secremain;				//字节数递减
			if(NumByteToWrite>4096)	secremain=4096;	//下一个扇区还是写不完
			else 	secremain=NumByteToWrite;		//下一个扇区可以写完了
		}	 
	}	 
}

//擦除一个扇区
//Dst_Addr:扇区地址 根据实际容量设置
//擦除一个扇区的最少时间:150ms

void W25Q128_Erase_Sector(uint32_t Dst_Addr)   
{  
    W25Q128_Write_Enable();                  //SET WEL 	 
    W25Q128_Wait_Busy();   
  	SPI_FLASH_NSS_SELECT() ;                            //使能器件   
    W25Q128_ReadWriteByte(W25X_SectorErase);   //发送扇区擦除指令 
    W25Q128_ReadWriteByte((uint8_t)((Dst_Addr)>>16));  //发送24bit地址    
    W25Q128_ReadWriteByte((uint8_t)((Dst_Addr)>>8));   
    W25Q128_ReadWriteByte((uint8_t)Dst_Addr);  
	SPI_FLASH_NSS_ABANDON();                            	//取消片选     	      
    W25Q128_Wait_Busy();   				    	//等待擦除完成
}  

//擦除整个芯片		  
//等待时间超长...
void W25Q128_Erase_Chip(void)   
{                                   
    W25Q128_Write_Enable();                  //SET WEL 
    W25Q128_Wait_Busy();   
  	SPI_FLASH_NSS_SELECT() ;                           //使能器件   
    W25Q128_ReadWriteByte(W25X_ChipErase);   //发送片擦除命令  
	SPI_FLASH_NSS_ABANDON();                           //取消片选     	      
	W25Q128_Wait_Busy();   				   	//等待芯片擦除结束
}


//等待空闲
void W25Q128_Wait_Busy(void)   
{   
	while((W25Q128_ReadSR()&0x01)==0x01);   // 等待BUSY位清空
}  

//进入掉电模式
void W25Q128_PowerDown(void)   
{ 
  	SPI_FLASH_NSS_SELECT() ;                            //使能器件   
    W25Q128_ReadWriteByte(W25X_PowerDown);    //发送掉电命令  
	SPI_FLASH_NSS_ABANDON();                            //取消片选     	      
}   
//掉电唤醒
void W25Q128_WAKEUP(void)   
{  
  	SPI_FLASH_NSS_SELECT() ;                                //使能器件   
    W25Q128_ReadWriteByte(W25X_ReleasePowerDown);    
	SPI_FLASH_NSS_ABANDON();                                //取消片选     	      
} 


//---------------------------------------------------------
//型号识别 
//成功返回0 失败返回1
uint8_t W25Q128_TypeTest(void)
{
    uint32_t deviceN=0;
    uint8_t ret=0;
    
    deviceN = W25Q128_ReadID();//ID为0XEF17
    App_Printf("$$FlashID=0x%X\r\n",(uint16_t)deviceN);
    
    deviceN=W25Q128_ReadDeviceID();

    switch(deviceN)
    {
        case  W25Q16_sFLASH_ID:
        App_Printf("$$ReadDeviceID=0x%X->W25Q16\r\n",deviceN);
        break;
        
        case  W25Q32_sFLASH_ID:
        App_Printf("$$ReadDeviceID=0x%X->W25Q32\r\n",deviceN);
        break;
        
        case  W25Q64_sFLASH_ID:
        App_Printf("$$ReadDeviceID=0x%X->W25Q64\r\n",deviceN);
        break;
        
        case  W25Q128_sFLASH_ID:
        App_Printf("$$ReadDeviceID=0x%X->W25Q128\r\n",deviceN);
        break;
        
        default:
        App_Printf("$$ReadDeviceID=0x%X->未知型号及其容量\r\n",deviceN);
        ret=1;
        break;
    }
       
    return ret;  
}





