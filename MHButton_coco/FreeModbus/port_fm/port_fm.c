 /*
      定义modbus各寄存器参数，实现输入寄存器、保持寄存器、线圈、离散数字量回调函数
  */
/* ----------------------- Modbus includes ----------------------------------*/
#include "port_fm.h"
#include "mb.h"
#include "mbport.h"
#include "mbutils.h"

//void EnterCriticalSection(  )
//{
//	__ASM volatile("cpsid i");
//}

//void ExitCriticalSection(  )
//{
//    __ASM volatile("cpsie i");
//}


/******************************************************************************
                               定义Modbus相关参数
******************************************************************************/

USHORT usRegInputStart = REG_INPUT_START;
USHORT usRegInputBuf[REG_INPUT_NREGS];

USHORT usRegHoldingStart = REG_HOLDING_START;
USHORT usRegHoldingBuf[REG_HOLDING_NREGS] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

USHORT usRegCoilsStart = REG_COILS_START;
UCHAR  ucRegCoilsBuf[REG_COILS_SIZE / 8] = {0x98,0x6e};	   //数量8的整数倍。

USHORT usRegDiscreteStart = REG_DISCRETE_START;
UCHAR  usRegDiscreteBuf[REG_DISCRETE_SIZE / 8]={0xaa,0xfe};	  //数量低于8个还需要验证一下，是否需要加1呢。

/**
  *****************************************************************************
  * @Name   : 操作输入寄存器
  *
  * @Brief  : 对应功能码0x04 -> eMBFuncReadInputRegister
  *
  * @Input  : *pucRegBuffer：数据缓冲区，响应主机用
  *           usAddress:     寄存器地址
  *           usNRegs:       操作寄存器个数
  *
  * @Output : none
  *
  * @Return : Modbus状态信息
  *****************************************************************************
**/
//----------------------------------------------------------------
//0x04 读输入寄存器  
//输入寄存器 起始地址    REG_INPUT_START 
//输入寄存器 总有效个数  REG_INPUT_NREGS 
//输入寄存器 尾地址      REG_INPUT_START + REG_INPUT_NREGS
eMBErrorCode eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
	eMBErrorCode eStatus = MB_ENOERR;
	int          iRegIndex = 0;
	//判断地址合法性
	if ((usAddress >= REG_INPUT_START) && (usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS))
	{
		iRegIndex = (int)(usAddress - usRegInputStart);
		while (usNRegs > 0)
		{
			*pucRegBuffer++ = (UCHAR)( usRegInputBuf[iRegIndex] >> 8);  //高8位字节
			*pucRegBuffer++ = (UCHAR)( usRegInputBuf[iRegIndex] & 0xFF); //低8位字节
			iRegIndex++;
			usNRegs--;
		}
	}
	else  //错误地址
	{
		eStatus = MB_ENOREG;
	}

	return eStatus;
}

/**
  *****************************************************************************
  * @Name   : 操作保持寄存器
  *
  * @Brief  : 对应功能码0x06 -> eMBFuncWriteHoldingRegister
  *                    0x16 -> eMBFuncWriteMultipleHoldingRegister
  *                    0x03 -> eMBFuncReadHoldingRegister
  *                    0x23 -> eMBFuncReadWriteMultipleHoldingRegister
  *
  * @Input  : *pucRegBuffer：数据缓冲区，响应主机用
  *           usAddress:     寄存器地址
  *           usNRegs:       操作寄存器个数
  *           eMode:         功能码
  *
  * @Output : none
  *
  * @Return : Modbus状态信息
  *****************************************************************************
**/
//----------------------------------------------------------------
//0x06 写单个寄存器  
//0x10 写多个寄存器  
//0x03 读保持寄存器  
//0x17 读写多个寄存器  
//寄存器 起始地址    REG_HOLDING_START 
//寄存器 总有效个数  REG_HOLDING_NREGS 
//寄存器 尾地址     REG_HOLDING_START + REG_HOLDING_NREGS
eMBErrorCode eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode )
{
	eMBErrorCode eStatus = MB_ENOERR;
	int          iRegIndex = 0;

	//判断地址是否合法
	if((usAddress >= REG_HOLDING_START) && ((usAddress + usNRegs) <= (REG_HOLDING_START + REG_HOLDING_NREGS)))
	{
		iRegIndex = (int)(usAddress - usRegHoldingStart);
		//根据功能码进行操作
		switch(eMode)
		{
			case MB_REG_READ:  //读保持寄存器  读写保持寄存器都要保证数据的透明性
                
            //##############################################################################################
            //互斥操作
            osMutexAcquire(ReadWriteRegHoldingMutexHandle, osWaitForever);   //一直等待获取互斥资源 
            while(usNRegs > 0)
            {
                *pucRegBuffer++ = (uint8_t)(usRegHoldingBuf[iRegIndex] >> 8);   //高8位字节
                *pucRegBuffer++ = (uint8_t)(usRegHoldingBuf[iRegIndex] & 0xFF); //低8位字节
                iRegIndex++;
                usNRegs--;
            }
            osMutexRelease(ReadWriteRegHoldingMutexHandle);                  //释放互斥资源    
            //############################################################################################## 
                    
            break;

			case MB_REG_WRITE:  //写保持寄存器 读写保持寄存器都要保证数据的透明性
            g_holdingChangeFlag=1;
            //##############################################################################################
            //互斥操作
            osMutexAcquire(ReadWriteRegHoldingMutexHandle, osWaitForever);   //一直等待获取互斥资源 
            
            while(usNRegs > 0)
            {
                usRegHoldingBuf[iRegIndex] = *pucRegBuffer++ << 8;  //高8位字节
                usRegHoldingBuf[iRegIndex] |= *pucRegBuffer++;      //低8位字节
                if(4>=iRegIndex)
                {
                     g_keyStateTalbe[iRegIndex]=usRegHoldingBuf[iRegIndex];
                }
                else if(7>=iRegIndex)
                {
                    g_diNumTalbe[iRegIndex-4]=usRegHoldingBuf[iRegIndex];
                }
                else if(12>=iRegIndex)
                {
                    g_keyTimeTalbe[iRegIndex-7]=usRegHoldingBuf[iRegIndex];
                }
                iRegIndex++;
                usNRegs--;
            }
            
            osMutexRelease(ReadWriteRegHoldingMutexHandle);                  //释放互斥资源    
            //##############################################################################################  
            
            break;
		}
	}
	else  //错误地址
	{
		eStatus = MB_ENOREG;
	}

	return eStatus;
}

/**
  *****************************************************************************
  * @Name   : 操作线圈
  *
  * @Brief  : 对应功能码0x01 -> eMBFuncReadCoils
  *                    0x05 -> eMBFuncWriteCoil
  *                    0x15 -> 写多个线圈 eMBFuncWriteMultipleCoils
  *
  * @Input  : *pucRegBuffer：数据缓冲区，响应主机用
  *           usAddress:     寄存器地址
  *           usNRegs:       操作寄存器个数
  *           eMode:         功能码
  *
  * @Output : none
  *
  * @Return : Modbus状态信息
  *****************************************************************************
**/
//0x01 读线圈  
//0x05 写单个线圈  
//0x0F 写多个线圈  
//线圈 起始地址    REG_COILS_START 
//线圈 总有效个数  REG_COILS_SIZE 
//线圈 尾地址      REG_COILS_START + REG_COILS_SIZE
eMBErrorCode eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils, eMBRegisterMode eMode )
{
	eMBErrorCode	eStatus = MB_ENOERR;
	int 			iNCoils = ( int )usNCoils;
	unsigned short	usBitOffset;

	/* Check if we have registers mapped at this block. */
	if( ( usAddress >= REG_COILS_START ) &&	( usAddress + usNCoils <= REG_COILS_START + REG_COILS_SIZE ) )
	{
		usBitOffset = ( unsigned short )( usAddress - REG_COILS_START );
		switch ( eMode )
		{
			/* Read current values and pass to protocol stack. */
			case MB_REG_READ:
			while( iNCoils > 0 )
			{
				*pucRegBuffer++ = xMBUtilGetBits( ucRegCoilsBuf, usBitOffset,	( unsigned char )( iNCoils > 8 ? 8 : iNCoils ) );
				iNCoils -= 8;
				usBitOffset += 8;
			}
			break;

				/* Update current register values. */
			case MB_REG_WRITE:
			while( iNCoils > 0 )
			{
				xMBUtilSetBits( ucRegCoilsBuf, usBitOffset,	( unsigned char )( iNCoils > 8 ? 8 : iNCoils ),	*pucRegBuffer++ );
				iNCoils -= 8;
				usBitOffset += 8;
			}
			break;
		}

	}
	else
	{
		eStatus = MB_ENOREG;
	}
	return eStatus;
}


/**
  *****************************************************************************
  * @Name   : 操作离散寄存器
  *
  * @Brief  : 对应功能码0x02 -> eMBFuncReadDiscreteInputs
  *
  * @Input  : *pucRegBuffer：数据缓冲区，响应主机用
  *           usAddress:     寄存器地址
  *           usNRegs:       操作寄存器个数
  *
  * @Output : none
  *
  * @Return : Modbus状态信息
  *****************************************************************************
**/
//eMBErrorCode eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
//{
//	pucRegBuffer = pucRegBuffer;

//	return MB_ENOREG;
//}

//0x02 读离散输入  
//离散输入 起始地址    REG_DISCRETE_START 
//离散输入 总有效个数  REG_DISCRETE_SIZE 
//离散输入 尾地址     REG_DISCRETE_START + REG_DISCRETE_SIZE

eMBErrorCode
eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    short           iNDiscrete = ( short )usNDiscrete;
    USHORT  usBitOffset;

    /* Check if we have registers mapped at this  block. */
    if( ( usAddress >= REG_DISCRETE_START ) && ( usAddress + usNDiscrete <= REG_DISCRETE_START + REG_DISCRETE_SIZE ) )
    {
        usBitOffset = ( USHORT )( usAddress - REG_DISCRETE_START );
        while( iNDiscrete > 0 )
        {
            *pucRegBuffer++ =
            xMBUtilGetBits( usRegDiscreteBuf, usBitOffset,( UCHAR )( iNDiscrete > 8 ? 8 : iNDiscrete ) );
            iNDiscrete -= 8;
            usBitOffset += 8;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}

