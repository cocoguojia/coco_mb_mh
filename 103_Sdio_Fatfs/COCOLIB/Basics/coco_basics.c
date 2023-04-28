
  
#include "coco_basics.h"

#define App_Printf_NUM_MAX 253
extern osMutexId_t AppPrintfMutexHandle;;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//ȫ�ֺ���
//----------------------------------------------------------------------------------------------
//�̰߳�ȫ��printf��ʽ
//�û����ź�����֤��ӡ���̰߳�ȫ����
void  App_Printf(const char *fmt, ...)
{
    char  buf_str[App_Printf_NUM_MAX + 1];  //App_Printf_NUM_MAX ����ʵ����Ҫ�������ø������С��ֵ
    va_list   v_args;

    va_start(v_args, fmt);
   (void)vsnprintf((char       *)&buf_str[0],
                   (size_t      ) sizeof(buf_str),
                   (char const *) fmt,
                                  v_args);
    va_end(v_args);

    //##############################################################################################
	//�������
    osMutexAcquire(AppPrintfMutexHandle, osWaitForever);    //һֱ�ȴ���ȡ������Դ 
    printf("%s", buf_str);
    osMutexRelease(AppPrintfMutexHandle);                   //�ͷŻ�����Դ
    //##############################################################################################
}

//----------------------------------------------------------------------------------------------
///�ض���c�⺯��printf�����ڣ��ض�����ʹ��printf����
int fputc(int ch, FILE *f)
{
	//HAL_StatusTypeDef returnDat;
	
    //------------------------------------------------------------------------------
    //cocoguojia send
    
    HAL_UART_Transmit(&huart1,(uint8_t*)&ch,1,10);
    
  
    //----------------------------------------------------------------------------------------
    ////���ﲻ������ �����ڶ��˵�ʱ�� ���׻���Ӱ��    
    //returnDat=HAL_UART_Transmit(&huart1,(uint8_t*)&ch,1,10);
    //do
    //{
    //returnDat=HAL_UART_Transmit(&huart1,(uint8_t*)&ch,1,10);
    //if(HAL_OK!=returnDat)
    //{
    ////osDelay(10);
    //}
    //}
    //while(HAL_OK!=returnDat);
		
	return (ch);
}


//----------------------------------------------------------------------------------------------
//�̰߳�ȫ�� ��ӡ�ַ��� ����
//�û����ź�����֤��ӡ���̰߳�ȫ����
void  App_PutStr(const char *str)
{

    uint16_t len=strlen(str);
    //##############################################################################################
	//�������
    osMutexAcquire(AppPrintfMutexHandle, osWaitForever);    //һֱ�ȴ���ȡ������Դ 
    HAL_UART_Transmit(&huart1,(uint8_t*)str,len,10);
    osMutexRelease(AppPrintfMutexHandle);                   //�ͷŻ�����Դ
    //############################################################################################## 
}


