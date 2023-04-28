
  
#include "coco_basics.h"

#define App_Printf_NUM_MAX 253
extern osMutexId_t AppPrintfMutexHandle;;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//全局函数
//----------------------------------------------------------------------------------------------
//线程安全的printf方式
//用互斥信号量保证打印的线程安全性能
void  App_Printf(const char *fmt, ...)
{
    char  buf_str[App_Printf_NUM_MAX + 1];  //App_Printf_NUM_MAX 根据实际需要可以设置更大或者小的值
    va_list   v_args;

    va_start(v_args, fmt);
   (void)vsnprintf((char       *)&buf_str[0],
                   (size_t      ) sizeof(buf_str),
                   (char const *) fmt,
                                  v_args);
    va_end(v_args);

    //##############################################################################################
	//互斥操作
    osMutexAcquire(AppPrintfMutexHandle, osWaitForever);    //一直等待获取互斥资源 
    printf("%s", buf_str);
    osMutexRelease(AppPrintfMutexHandle);                   //释放互斥资源
    //##############################################################################################
}

//----------------------------------------------------------------------------------------------
///重定向c库函数printf到串口，重定向后可使用printf函数
int fputc(int ch, FILE *f)
{
	//HAL_StatusTypeDef returnDat;
	
    //------------------------------------------------------------------------------
    //cocoguojia send
    
    HAL_UART_Transmit(&huart1,(uint8_t*)&ch,1,10);
    
  
    //----------------------------------------------------------------------------------------
    ////这里不能这样 当串口多了的时候 容易互相影响    
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
//线程安全的 打印字符串 函数
//用互斥信号量保证打印的线程安全性能
void  App_PutStr(const char *str)
{

    uint16_t len=strlen(str);
    //##############################################################################################
	//互斥操作
    osMutexAcquire(AppPrintfMutexHandle, osWaitForever);    //一直等待获取互斥资源 
    HAL_UART_Transmit(&huart1,(uint8_t*)str,len,10);
    osMutexRelease(AppPrintfMutexHandle);                   //释放互斥资源
    //############################################################################################## 
}


