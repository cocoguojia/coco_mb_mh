/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "myinclude.h"
#include "sdio.h"
#include "usart.h"

#include "ff.h"
#include "ff_gen_drv.h"
#include "sd_diskio.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

//extern Diskio_drvTypeDef  USER_Driver ;
//extern Diskio_drvTypeDef  SD_Driver ;
extern void MX_SDIO_SD_Init(void);
//extern char USERPath[4];                   /* SD���߼��豸·�� */
extern char SDPath[4];                   /* SD���߼��豸·�� */
FATFS fs;													/* FatFs�ļ�ϵͳ���� */
FIL file;													/* �ļ����� */
FRESULT f_res;                    /* �ļ�������� */
UINT fnum;            					  /* �ļ��ɹ���д���� */
BYTE ReadBuffer[1024]={0};        /* �������� */
BYTE WriteBuffer[]= "Ҽ������cocoguojia test sdio �ļ�ϵͳ\r\n";
	uint32_t resTemp=0;
#define BLOCK_START_ADDR         0     /* Block start address      */
#define NUM_OF_BLOCKS            1   /* Total number of blocks   */
#define BUFFER_WORDS_SIZE        ((BLOCKSIZE * NUM_OF_BLOCKS) >> 2) /* Total data size in bytes */
void ShowFun(HAL_SD_CardStateTypeDef n);
static void printf_fatfs_error(FRESULT fresult);
void SD_Test(void);
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 1024 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for AppPrintfMutex */
osMutexId_t AppPrintfMutexHandle;
const osMutexAttr_t AppPrintfMutex_attributes = {
  .name = "AppPrintfMutex"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */
  /* Create the mutex(es) */
  /* creation of AppPrintfMutex */
  AppPrintfMutexHandle = osMutexNew(&AppPrintfMutex_attributes);

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
 
    while(!(K1ING))
    {
        while(!(K1ING))
        {
               RGBR_EN;
            osDelay(10);
        }
        osDelay(100);
    }
       RGBP_EN;
       osDelay(1000);

         RGBY_EN;
          osDelay(1000);
    App_Printf("POWER ON");
     RGBW_EN;

    SD_Test();
       RGB_ALLOFF;
  for(;;)
  {
   
        LED1_EN;
        osDelay(1000);
        LED1_NO;
        App_Printf("cocoguojia\r\n");
        osDelay(1000);
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void SD_Test(void)
{   
  uint8_t i;    
    printf("��%s\r\n",SDPath);//��ʾһ�� ��ǰ��SDPath������
    
    //��SD�������ļ�ϵͳ���ļ�ϵͳ����ʱ���SD����ʼ��
    f_res = f_mount(&fs,(TCHAR const*)SDPath,1);//��������SDPath�Ϲ����ļ�ϵͳ ����������Ч    
    printf_fatfs_error(f_res);
    
    /*----------------------- ��ʽ������ ---------------------------*/  
    /* ���û���ļ�ϵͳ�͸�ʽ�����������ļ�ϵͳ */
    if(f_res == FR_NO_FILESYSTEM)
    {
      printf("��SD����û���ļ�ϵͳ���������и�ʽ��...\r\n");
      /* ��ʽ�� */
      f_res=f_mkfs((TCHAR const*)SDPath,0,0);							
      
      if(f_res == FR_OK)
      {
        printf("��SD���ѳɹ���ʽ���ļ�ϵͳ��\r\n");
        /* ��ʽ������ȡ������ */
        f_res = f_mount(NULL,(TCHAR const*)SDPath,1);			
        /* ���¹���	*/			
        f_res = f_mount(&fs,(TCHAR const*)SDPath,1);
      }
      else
      {
        printf("������ʽ��ʧ�ܡ�����\r\n");
        while(1);
      }
    }
    else if(f_res!=FR_OK)
    {
      printf("����SD�������ļ�ϵͳʧ�ܡ�(%d)\r\n",f_res);
      printf_fatfs_error(f_res);
      while(1);
    }
    else
    {
      printf("���ļ�ϵͳ���سɹ������Խ��ж�д����\r\n");
    }
  
    osDelay(200);
    /*----------------------- �ļ�ϵͳ���ԣ�д���� -----------------------------*/
    /* ���ļ�������ļ��������򴴽��� */
    printf("****** ���������ļ�д�����... ******\r\n");	
    f_res = f_open(&file, "�ҵ��ļ�001.txt",FA_CREATE_ALWAYS | FA_WRITE );
    if ( f_res == FR_OK )
    {
      printf("����/����FatFs��д�����ļ�.txt�ļ��ɹ������ļ�д�����ݡ�\r\n");
      /* ��ָ���洢������д�뵽�ļ��� */
      f_res=f_write(&file,WriteBuffer,sizeof(WriteBuffer),&fnum);
      if(f_res==FR_OK)
      {
        printf("���ļ�д��ɹ���д���ֽ����ݣ�%d\r\n",fnum);
        printf("�����ļ�д�������Ϊ��\r\n%s\r\n",WriteBuffer);
      }
      else
      {
        printf("�����ļ�д��ʧ�ܣ�(%d)\r\n",f_res);
      }    
      /* ���ٶ�д���ر��ļ� */
      f_close(&file);
    }
    else
    {	
  
      printf("������/�����ļ�ʧ�ܡ�\r\n");
    }
          osDelay(1000);
    /*------------------- �ļ�ϵͳ���ԣ������� ------------------------------------*/
    printf("****** ���������ļ���ȡ����... ******\r\n");
    f_res = f_open(&file, "�ҵ��ļ�001.txt", FA_OPEN_EXISTING | FA_READ); 	 
    if(f_res == FR_OK)
    {
      printf("�����ļ��ɹ���\r\n");
      
      for(i=0;i<=4;i++)
      {
         
          f_lseek(&file, i*2);//���ƫ�Ƶĵ�λ���ֽ� ������ƫ����2���ֽ� ��һ������
          
          f_res = f_read(&file, ReadBuffer, sizeof(ReadBuffer), &fnum); 
          if(f_res==FR_OK)
          {
            printf("���ļ���ȡ�ɹ�,�����ֽ����ݣ�%d\r\n",fnum);
            printf("����ȡ�õ��ļ�����Ϊ��\r\n%s \r\n", ReadBuffer);	
          }
          else
          {
            printf("�����ļ���ȡʧ�ܣ�(%d)\r\n",f_res);
          }	
      }          
    }
    else
    {
     printf_fatfs_error(f_res);
      printf("�������ļ�ʧ�ܡ�\r\n");
    }
    
   
    
    
    /* ���ٶ�д���ر��ļ� */
    f_close(&file);
    
    /* ����ʹ�ã�ȡ������ */
    f_res = f_mount(NULL,(TCHAR const*)SDPath,1);	

  /* ע��һ��FatFS�豸��SD�� */
  FATFS_UnLinkDriver(SDPath);
  
}

/**
  * @brief  ��ӡ�����Ϣ
  * @param  ��
  * @retval ��
  */
static void printf_fatfs_error(FRESULT fresult)
{
  switch(fresult)
  {
    case FR_OK:                   
      printf("�������ɹ���\r\n");
    break;
    case FR_DISK_ERR:             
      printf("����Ӳ�����������������\r\n");
    break;
    case FR_INT_ERR:              
      printf("�������Դ���\r\n");
    break;
    case FR_NOT_READY:            
      printf("���������豸�޷�������\r\n");
    break;
    case FR_NO_FILE:              
      printf("�����޷��ҵ��ļ���\r\n");
    break;
    case FR_NO_PATH:             
      printf("�����޷��ҵ�·����\r\n");
    break;
    case FR_INVALID_NAME:         
      printf("������Ч��·������\r\n");
    break;
    case FR_DENIED:               
    case FR_EXIST:                
      printf("�����ܾ����ʡ�\r\n");
    break;
    case FR_INVALID_OBJECT:       
      printf("������Ч���ļ���·����\r\n");
    break;
    case FR_WRITE_PROTECTED:      
      printf("�����߼��豸д������\r\n");
    break;
    case FR_INVALID_DRIVE:        
      printf("������Ч���߼��豸��\r\n");
    break;
    case FR_NOT_ENABLED:          
      printf("������Ч�Ĺ�������\r\n");
    break;
    case FR_NO_FILESYSTEM:        
      printf("������Ч���ļ�ϵͳ��\r\n");
    break;
    case FR_MKFS_ABORTED:         
      printf("���������������⵼��f_mkfs��������ʧ�ܡ�\r\n");
    break;
    case FR_TIMEOUT:              
      printf("����������ʱ��\r\n");
    break;
    case FR_LOCKED:               
      printf("�����ļ���������\r\n");
    break;
    case FR_NOT_ENOUGH_CORE:      
      printf("�������ļ���֧�ֻ�ȡ�ѿռ�ʧ�ܡ�\r\n");
    break;
    case FR_TOO_MANY_OPEN_FILES:  
      printf("������̫���ļ���\r\n");
    break;
    case FR_INVALID_PARAMETER:    
      printf("����������Ч��\r\n");
    break;
  }
}


/* USER CODE END Application */

