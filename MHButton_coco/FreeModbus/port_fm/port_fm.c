 /*
      ����modbus���Ĵ���������ʵ������Ĵ��������ּĴ�������Ȧ����ɢ�������ص�����
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
                               ����Modbus��ز���
******************************************************************************/

USHORT usRegInputStart = REG_INPUT_START;
USHORT usRegInputBuf[REG_INPUT_NREGS];

USHORT usRegHoldingStart = REG_HOLDING_START;
USHORT usRegHoldingBuf[REG_HOLDING_NREGS] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

USHORT usRegCoilsStart = REG_COILS_START;
UCHAR  ucRegCoilsBuf[REG_COILS_SIZE / 8] = {0x98,0x6e};	   //����8����������

USHORT usRegDiscreteStart = REG_DISCRETE_START;
UCHAR  usRegDiscreteBuf[REG_DISCRETE_SIZE / 8]={0xaa,0xfe};	  //��������8������Ҫ��֤һ�£��Ƿ���Ҫ��1�ء�

/**
  *****************************************************************************
  * @Name   : ��������Ĵ���
  *
  * @Brief  : ��Ӧ������0x04 -> eMBFuncReadInputRegister
  *
  * @Input  : *pucRegBuffer�����ݻ���������Ӧ������
  *           usAddress:     �Ĵ�����ַ
  *           usNRegs:       �����Ĵ�������
  *
  * @Output : none
  *
  * @Return : Modbus״̬��Ϣ
  *****************************************************************************
**/
//----------------------------------------------------------------
//0x04 ������Ĵ���  
//����Ĵ��� ��ʼ��ַ    REG_INPUT_START 
//����Ĵ��� ����Ч����  REG_INPUT_NREGS 
//����Ĵ��� β��ַ      REG_INPUT_START + REG_INPUT_NREGS
eMBErrorCode eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
	eMBErrorCode eStatus = MB_ENOERR;
	int          iRegIndex = 0;
	//�жϵ�ַ�Ϸ���
	if ((usAddress >= REG_INPUT_START) && (usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS))
	{
		iRegIndex = (int)(usAddress - usRegInputStart);
		while (usNRegs > 0)
		{
			*pucRegBuffer++ = (UCHAR)( usRegInputBuf[iRegIndex] >> 8);  //��8λ�ֽ�
			*pucRegBuffer++ = (UCHAR)( usRegInputBuf[iRegIndex] & 0xFF); //��8λ�ֽ�
			iRegIndex++;
			usNRegs--;
		}
	}
	else  //�����ַ
	{
		eStatus = MB_ENOREG;
	}

	return eStatus;
}

/**
  *****************************************************************************
  * @Name   : �������ּĴ���
  *
  * @Brief  : ��Ӧ������0x06 -> eMBFuncWriteHoldingRegister
  *                    0x16 -> eMBFuncWriteMultipleHoldingRegister
  *                    0x03 -> eMBFuncReadHoldingRegister
  *                    0x23 -> eMBFuncReadWriteMultipleHoldingRegister
  *
  * @Input  : *pucRegBuffer�����ݻ���������Ӧ������
  *           usAddress:     �Ĵ�����ַ
  *           usNRegs:       �����Ĵ�������
  *           eMode:         ������
  *
  * @Output : none
  *
  * @Return : Modbus״̬��Ϣ
  *****************************************************************************
**/
//----------------------------------------------------------------
//0x06 д�����Ĵ���  
//0x10 д����Ĵ���  
//0x03 �����ּĴ���  
//0x17 ��д����Ĵ���  
//�Ĵ��� ��ʼ��ַ    REG_HOLDING_START 
//�Ĵ��� ����Ч����  REG_HOLDING_NREGS 
//�Ĵ��� β��ַ     REG_HOLDING_START + REG_HOLDING_NREGS
eMBErrorCode eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode )
{
	eMBErrorCode eStatus = MB_ENOERR;
	int          iRegIndex = 0;

	//�жϵ�ַ�Ƿ�Ϸ�
	if((usAddress >= REG_HOLDING_START) && ((usAddress + usNRegs) <= (REG_HOLDING_START + REG_HOLDING_NREGS)))
	{
		iRegIndex = (int)(usAddress - usRegHoldingStart);
		//���ݹ�������в���
		switch(eMode)
		{
			case MB_REG_READ:  //�����ּĴ���  ��д���ּĴ�����Ҫ��֤���ݵ�͸����
                
            //##############################################################################################
            //�������
            osMutexAcquire(ReadWriteRegHoldingMutexHandle, osWaitForever);   //һֱ�ȴ���ȡ������Դ 
            while(usNRegs > 0)
            {
                *pucRegBuffer++ = (uint8_t)(usRegHoldingBuf[iRegIndex] >> 8);   //��8λ�ֽ�
                *pucRegBuffer++ = (uint8_t)(usRegHoldingBuf[iRegIndex] & 0xFF); //��8λ�ֽ�
                iRegIndex++;
                usNRegs--;
            }
            osMutexRelease(ReadWriteRegHoldingMutexHandle);                  //�ͷŻ�����Դ    
            //############################################################################################## 
                    
            break;

			case MB_REG_WRITE:  //д���ּĴ��� ��д���ּĴ�����Ҫ��֤���ݵ�͸����
            g_holdingChangeFlag=1;
            //##############################################################################################
            //�������
            osMutexAcquire(ReadWriteRegHoldingMutexHandle, osWaitForever);   //һֱ�ȴ���ȡ������Դ 
            
            while(usNRegs > 0)
            {
                usRegHoldingBuf[iRegIndex] = *pucRegBuffer++ << 8;  //��8λ�ֽ�
                usRegHoldingBuf[iRegIndex] |= *pucRegBuffer++;      //��8λ�ֽ�
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
            
            osMutexRelease(ReadWriteRegHoldingMutexHandle);                  //�ͷŻ�����Դ    
            //##############################################################################################  
            
            break;
		}
	}
	else  //�����ַ
	{
		eStatus = MB_ENOREG;
	}

	return eStatus;
}

/**
  *****************************************************************************
  * @Name   : ������Ȧ
  *
  * @Brief  : ��Ӧ������0x01 -> eMBFuncReadCoils
  *                    0x05 -> eMBFuncWriteCoil
  *                    0x15 -> д�����Ȧ eMBFuncWriteMultipleCoils
  *
  * @Input  : *pucRegBuffer�����ݻ���������Ӧ������
  *           usAddress:     �Ĵ�����ַ
  *           usNRegs:       �����Ĵ�������
  *           eMode:         ������
  *
  * @Output : none
  *
  * @Return : Modbus״̬��Ϣ
  *****************************************************************************
**/
//0x01 ����Ȧ  
//0x05 д������Ȧ  
//0x0F д�����Ȧ  
//��Ȧ ��ʼ��ַ    REG_COILS_START 
//��Ȧ ����Ч����  REG_COILS_SIZE 
//��Ȧ β��ַ      REG_COILS_START + REG_COILS_SIZE
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
  * @Name   : ������ɢ�Ĵ���
  *
  * @Brief  : ��Ӧ������0x02 -> eMBFuncReadDiscreteInputs
  *
  * @Input  : *pucRegBuffer�����ݻ���������Ӧ������
  *           usAddress:     �Ĵ�����ַ
  *           usNRegs:       �����Ĵ�������
  *
  * @Output : none
  *
  * @Return : Modbus״̬��Ϣ
  *****************************************************************************
**/
//eMBErrorCode eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
//{
//	pucRegBuffer = pucRegBuffer;

//	return MB_ENOREG;
//}

//0x02 ����ɢ����  
//��ɢ���� ��ʼ��ַ    REG_DISCRETE_START 
//��ɢ���� ����Ч����  REG_DISCRETE_SIZE 
//��ɢ���� β��ַ     REG_DISCRETE_START + REG_DISCRETE_SIZE

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

