 /**
  ******************************************************************************
  * @file    bsp_spi_flash.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   spi flash �ײ�Ӧ�ú���bsp 
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��STM32 F103 ������
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "w25q64.h"


extern SPI_HandleTypeDef hspi1;


//-----------------------------------------------------------------------------------------------------
//cocoguojia �ĳ�HAL����� SPI���͹����н��� 
//ֻ����hal��HAL_SPI_TransmitReceive������2����������(����ʱ����ʵҲ�Ӿ��Ǻ����� �յ�ʱ���ȷ���)
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


uint8_t W25Q128_ReadWriteByte(uint8_t TxData)//������װһ��
{     
 	return SPI1_ReadWriteByte(TxData); 		
}


uint16_t W25Q128_ReadID(void)//��ȡоƬID=0XEF17
{
	uint16_t Temp = 0;	  
	SPI_FLASH_NSS_SELECT() ; 				    
	W25Q128_ReadWriteByte(W25X_ManufactDeviceID);//���Ͷ�ȡID����	    
	W25Q128_ReadWriteByte(0x00); 	    
	W25Q128_ReadWriteByte(0x00); 
    W25Q128_ReadWriteByte(0x00); 
    
	Temp=W25Q128_ReadWriteByte(Dummy_Byte);    
    Temp<<=8; 
	Temp+=(uint16_t)W25Q128_ReadWriteByte(Dummy_Byte); 
	SPI_FLASH_NSS_ABANDON();				    
	return Temp;
}

uint32_t W25Q128_ReadDeviceID(void) //��ȡ�豸ID
{
	uint32_t Temp = 0;	  
	SPI_FLASH_NSS_SELECT() ; 				    
	W25Q128_ReadWriteByte(W25X_JedecDeviceID);//���Ͷ�ȡID����	    
	Temp=(uint32_t)W25Q128_ReadWriteByte(Dummy_Byte); 
    Temp<<=8;    
	Temp+=(uint32_t)W25Q128_ReadWriteByte(Dummy_Byte);
    Temp<<=8;     
	Temp+=(uint32_t)W25Q128_ReadWriteByte(Dummy_Byte);
    
	SPI_FLASH_NSS_ABANDON();				    
	return Temp;
}


   

//��ȡW25Q128��״̬�Ĵ���
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
//SPR:Ĭ��0,״̬�Ĵ�������λ,���WPʹ��
//TB,BP2,BP1,BP0:FLASH����д��������
//WEL:дʹ������
//BUSY:æ���λ(1,æ;0,����)
//Ĭ��:0x00
uint8_t W25Q128_ReadSR(void)//��ȡ״̬�Ĵ���
{
    uint8_t byte=0;
    SPI_FLASH_NSS_SELECT() ;                               //ʹ������
    W25Q128_ReadWriteByte(W25X_ReadStatusReg1);  //���Ͷ�ȡ״̬�Ĵ�������
    byte=W25Q128_ReadWriteByte(0Xff);            //��ȡһ���ֽ�
    SPI_FLASH_NSS_ABANDON();                               //ȡ��Ƭѡ
    return byte;
}

//дW25Q128״̬�Ĵ���
//ֻ��SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)����д!!!
void W25Q128_WriteSR(uint8_t sr)//д״̬�Ĵ���
{
    SPI_FLASH_NSS_SELECT() ;                           		//ʹ������
    W25Q128_ReadWriteByte(W25X_WriteStatusReg1);	//����дȡ״̬�Ĵ�������
    W25Q128_ReadWriteByte(sr);               		//д��һ���ֽ�
    SPI_FLASH_NSS_ABANDON();                            		//ȡ��Ƭѡ
}

void W25Q128_Write_Enable(void) //дʹ�� 
{
  SPI_FLASH_NSS_SELECT() ;	
	W25Q128_ReadWriteByte(W25X_WriteEnable);
	SPI_FLASH_NSS_ABANDON();	
}

void W25Q128_Write_Disable(void) //��ֹд��	
{
   	SPI_FLASH_NSS_SELECT() ;	
	W25Q128_ReadWriteByte(W25X_WriteDisable);
	SPI_FLASH_NSS_ABANDON();	
}

void W25Q128_Read(uint8_t* pBuffer,uint32_t ReadAddr,uint16_t NumByteToRead)   
{ 						    
	SPI_FLASH_NSS_SELECT() ;	                        //ʹ������   
    W25Q128_ReadWriteByte(W25X_ReadData);               //���Ͷ�ȡ����  
    W25Q128_ReadWriteByte((uint8_t)((ReadAddr)>>16));   //����24bit��ַ    
    W25Q128_ReadWriteByte((uint8_t)((ReadAddr)>>8));   
    W25Q128_ReadWriteByte((uint8_t)ReadAddr);   
    for(uint16_t i=0;i<NumByteToRead;i++)
	{ 
       pBuffer[i]=W25Q128_ReadWriteByte(0XFF);    //ѭ������  
    }
	SPI_FLASH_NSS_ABANDON();				    	      
}  


void W25Q128_Write_Page(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)
{
  
	W25Q128_Write_Enable();                  //SET WEL 
	SPI_FLASH_NSS_SELECT() ;                            //ʹ������   
	W25Q128_ReadWriteByte(W25X_PageProgram);   //����дҳ����   

	W25Q128_ReadWriteByte((uint8_t)((WriteAddr)>>16)); //����24bit��ַ    
	W25Q128_ReadWriteByte((uint8_t)((WriteAddr)>>8));   
	W25Q128_ReadWriteByte((uint8_t)WriteAddr);   
  for(uint16_t i=0;i<NumByteToWrite;i++)
	{
		W25Q128_ReadWriteByte(pBuffer[i]);//ѭ��д��  
	}
	SPI_FLASH_NSS_ABANDON();                            //ȡ��Ƭѡ 
	W25Q128_Wait_Busy();					   //�ȴ�д�����
} 

//�޼���дSPI FLASH 
//����ȷ����д�ĵ�ַ��Χ�ڵ�����ȫ��Ϊ0XFF,�����ڷ�0XFF��д������ݽ�ʧ��!
//�����Զ���ҳ���� 
void W25Q128_Write_NoCheck(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)   
{ 			 		 
	uint16_t pageremain=256-WriteAddr%256; //��ҳʣ����ֽ���		 	    
	if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;//������256���ֽ�
	while(1)
	{	   
		W25Q128_Write_Page(pBuffer,WriteAddr,pageremain);
		if(NumByteToWrite==pageremain)	break;//д�������
	 	else 
		{
			pBuffer+=pageremain;
			WriteAddr+=pageremain;	
			NumByteToWrite-=pageremain;			  //��ȥ�Ѿ�д���˵��ֽ���
			if(NumByteToWrite>256)pageremain=256; //һ�ο���д��256���ֽ�
			else pageremain=NumByteToWrite; 	  //����256���ֽ���
		}
	}	    
} 

//дSPI FLASH  
//��ָ����ַ��ʼд��ָ�����ȵ�����
//�ú�������������!
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)						
//NumByteToWrite:Ҫд����ֽ���(���65535)   
uint8_t W25Q128_BUFFER[4096];		 
void W25Q128_Write(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)   
{ 
 	uint16_t i;    
	uint8_t * W25Q128_BUF;	  
    W25Q128_BUF=W25Q128_BUFFER;	     
 	uint32_t secpos = WriteAddr/4096;//������ַ  
	uint16_t secoff = WriteAddr%4096;//�������ڵ�ƫ��
	uint16_t secremain = 4096-secoff;//����ʣ��ռ��С   

 	if(NumByteToWrite<=secremain)		secremain=NumByteToWrite;//������4096���ֽ�
	while(1) 
	{	
		W25Q128_Read(W25Q128_BUF,secpos*4096,4096);//������������������
		for(i=0;i<secremain;i++)//У������
		{
			if(W25Q128_BUF[secoff+i]!=0XFF)	break;//��Ҫ����  	  
		}
		if(i<secremain)//��Ҫ����
		{
			W25Q128_Erase_Sector(secpos*4096);//�����������
			for(i=0;i<secremain;i++)	   //����
			{
				W25Q128_BUF[i+secoff]=pBuffer[i];	  
			}
			W25Q128_Write_NoCheck(W25Q128_BUF,secpos*4096,4096);//д����������  

		}else W25Q128_Write_NoCheck(pBuffer,WriteAddr,secremain);//д�Ѿ������˵�,ֱ��д������ʣ������. 				   
		if(NumByteToWrite==secremain)	break;//д�������
		else//д��δ����
		{
			secpos++;//������ַ��1
			secoff=0;//ƫ��λ��Ϊ0 	 

		    pBuffer+=secremain;  //ָ��ƫ��
			WriteAddr+=secremain;//д��ַƫ��	   
		    NumByteToWrite-=secremain;				//�ֽ����ݼ�
			if(NumByteToWrite>4096)	secremain=4096;	//��һ����������д����
			else 	secremain=NumByteToWrite;		//��һ����������д����
		}	 
	}	 
}

//����һ������
//Dst_Addr:������ַ ����ʵ����������
//����һ������������ʱ��:150ms

void W25Q128_Erase_Sector(uint32_t Dst_Addr)   
{  
    W25Q128_Write_Enable();                  //SET WEL 	 
    W25Q128_Wait_Busy();   
  	SPI_FLASH_NSS_SELECT() ;                            //ʹ������   
    W25Q128_ReadWriteByte(W25X_SectorErase);   //������������ָ�� 
    W25Q128_ReadWriteByte((uint8_t)((Dst_Addr)>>16));  //����24bit��ַ    
    W25Q128_ReadWriteByte((uint8_t)((Dst_Addr)>>8));   
    W25Q128_ReadWriteByte((uint8_t)Dst_Addr);  
	SPI_FLASH_NSS_ABANDON();                            	//ȡ��Ƭѡ     	      
    W25Q128_Wait_Busy();   				    	//�ȴ��������
}  

//��������оƬ		  
//�ȴ�ʱ�䳬��...
void W25Q128_Erase_Chip(void)   
{                                   
    W25Q128_Write_Enable();                  //SET WEL 
    W25Q128_Wait_Busy();   
  	SPI_FLASH_NSS_SELECT() ;                           //ʹ������   
    W25Q128_ReadWriteByte(W25X_ChipErase);   //����Ƭ��������  
	SPI_FLASH_NSS_ABANDON();                           //ȡ��Ƭѡ     	      
	W25Q128_Wait_Busy();   				   	//�ȴ�оƬ��������
}


//�ȴ�����
void W25Q128_Wait_Busy(void)   
{   
	while((W25Q128_ReadSR()&0x01)==0x01);   // �ȴ�BUSYλ���
}  

//�������ģʽ
void W25Q128_PowerDown(void)   
{ 
  	SPI_FLASH_NSS_SELECT() ;                            //ʹ������   
    W25Q128_ReadWriteByte(W25X_PowerDown);    //���͵�������  
	SPI_FLASH_NSS_ABANDON();                            //ȡ��Ƭѡ     	      
}   
//���绽��
void W25Q128_WAKEUP(void)   
{  
  	SPI_FLASH_NSS_SELECT() ;                                //ʹ������   
    W25Q128_ReadWriteByte(W25X_ReleasePowerDown);    
	SPI_FLASH_NSS_ABANDON();                                //ȡ��Ƭѡ     	      
} 


//---------------------------------------------------------
//�ͺ�ʶ�� 
//�ɹ�����0 ʧ�ܷ���1
uint8_t W25Q128_TypeTest(void)
{
    uint32_t deviceN=0;
    uint8_t ret=0;
    
    deviceN = W25Q128_ReadID();//IDΪ0XEF17
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
        App_Printf("$$ReadDeviceID=0x%X->δ֪�ͺż�������\r\n",deviceN);
        ret=1;
        break;
    }
       
    return ret;  
}





