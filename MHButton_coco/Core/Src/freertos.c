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
#include "BL8025.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */


//----------------------------------------------------------------------
//如果我们想看实时剩余堆多大 那么就使能这个 注意这个只在调试的时候使能
//#define  LOOK_FREEMEMORY 1


typedef enum { FAILED = 0, PASSED = !FAILED} TestStatus;

/* 发送缓冲区初始化 */
uint8_t Tx_Buffer[] = "firestm325345gh\r\n";

//TIME_T setTimerStruct={0,15,14,1,3,4,23};


extern TIME_T g_justTimerStruct;
extern TIME_T g_wantSetTimerStruct;



//读取的ID存储位置
__IO uint32_t DeviceID = 0;
__IO uint32_t FlashID = 0;
__IO TestStatus TransferStatus1 = FAILED;


/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
void PowerOnAfterReadParameter(void);
void TestClockClear_1000ms(void);
void PowerOnTestDoWith(void);
void readMcuIdForMac(void);
void ClearParameter(void);
//void RemoteIpAndPortShow(void);
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
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for netBridgeModbusQueue */
osMessageQueueId_t netBridgeModbusQueueHandle;
const osMessageQueueAttr_t netBridgeModbusQueue_attributes = {
  .name = "netBridgeModbusQueue"
};
/* Definitions for wifiSendQueue */
osMessageQueueId_t wifiSendQueueHandle;
const osMessageQueueAttr_t wifiSendQueue_attributes = {
  .name = "wifiSendQueue"
};
/* Definitions for w5500SendQueue */
osMessageQueueId_t w5500SendQueueHandle;
const osMessageQueueAttr_t w5500SendQueue_attributes = {
  .name = "w5500SendQueue"
};
/* Definitions for AppPrintfMutex */
osMutexId_t AppPrintfMutexHandle;
const osMutexAttr_t AppPrintfMutex_attributes = {
  .name = "AppPrintfMutex"
};
/* Definitions for ReadWriteRegHoldingMutex */
osMutexId_t ReadWriteRegHoldingMutexHandle;
const osMutexAttr_t ReadWriteRegHoldingMutex_attributes = {
  .name = "ReadWriteRegHoldingMutex"
};
/* Definitions for PendingCachePointsMutex */
osMutexId_t PendingCachePointsMutexHandle;
const osMutexAttr_t PendingCachePointsMutex_attributes = {
  .name = "PendingCachePointsMutex"
};
/* Definitions for W25q64Mutex */
osMutexId_t W25q64MutexHandle;
const osMutexAttr_t W25q64Mutex_attributes = {
  .name = "W25q64Mutex"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
uint8_t read_buffU8[16];
uint16_t read_buffU16[8];
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

  /* creation of ReadWriteRegHoldingMutex */
  ReadWriteRegHoldingMutexHandle = osMutexNew(&ReadWriteRegHoldingMutex_attributes);

  /* creation of PendingCachePointsMutex */
  PendingCachePointsMutexHandle = osMutexNew(&PendingCachePointsMutex_attributes);

  /* creation of W25q64Mutex */
  W25q64MutexHandle = osMutexNew(&W25q64Mutex_attributes);

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of netBridgeModbusQueue */
  netBridgeModbusQueueHandle = osMessageQueueNew (8, sizeof(uint16_t), &netBridgeModbusQueue_attributes);

  /* creation of wifiSendQueue */
  wifiSendQueueHandle = osMessageQueueNew (16, sizeof(uint16_t), &wifiSendQueue_attributes);

  /* creation of w5500SendQueue */
  w5500SendQueueHandle = osMessageQueueNew (16, sizeof(uint16_t), &w5500SendQueue_attributes);

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
    uint8_t weekTemp=0;
    uint8_t clockErr=0;//1=时钟芯片错误
    uint8_t clockErrShowN=0;
    
    #ifdef  LOOK_FREEMEMORY 
    size_t freememory; 
    uint8_t n=27;
    #endif
    
    osDelay(500);

 
    PowerOnTestDoWith();                //上电判断主电是否上电 采取相应的策略 函数
    
    readMcuIdForMac();
//    RemoteIpAndPortShow();

    PowerOnAfterReadParameter();       //读取参数如果是0xffff则设置为0 参数 我们这里只指 按键按下时间和ABC三路上升沿次数
    
    clockErr=bsp_InitRtc();
    
    App_Printf("$$ Start User Task\r\n");
    COCO_MX_FREERTOS_Init();            //启动其他用户任务
            

  for(;;)
  {
      HAL_GPIO_WritePin(LEDB_GPIO_Port, LEDB_Pin, GPIO_PIN_RESET);
      osDelay(500);
      HAL_GPIO_WritePin(LEDB_GPIO_Port, LEDB_Pin, GPIO_PIN_SET);
      osDelay(500);  

      //----------------------------------------------------------------------
      //看我们分配的总堆栈(configTOTAL_HEAP_SIZE)还剩下多少没用 单位字节  
      #ifdef  LOOK_FREEMEMORY 
      n++;
      if(30<=n)
      {
          freememory= xPortGetFreeHeapSize(); 
          App_Printf("$$###### FreeMemory=%d ######\r\n",freememory);
          n=0;
      }
      #endif      
     
      if(0==clockErr)
      {
           if(1==g_writeRX8025TimerFlag)
           {
               g_writeRX8025TimerFlag=0;
               bsp_SetRtcTime(&g_wantSetTimerStruct);
               osDelay(100);
               bsp_GetRtcTime(&g_justTimerStruct);
               weekTemp=bsp_ReadRtcWeekChange(&g_justTimerStruct);
               App_Printf("$$ year=20%d,month=%d,day=%d\r\n",g_justTimerStruct.year,g_justTimerStruct.month,g_justTimerStruct.day);
               App_Printf("$$ H=%d,M=%d,S=%d,Week=%d\r\n",g_justTimerStruct.hour,g_justTimerStruct.minute,g_justTimerStruct.second,weekTemp);
           }              
           else
           {
               TestClockClear_1000ms();  //判断是否过了零点 过零点则清零寄存器  
           }
        
      }
      else
      {
          if(0==clockErrShowN)
          {
            App_Printf("$$ Bl8025 is Err...\r\n");
         
          }
          clockErrShowN++;
          if(10<=clockErrShowN)
          {
            clockErrShowN=0;
          }
      }         
      osDelay(1);  
      
    //App_Printf("$$ sw1=%d,sw2=%d,sw3=%d,sw4=%d,sw5=%d\r\n",g_keyStateTalbe[1],g_keyStateTalbe[2],g_keyStateTalbe[3],g_keyStateTalbe[4],g_keyStateTalbe[5]);
    //App_Printf("$$ DIA=%d,DIB=%d,DIC=%d\r\n",g_diNumTalbe[1],g_diNumTalbe[2],g_diNumTalbe[3]);
    //App_Printf("$$ swT1=%d,swT2=%d,swT3=%d,swT4=%d,swT5=%d\r\n",g_keyTimeTalbe[1],g_keyTimeTalbe[2],g_keyTimeTalbe[3],g_keyTimeTalbe[4],g_keyTimeTalbe[5]);

  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

//-------------------------------------------------------------------
//读mcu全球ID ID有12字节 我们用其后6个字节 来赋值给 W5500 的MAC
void readMcuIdForMac(void)
{
    uint32_t temp32=0;
    temp32=HAL_GetUIDw1();
    g_w5500_mac[0]=(uint8_t)((temp32&0x0000ff00)>>8);
    g_w5500_mac[1]=(uint8_t)((temp32&0x000000ff));
    temp32=HAL_GetUIDw2();
    g_w5500_mac[2]=(uint8_t)((temp32&0xff000000)>>24);
    g_w5500_mac[3]=(uint8_t)((temp32&0x00ff0000)>>16);
    g_w5500_mac[4]=(uint8_t)((temp32&0x0000ff00)>>8);
    g_w5500_mac[5]=(uint8_t)((temp32&0x000000ff));
    App_Printf("\r\n$$*******************************************************\r\n");
    App_Printf("$$###Read Fixed Parameter From W25Q128 ###\r\n");
    App_Printf("$$W5500 MAC=%02X-%02X-%02X-%02X-%02X-%02X\r\n",g_w5500_mac[0],g_w5500_mac[1],g_w5500_mac[2],g_w5500_mac[3],g_w5500_mac[4],g_w5500_mac[5]);
}

////-------------------------------------------------------------------
////网口 写在程序里的 远端IP和端口
//void RemoteIpAndPortShow(void)
//{
//    App_Printf("$$W5500 remote_ip=%d.%d.%d.%d\r\n",g_w5500_remote_ip[0],g_w5500_remote_ip[1],g_w5500_remote_ip[2],g_w5500_remote_ip[3]);
//    App_Printf("$$W5500 remote_port=%d\r\n",g_w5500_remote_port);
//}


//------------------------------------------------------------------
//上电判断主电是否上电 采取相应的策略 函数
void PowerOnTestDoWith(void)
{
    uint8_t i=6;
    
    #ifndef POWER_BATTERY_MANAGEMENT
    //-----------------------------------------------------------------------------------
    //复位后，如果检测 系统主电还没上电 则原地踏步等待主电上电  
    if(GPIO_PIN_SET==HAL_GPIO_ReadPin(POWER_CHECK_GPIO_Port,POWER_CHECK_Pin))
    {
       osDelay(1);
       if(GPIO_PIN_SET==HAL_GPIO_ReadPin(POWER_CHECK_GPIO_Port,POWER_CHECK_Pin))
       {
//           App_Printf("$$ POWEN DOWN,LET/'s marking time...\r\n");
           while(GPIO_PIN_SET==HAL_GPIO_ReadPin(POWER_CHECK_GPIO_Port,POWER_CHECK_Pin))
           {
               while(GPIO_PIN_SET==HAL_GPIO_ReadPin(POWER_CHECK_GPIO_Port,POWER_CHECK_Pin))
               {
                    //-----------------------------------------------------------------------------------
                    //原地踏步小灯指示 每2s 短闪烁一次  
                    HAL_GPIO_WritePin(LEDB_GPIO_Port, LEDB_Pin, GPIO_PIN_RESET);
                    osDelay(100);
                    HAL_GPIO_WritePin(LEDB_GPIO_Port, LEDB_Pin, GPIO_PIN_SET);
                    i=0;
                    while(200>=i)
                    {
                        i++;
                        if(GPIO_PIN_SET==HAL_GPIO_ReadPin(POWER_CHECK_GPIO_Port,POWER_CHECK_Pin))
                        {
                            osDelay(10);
                        }
                        else
                        {
                            i=220;
                        }
                    }
               }
               osDelay(100);
           }
           osDelay(100);
                
           //-----------------------------------------------------------------------------------
           //主电上电 系统复位 
           App_Printf("$$ Let MCU Rsset\r\n");
           
           taskENTER_CRITICAL();
           HAL_NVIC_SystemReset();
           taskEXIT_CRITICAL();
       }
    }
    #endif

    App_Printf("$$ Power on\r\n");
    //-----------------------------------------------------------------------------------
    //主电上电 小灯强亮 快速闪烁6次 
    i=6;
    while(i--)
    {
        HAL_GPIO_WritePin(LEDB_GPIO_Port, LEDB_Pin, GPIO_PIN_RESET);
        osDelay(40);
        HAL_GPIO_WritePin(LEDB_GPIO_Port, LEDB_Pin, GPIO_PIN_SET);
        osDelay(15);
    }
   
}
           

//------------------------------------------------------------------
//参数 我们这里只指 按键按下时间和ABC三路上升沿次数
//0=读取参数如果是0xffff则设置为0
//1=所有参数都设置为0
//readOutOrWrite0Flag  0=读出  1=全部写0
void PowerOnAfterReadParameter(void)
{

    uint8_t m;   
    uint8_t clearFlag=0;//1=发生了断电 读完参后 我清零 
    uint8_t table[20];
 
    //##############################################################################################
    //互斥操作
    osMutexAcquire(W25q64MutexHandle, osWaitForever);   //一直等待获取互斥资源 
    W25Q128_Read(&table[0],WQ_PAGE_DEFAULT,1);
    osMutexRelease(W25q64MutexHandle);                  //释放互斥资源    
    //##############################################################################################
    if(FIRST_PARAAMETER==table[0])
    {   
        App_Printf("\r\n$$------------------------------------------------\r\n"); 
        App_Printf("$$###Read Parameter From W25Q128 ###\r\n");
        //##############################################################################################
        //互斥操作
        osMutexAcquire(W25q64MutexHandle, osWaitForever);   //一直等待获取互斥资源 
        W25Q128_TypeTest();
        W25Q128_Read(&read_buffU8[0],WQ_PAGE_SW5ROAD3,16);
        osMutexRelease(W25q64MutexHandle);                  //释放互斥资源    
        //##############################################################################################
         
        for(m=0;m<16;)
        {
            read_buffU16[m/2]=read_buffU8[m];
            read_buffU16[m/2]<<=8;
            m++;
            read_buffU16[m/2]+=read_buffU8[m];
            if(0xffff==read_buffU16[m/2])
            {
                App_Printf("$$ read_buffU16[%d] clear 0 \r\n",m/2);
                read_buffU16[m/2]=0;
            }
            else
            {
                clearFlag=1;
            }
            m++;            
        }
        
        if(1==clearFlag)
        {
            clearFlag=0;
            //##############################################################################################
            //互斥操作
            osMutexAcquire(W25q64MutexHandle, osWaitForever);   //一直等待获取互斥资源        
            W25Q128_Erase_Sector(WQ_PAGE_SW5ROAD3);  
            osMutexRelease(W25q64MutexHandle);                  //释放互斥资源    
            //##############################################################################################
        }
        
        App_Printf("$$ ABC:\r\n");
        for(m=0;m<3;m++)
        {
            App_Printf("0x%04X-",read_buffU16[m]);
        }
        App_Printf("\r\n");
        
        App_Printf("$$ sw timer:\r\n");
         for(m=3;m<8;m++)
        {
            App_Printf("0x%04X-",read_buffU16[m]);
        }
        App_Printf("\r\n");
        
       
        //##############################################################################################
        //互斥操作
        osMutexAcquire(ReadWriteRegHoldingMutexHandle, osWaitForever);   //一直等待获取互斥资源   
        for(m=0;m<3;m++)
        {
            g_diNumTalbe[m+1]=read_buffU16[m];
            usRegHoldingBuf[m+5]=read_buffU16[m];
        }
        for(m=3;m<8;m++)
        {
            g_keyTimeTalbe[m-2]=read_buffU16[m];
            usRegHoldingBuf[m+5]=read_buffU16[m];
        }    
        osMutexRelease(ReadWriteRegHoldingMutexHandle);                  //释放互斥资源       
        //############################################################################################## 

        //##############################################################################################
        //互斥操作
        osMutexAcquire(W25q64MutexHandle, osWaitForever);   //一直等待获取互斥资源 
        W25Q128_Read(&table[0],WQ_PAGE_W5500_IP,4);
        for(m=0;m<4;m++)
        {
            g_w5500_local_ip[m]=table[m];
        }
       
        W25Q128_Read(&table[0],WQ_PAGE_W5500_SN,4);
        for(m=0;m<4;m++)
        {
            g_w5500_subnet[m]=table[m];
        }

        W25Q128_Read(&table[0],WQ_PAGE_W5500_GW,4);
        for(m=0;m<4;m++)
        {
            g_w5500_gateway[m]=table[m];
        }
     
        W25Q128_Read(&table[0],WQ_PAGE_W5500_ENNO,1);
        if(0==table[0])
        {
            g_w5500EnFlag=0;
        }
        else
        {
            g_w5500EnFlag=1;
        }
        
        //---------------------------------------------------
        //wifi apid 不超过8位
        W25Q128_Read(&table[0],WQ_PAGE_WIFI_APID,9);
        for(m=0;m<9;m++)
        {
            if(0xff==table[m])
            {
                table[m]=0;
            }
        }
        table[8]=0;
        strcpy(g_wifi_ap_sidd_temp,(const char*)&table[0]);
        
        //---------------------------------------------------
        //wifi apkey 等于8位
        W25Q128_Read(&table[0],WQ_PAGE_WIFI_APKEY,9);
        for(m=0;m<9;m++)
        {
            if(0xff==table[m])
            {
                table[m]=0;
            }
        }
        table[8]=0;
        strcpy(g_wifi_ap_password_temp,(const char*)&table[0]);
        
        W25Q128_Read(&table[0],WQ_PAGE_WIFI_AP_IP,4);
        for(m=0;m<4;m++)
        {
            g_wifi_ap_ip_temp[m]=table[m];
        }
       
        W25Q128_Read(&table[0],WQ_PAGE_WIFI_AP_SN,4);
        for(m=0;m<4;m++)
        {
            g_wifi_ap_sn_temp[m]=table[m];
        }

        W25Q128_Read(&table[0],WQ_PAGE_WIFI_AP_GW,4);
        for(m=0;m<4;m++)
        {
            g_wifi_ap_gw_temp[m]=table[m];
        }
        
        //---------------------------------------------------
        //wifi staid 不超过12位
        W25Q128_Read(&table[0],WQ_PAGE_WIFI_STASSID,13);
        for(m=0;m<12;m++)
        {
            if(0xff==table[m])
            {
                table[m]=0;
            }
        }
        table[12]=0;
        strcpy(g_wifi_sta_sidd_temp,(const char*)&table[0]);
        
        //---------------------------------------------------
        //wifi stakey 不超过12位
        W25Q128_Read(&table[0],WQ_PAGE_WIFI_STAKEY,13);
        for(m=0;m<12;m++)
        {
            if(0xff==table[m])
            {
                table[m]=0;
            }
        }
        table[12]=0;
        strcpy(g_wifi_sta_password_temp,(const char*)&table[0]);
        
        W25Q128_Read(&table[0],WQ_PAGE_WIFI_STA_IP,4);
        for(m=0;m<4;m++)
        {
            g_wifi_sta_ip_temp[m]=table[m];
        }
       
        W25Q128_Read(&table[0],WQ_PAGE_WIFI_STA_SN,4);
        for(m=0;m<4;m++)
        {
            g_wifi_sta_sn_temp[m]=table[m];
        }

        W25Q128_Read(&table[0],WQ_PAGE_WIFI_STA_GW,4);
        for(m=0;m<4;m++)
        {
            g_wifi_sta_gw_temp[m]=table[m];
        }
        
//        //---------------------------------------------------
//        //wifi reip
//        W25Q128_Read(&table[0],WQ_PAGE_WIFI_REIP,4);
//        for(m=0;m<4;m++)
//        {
//            g_wifi_sta_sever_ip_temp[m]=table[m];
//        }
        
        //---------------------------------------------------
        //wifi  report不超过5位
        W25Q128_Read(&table[0],WQ_PAGE_WIFI_REPORT,6);
        for(m=0;m<5;m++)
        {
            if(0xff==table[m])
            {
                table[m]=0;
            }
        }
        table[5]=0;
        strcpy(g_wifi_sta_sever_port_temp,(const char*)&table[0]);
        
        
        
        W25Q128_Read(&table[0],WQ_PAGE_WIFI_ENNO,1);
        if(0==table[0])
        {
            g_wifiEnFlag=0;
        }
        else
        {
            g_wifiEnFlag=1;
        }
        
        
        osMutexRelease(W25q64MutexHandle);                  //释放互斥资源    
        //############################################################################################## 
        
        if((0==g_w5500EnFlag)&&(0==g_wifiEnFlag))
        {
            g_w5500EnFlag=1;
        }
    }
    else
    {
        App_Printf("\r\n$$------------------------------------------------\r\n");
        App_Printf("$$###First Wirst Default Parameter For W25Q128 ###\r\n");  
        g_w5500_local_ip[0]  =192;
        g_w5500_local_ip[1]  =168;
        g_w5500_local_ip[2]  =0;
        g_w5500_local_ip[3]  =88;
        
        g_w5500_subnet[0]=255;
        g_w5500_subnet[1]=255;
        g_w5500_subnet[2]=255;
        g_w5500_subnet[3]=0;
        
        g_w5500_gateway[0]=192;
        g_w5500_gateway[1]=168;
        g_w5500_gateway[2]=0;
        g_w5500_gateway[3]=1;
        
        g_w5500EnFlag=1;
        
     
        strcpy(g_wifi_ap_sidd_temp,"MHAUTO_6");
        strcpy(g_wifi_ap_password_temp,"12345678");
        g_wifi_ap_ip_temp[0]=192;
        g_wifi_ap_ip_temp[1]=168;
        g_wifi_ap_ip_temp[2]=1;
        g_wifi_ap_ip_temp[3]=1;
        
        g_wifi_ap_sn_temp[0]=255;
        g_wifi_ap_sn_temp[1]=255;
        g_wifi_ap_sn_temp[2]=255;
        g_wifi_ap_sn_temp[3]=0;
        
        g_wifi_ap_gw_temp[0]=192;
        g_wifi_ap_gw_temp[1]=168;
        g_wifi_ap_gw_temp[2]=1;
        g_wifi_ap_gw_temp[3]=1;

        strcpy(g_wifi_sta_sidd_temp,"HONOR990");
        strcpy(g_wifi_sta_password_temp,"123400789");
        g_wifi_sta_ip_temp[0]=192;
        g_wifi_sta_ip_temp[1]=168;
        g_wifi_sta_ip_temp[2]=43;
        g_wifi_sta_ip_temp[3]=2;
        
        g_wifi_sta_sn_temp[0]=255;
        g_wifi_sta_sn_temp[1]=255;
        g_wifi_sta_sn_temp[2]=255;
        g_wifi_sta_sn_temp[3]=0;
        
        g_wifi_sta_gw_temp[0]=192;
        g_wifi_sta_gw_temp[1]=168;
        g_wifi_sta_gw_temp[2]=43;
        g_wifi_sta_gw_temp[3]=1;
        
//        g_wifi_sta_sever_ip_temp[0]=192;
//        g_wifi_sta_sever_ip_temp[1]=168;
//        g_wifi_sta_sever_ip_temp[2]=43;
//        g_wifi_sta_sever_ip_temp[3]=52;
        strcpy(g_wifi_sta_sever_port_temp,"6800");
        
        g_wifiEnFlag=1;
        
        //##############################################################################################
        //互斥操作
        osMutexAcquire(W25q64MutexHandle, osWaitForever);   //一直等待获取互斥资源 
        
        W25Q128_Erase_Sector(WQ_PAGE_W5500_IP);  
        W25Q128_Write(&g_w5500_local_ip[0],WQ_PAGE_W5500_IP,4); 
        
        W25Q128_Erase_Sector(WQ_PAGE_W5500_SN);  
        W25Q128_Write(&g_w5500_subnet[0],WQ_PAGE_W5500_SN,4); 
        
        W25Q128_Erase_Sector(WQ_PAGE_W5500_GW);  
        W25Q128_Write(&g_w5500_gateway[0],WQ_PAGE_W5500_GW,4); 
        
        W25Q128_Erase_Sector(WQ_PAGE_W5500_ENNO);  
        W25Q128_Write(&g_w5500EnFlag,WQ_PAGE_W5500_ENNO,g_w5500EnFlag); 
        

        W25Q128_Erase_Sector(WQ_PAGE_WIFI_APID);  
        W25Q128_Write((uint8_t*)&g_wifi_ap_sidd_temp[0],WQ_PAGE_WIFI_APID,strlen(&g_wifi_ap_sidd_temp[0])+1); 
        W25Q128_Erase_Sector(WQ_PAGE_WIFI_APKEY);  
        W25Q128_Write((uint8_t*)&g_wifi_ap_password_temp[0],WQ_PAGE_WIFI_APKEY,strlen(&g_wifi_ap_password_temp[0])+1);  
        W25Q128_Erase_Sector(WQ_PAGE_WIFI_AP_IP);  
        W25Q128_Write((uint8_t*)&g_wifi_ap_ip_temp[0],WQ_PAGE_WIFI_AP_IP,4); 
        W25Q128_Erase_Sector(WQ_PAGE_WIFI_AP_SN);  
        W25Q128_Write((uint8_t*)&g_wifi_ap_sn_temp[0],WQ_PAGE_WIFI_AP_SN,4); 
        W25Q128_Erase_Sector(WQ_PAGE_WIFI_AP_GW);  
        W25Q128_Write((uint8_t*)&g_wifi_ap_gw_temp[0],WQ_PAGE_WIFI_AP_GW,4); 

        W25Q128_Erase_Sector(WQ_PAGE_WIFI_STASSID);  
        W25Q128_Write((uint8_t*)&g_wifi_sta_sidd_temp[0],WQ_PAGE_WIFI_STASSID,strlen(&g_wifi_sta_sidd_temp[0])+1); 
        W25Q128_Erase_Sector(WQ_PAGE_WIFI_STAKEY);  
        W25Q128_Write((uint8_t*)&g_wifi_sta_password_temp[0],WQ_PAGE_WIFI_STAKEY,strlen(&g_wifi_sta_password_temp[0])+1); 

        W25Q128_Erase_Sector(WQ_PAGE_WIFI_STA_IP);  
        W25Q128_Write((uint8_t*)&g_wifi_sta_ip_temp[0],WQ_PAGE_WIFI_STA_IP,4); 
        W25Q128_Erase_Sector(WQ_PAGE_WIFI_STA_SN);  
        W25Q128_Write((uint8_t*)&g_wifi_sta_sn_temp[0],WQ_PAGE_WIFI_STA_SN,4); 
        W25Q128_Erase_Sector(WQ_PAGE_WIFI_STA_GW);  
        W25Q128_Write((uint8_t*)&g_wifi_sta_gw_temp[0],WQ_PAGE_WIFI_STA_GW,4); 
        
//        W25Q128_Erase_Sector(WQ_PAGE_WIFI_REIP);  
//        W25Q128_Write((uint8_t*)&g_wifi_sta_sever_ip_temp[0],WQ_PAGE_WIFI_REIP,4); 
        W25Q128_Erase_Sector(WQ_PAGE_WIFI_REPORT);  
        W25Q128_Write((uint8_t*)&g_wifi_sta_sever_port_temp[0],WQ_PAGE_WIFI_REPORT,strlen(&g_wifi_sta_sever_port_temp[0])+1); 
      
        W25Q128_Erase_Sector(WQ_PAGE_WIFI_ENNO);  
        W25Q128_Write(&g_w5500EnFlag,WQ_PAGE_WIFI_ENNO,g_wifiEnFlag); 


        table[0]=FIRST_PARAAMETER;
        W25Q128_Erase_Sector(WQ_PAGE_DEFAULT);  
        W25Q128_Write(&table[0],WQ_PAGE_DEFAULT,1); 
        
        osMutexRelease(W25q64MutexHandle);                  //释放互斥资源    
        //##############################################################################################  

        
       
        //-------------------------------------------------------------------------------------------------------------------
        //以下不属于默认参数操作 勿动
        //##############################################################################################
        //互斥操作
        osMutexAcquire(W25q64MutexHandle, osWaitForever);   //一直等待获取互斥资源 
        W25Q128_TypeTest();
        W25Q128_Read(&read_buffU8[0],WQ_PAGE_SW5ROAD3,16);
        osMutexRelease(W25q64MutexHandle);                  //释放互斥资源    
        //##############################################################################################
        for(m=0;m<16;m++)
        {
            if(0xff==read_buffU8[m])
            {
               ;
            }
            else
            {
               m=100;
            }
        }
        if(99<=m)
        {
            App_Printf("$$clear HoldingBuf\r\n");
            for(m=0;m<8;m++)
            {
               read_buffU16[m]=0;
            }
            //##############################################################################################
            //互斥操作
            osMutexAcquire(W25q64MutexHandle, osWaitForever);   //一直等待获取互斥资源        
            W25Q128_Erase_Sector(WQ_PAGE_SW5ROAD3);  
            osMutexRelease(W25q64MutexHandle);                  //释放互斥资源    
            //##############################################################################################
        }
        else
        {
            for(m=0;m<8;m++)
            {
               read_buffU16[m]=0;
            }
            App_Printf("$$HoldingBuf already clear\r\n"); 
        }
        
        //##############################################################################################
        //互斥操作
        osMutexAcquire(ReadWriteRegHoldingMutexHandle, osWaitForever);   //一直等待获取互斥资源   
        for(m=0;m<3;m++)
        {
            g_diNumTalbe[m+1]=read_buffU16[m];
            usRegHoldingBuf[m+5]=read_buffU16[m];
        }
        for(m=3;m<8;m++)
        {
            g_keyTimeTalbe[m-2]=read_buffU16[m];
            usRegHoldingBuf[m+5]=read_buffU16[m];
        }    
        osMutexRelease(ReadWriteRegHoldingMutexHandle);                  //释放互斥资源       
        //############################################################################################## 
								
    }
    
    //---------------------------------------------------------------------------------------------------------------------------------------------------------------
    //串口打印当前参数    
    App_Printf("\r\n$$-----------------------W5500--------------------\r\n");    
    App_Printf("W5500 IP=%d.%d.%d.%d\r\n",g_w5500_local_ip[0],g_w5500_local_ip[1],g_w5500_local_ip[2],g_w5500_local_ip[3]);
    App_Printf("W5500 SN=%d.%d.%d.%d\r\n",g_w5500_subnet[0],g_w5500_subnet[1],g_w5500_subnet[2],g_w5500_subnet[3]);
    App_Printf("W5500 GW=%d.%d.%d.%d\r\n",g_w5500_gateway[0],g_w5500_gateway[1],g_w5500_gateway[2],g_w5500_gateway[3]);
    
    if(1==g_w5500EnFlag)
    {
        App_Printf("###W5500 is EN###\r\n");
    }
    else
    {
        App_Printf("###W5500 is NO###\r\n");
    }
    App_Printf("\r\n$$------------------------WIFI---------------------\r\n");
    App_Printf("WIFI apid=%s\r\n",g_wifi_ap_sidd_temp);
    App_Printf("WIFI apkey=%s\r\n",g_wifi_ap_password_temp);
    App_Printf("WIFI APIP=%d.%d.%d.%d\r\n",g_wifi_ap_ip_temp[0],g_wifi_ap_ip_temp[1],g_wifi_ap_ip_temp[2],g_wifi_ap_ip_temp[3]);
    App_Printf("WIFI APSN=%d.%d.%d.%d\r\n",g_wifi_ap_sn_temp[0],g_wifi_ap_sn_temp[1],g_wifi_ap_sn_temp[2],g_wifi_ap_sn_temp[3]);
    App_Printf("WIFI APGW=%d.%d.%d.%d\r\n",g_wifi_ap_gw_temp[0],g_wifi_ap_gw_temp[1],g_wifi_ap_gw_temp[2],g_wifi_ap_gw_temp[3]);
    
    App_Printf("WIFI staid=%s\r\n",g_wifi_sta_sidd_temp);
    App_Printf("WIFI stakey=%s\r\n",g_wifi_sta_password_temp);
    App_Printf("WIFI STAIP=%d.%d.%d.%d\r\n",g_wifi_sta_ip_temp[0],g_wifi_sta_ip_temp[1],g_wifi_sta_ip_temp[2],g_wifi_sta_ip_temp[3]);
    App_Printf("WIFI STASN=%d.%d.%d.%d\r\n",g_wifi_sta_sn_temp[0],g_wifi_sta_sn_temp[1],g_wifi_sta_sn_temp[2],g_wifi_sta_sn_temp[3]);
    App_Printf("WIFI STAGW=%d.%d.%d.%d\r\n",g_wifi_sta_gw_temp[0],g_wifi_sta_gw_temp[1],g_wifi_sta_gw_temp[2],g_wifi_sta_gw_temp[3]);
    
//    App_Printf("WIFI REIP=%d.%d.%d.%d\r\n",g_wifi_sta_sever_ip_temp[0],g_wifi_sta_sever_ip_temp[1],g_wifi_sta_sever_ip_temp[2],g_wifi_sta_sever_ip_temp[3]);
    App_Printf("WIFI REPORT=%s\r\n",g_wifi_sta_sever_port_temp);
      
    if(1==g_wifiEnFlag)
    {
        App_Printf("###WIFI is EN###\r\n");
    }
    else
    {
        App_Printf("###WIFI is NO###\r\n");
    }
    App_Printf("$$********READ********W25Q128********END********\r\n\r\n");
}


//----------------------------------------------------------------------------------------------------------------
//1=过了0点 要清零 时间在0:00~0:01分之间 注意如果清零了 要等2分钟后再检测
void TestClockClear_1000ms(void)
{
    uint8_t weekTemp;
    static uint8_t clearFlag=0;
    static uint16_t clearN=0;
    static uint16_t n=0;
    
    while(1==g_readingRX8025TimerFlag)
    {
        osDelay(10);
    }
    g_readingRX8025TimerFlag=1;
    bsp_GetRtcTime(&g_justTimerStruct);
    g_readingRX8025TimerFlag=0;
    n++;
    
    if(30<=n)
    {
        weekTemp= bsp_ReadRtcWeekChange(&g_justTimerStruct);

        App_Printf("$$ year=20%d,month=%d,day=%d\r\n",g_justTimerStruct.year,g_justTimerStruct.month,g_justTimerStruct.day);
        App_Printf("$$ H=%d,M=%d,S=%d,Week=%d\r\n",g_justTimerStruct.hour,g_justTimerStruct.minute,g_justTimerStruct.second,weekTemp);
        n=0;
    }
      
    if(0==clearFlag)
    {
        if((0==g_justTimerStruct.hour)&&(0==g_justTimerStruct.minute))
        {
            ClearParameter();
            App_Printf("$$ ##########0:00 let/'s clear Parameter#########\r\n");
            clearFlag=1;
            clearN=0;  
        }
    }
    else
    {
        clearN++;
        if(120<clearN)
        {
            clearFlag=0;
            clearN=0;
        }
    }
}

//-------------------------------------------------------------------------------------------------------------
//零点了,清零各个参数
void ClearParameter(void)
{
    //uint8_t save_Buffer[20];
    uint8_t m;
  
    //##############################################################################################
    //互斥操作
    osMutexAcquire(ReadWriteRegHoldingMutexHandle, osWaitForever);   //一直等待获取互斥资源 
    for(m=0;m<16;m++)
    {
    //save_Buffer[m]=0; 
        usRegHoldingBuf[m]=0;        
    }  
    for(m=1;m<=5;m++)
    {
        //g_keyStateTalbe[m]=0;
        g_keyTimeTalbe[m]=0;             
    } 
    for(m=1;m<=3;m++)
    {
        g_diNumTalbe[m]=0;  
    } 
 
    osMutexRelease(ReadWriteRegHoldingMutexHandle);                  //释放互斥资源    
    //##############################################################################################  
    
    osDelay(2);
    
    ////##############################################################################################
    ////互斥操作
    //osMutexAcquire(W25q64MutexHandle, osWaitForever);   //一直等待获取互斥资源 
    //W25Q128_Erase_Sector(WQ_PAGE_SW5ROAD3);  
    //W25Q128_Write(&save_Buffer[0],WQ_PAGE_SW5ROAD3,16);
    //osMutexRelease(W25q64MutexHandle);                  //释放互斥资源    
    ////##############################################################################################    
}
             

/* USER CODE END Application */

