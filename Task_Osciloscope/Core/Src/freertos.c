/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
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
#include "usart.h"
#include "can.h"
#include "tim.h"
#include "stdio.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

// 枚举类型定义电机ID
typedef enum MotorId
{
	  MOTOR_ID_1 = 1,  // 电机1
    MOTOR_ID_2 = 2,  // 电机2
    MOTOR_ID_3 = 3,  // 电机3
    MOTOR_ID_4 = 4,  // 电机4
    MOTOR_ID_5 = 5,  // 电机5
    MOTOR_ID_6 = 6,  // 电机6
    MOTOR_ID_7 = 7,  // 电机7
  
} MotorId_t;
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
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityAboveNormal,
};
/* Definitions for canTask */
osThreadId_t canTaskHandle;
const osThreadAttr_t canTask_attributes = {
  .name = "canTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for uartTask */
osThreadId_t uartTaskHandle;
const osThreadAttr_t uartTask_attributes = {
  .name = "uartTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for pwmTask */
osThreadId_t pwmTaskHandle;
const osThreadAttr_t pwmTask_attributes = {
  .name = "pwmTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void DriveDjMotor(CAN_HandleTypeDef* hcan,MotorId_t motorId,int16_t motorVoltage);
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void StartCanTask(void *argument);
void StartUartTask(void *argument);
void StartPwmTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

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

  /* creation of canTask */
  canTaskHandle = osThreadNew(StartCanTask, NULL, &canTask_attributes);

  /* creation of uartTask */
  uartTaskHandle = osThreadNew(StartUartTask, NULL, &uartTask_attributes);

  /* creation of pwmTask */
  pwmTaskHandle = osThreadNew(StartPwmTask, NULL, &pwmTask_attributes);

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
  for(;;)
  { 
		taskENTER_CRITICAL(); //进入临界区，防止初始化被打断造成异常
		
		HAL_TIM_Base_Start(&htim12);              //开启TIM12时基
		HAL_TIM_PWM_Start(&htim12,TIM_CHANNEL_1); //开启PWM输出
		HAL_CAN_Start(&hcan1);                    //开启CAN
		
		taskEXIT_CRITICAL(); //初始化完成，退出临界区
		vTaskDelete(NULL);                        //初始化完成，删掉自己
   
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartCanTask */
/**
* @brief Function implementing the canTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartCanTask */
void StartCanTask(void *argument)
{
  /* USER CODE BEGIN StartCanTask */
	
	TickType_t xLastWakeTime;
	/* 获得当前的Tick Count */
	xLastWakeTime = xTaskGetTickCount();
	
  /* Infinite loop */
  for(;;)
  {

		//输出控制电机的CAN报文
		taskENTER_CRITICAL();
		DriveDjMotor(&hcan1,MOTOR_ID_1,2000);
		taskEXIT_CRITICAL();
		
		//(这个延时函数会自动更新xLastWakeTime,只要第一次进入任务时获取了系统时间即可)
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(10));//10ms控制一次电机
		
  }
  /* USER CODE END StartCanTask */
}

/* USER CODE BEGIN Header_StartUartTask */
/**
* @brief Function implementing the uartTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartUartTask */
void StartUartTask(void *argument)
{
  /* USER CODE BEGIN StartUartTask */
	
	TickType_t xLastWakeTime;
	/* 获得当前的Tick Count */
	xLastWakeTime = xTaskGetTickCount();
	
  /* Infinite loop */
  for(;;)
  {
		taskENTER_CRITICAL();
		printf("ZMS");         //打印ZMS
		taskEXIT_CRITICAL();
    
		//(这个延时函数会自动更新xLastWakeTime,只要第一次进入任务时获取了系统时间即可)
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(100));
  }
  /* USER CODE END StartUartTask */
}

/* USER CODE BEGIN Header_StartPwmTask */
/**
* @brief Function implementing the pwmTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartPwmTask */
void StartPwmTask(void *argument)
{
  /* USER CODE BEGIN StartPwmTask */
	
	TickType_t xLastWakeTime;
	/* 获得当前的Tick Count */
	xLastWakeTime = xTaskGetTickCount();
	uint8_t i=0;
  /* Infinite loop */
  for(;;)
  {
		

		
		__HAL_TIM_SetCompare(&htim12, TIM_CHANNEL_1,i);
		
		//更新i,每次进入该函数设置不同的占空比，实现占空比的周期性变化
		if(i<100) i+=5;
    else i=0;
       
		//(这个延时函数会自动更新xLastWakeTime,只要第一次进入任务时获取了系统时间即可)
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(1)); //占空比周期性变化，频率约等于47hz
  }
  /* USER CODE END StartPwmTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */


/*
* @brief : 通过CAN驱动Dj电机
* @param1: 通过哪个CAN发送控制电机的报文
* @param2: 电机ID 参数可以是下面的几个 @arg MotorId_t
* @param3: 电机转动电压 -25000~25000
*/

void DriveDjMotor(CAN_HandleTypeDef* hcan,MotorId_t motorId,int16_t motorVoltage)
{   
	   // 根据角度计算电压值 (-25000 ~ 25000)

    // 获取高 8 位和低 8 位
    uint8_t highByte = (motorVoltage >> 8) & 0xFF; // 右移 8 位并取高 8 位
    uint8_t lowByte = motorVoltage & 0xFF;         // 取低 8 位

    // 初始化 CAN 消息
    CanMessage_t canMessageTx = 
	  {
        .TxHeader = 
			  {
					.StdId = (motorId > 4) ? 0x2FF : 0x1FF,  //电机ID>4,则标识符ID为0x2FF，否则为0x1FF
            .IDE = CAN_ID_STD,        // 设置为标准 ID
            .RTR = CAN_RTR_DATA,      // 数据帧
            .DLC = 8                  // 数据长度
        },
        .TxDataBuf = {0, 0, 0, 0, 0, 0, 0, 0}  // 初始化数据缓冲区
    };
		
			 
    // 根据电机 ID 设置相应的电压值
    switch (motorId) 
		{
        case MOTOR_ID_1:
				case MOTOR_ID_5:
            canMessageTx.TxDataBuf[0] = highByte;
            canMessageTx.TxDataBuf[1] = lowByte;
            break;
				
        case MOTOR_ID_2:
				case MOTOR_ID_6:
            canMessageTx.TxDataBuf[2] = highByte;
            canMessageTx.TxDataBuf[3] = lowByte;
            break;
				
        case MOTOR_ID_3:
				case MOTOR_ID_7:
            canMessageTx.TxDataBuf[4] = highByte;
            canMessageTx.TxDataBuf[5] = lowByte;
            break;
				
        case MOTOR_ID_4:
            canMessageTx.TxDataBuf[6] = highByte;
            canMessageTx.TxDataBuf[7] = lowByte;
            break;
        default:
            return;  // 无效的电机ID
    }

    // 发送 CAN 消息
    SendCanMessage(hcan, &canMessageTx);
}

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
