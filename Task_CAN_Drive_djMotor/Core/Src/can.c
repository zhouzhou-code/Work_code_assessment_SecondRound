/**
  ******************************************************************************
  * @file    can.c
  * @brief   This file provides code for the configuration
  *          of the CAN instances.
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

/* Includes ------------------------------------------------------------------*/
#include "can.h"

/* USER CODE BEGIN 0 */
#include "usart.h"
#include "stdio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"


/* USER CODE END 0 */

CAN_HandleTypeDef hcan1;

/* CAN1 init function */
void MX_CAN1_Init(void)
{

  /* USER CODE BEGIN CAN1_Init 0 */

  /* USER CODE END CAN1_Init 0 */

  /* USER CODE BEGIN CAN1_Init 1 */

  /* USER CODE END CAN1_Init 1 */
  hcan1.Instance = CAN1;
  hcan1.Init.Prescaler = 2;
  //hcan1.Init.Mode = CAN_MODE_NORMAL;
  hcan1.Init.Mode =CAN_MODE_LOOPBACK;//换用LoopBack测试
  hcan1.Init.SyncJumpWidth = CAN_SJW_2TQ;
  hcan1.Init.TimeSeg1 = CAN_BS1_15TQ;
  hcan1.Init.TimeSeg2 = CAN_BS2_5TQ;
  hcan1.Init.TimeTriggeredMode = DISABLE;
  hcan1.Init.AutoBusOff = DISABLE;
  hcan1.Init.AutoWakeUp = DISABLE;
  hcan1.Init.AutoRetransmission = DISABLE;
  hcan1.Init.ReceiveFifoLocked = DISABLE;
  hcan1.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN1_Init 2 */
/*---------------------------------------CAN1初始化过滤器Start-----------------------------------------------------*/
	
	//配置为接收所有报文

  // 过滤器结构体
  CAN_FilterTypeDef  sFilterConfig;
	
	sFilterConfig.SlaveStartFilterBank = 0;           // 分配滤波器资源()
	sFilterConfig.FilterBank = 0;                      // 过滤器序号
	
	sFilterConfig.FilterIdHigh = 0x0000;               // 
	sFilterConfig.FilterIdLow = 0x0000;                // 
	sFilterConfig.FilterMaskIdHigh = 0x0000;           // 
	sFilterConfig.FilterMaskIdLow = 0x0000;            // 
	sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT; // 32位掩码
	sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;  // 掩码模式
	sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0; // 给邮箱0配置的过滤器
	sFilterConfig.FilterActivation = CAN_FILTER_ENABLE ;
	
	// 配置并自检
	if (HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig) != HAL_OK)
  {
    Error_Handler();
  }
				
	/*---------------------------------------CAN1初始化过滤器End-----------------------------------------------------*/
	//__HAL_CAN_ENABLE_IT(&hcan1,CAN_IT_RX_FIFO0_MSG_PENDING);//使能can1接收中断
	
	HAL_CAN_ActivateNotification(&hcan1,CAN_IT_RX_FIFO0_MSG_PENDING);//使能can1接收中断
   HAL_CAN_Start(&hcan1);         //开启can1
  /* USER CODE END CAN1_Init 2 */

}

void HAL_CAN_MspInit(CAN_HandleTypeDef* canHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspInit 0 */

  /* USER CODE END CAN1_MspInit 0 */
    /* CAN1 clock enable */
    __HAL_RCC_CAN1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**CAN1 GPIO Configuration
    PA11     ------> CAN1_RX
    PA12     ------> CAN1_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_CAN1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* CAN1 interrupt Init */
    HAL_NVIC_SetPriority(CAN1_RX0_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);
    HAL_NVIC_SetPriority(CAN1_RX1_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(CAN1_RX1_IRQn);
  /* USER CODE BEGIN CAN1_MspInit 1 */

  /* USER CODE END CAN1_MspInit 1 */
  }
}

void HAL_CAN_MspDeInit(CAN_HandleTypeDef* canHandle)
{

  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspDeInit 0 */

  /* USER CODE END CAN1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_CAN1_CLK_DISABLE();

    /**CAN1 GPIO Configuration
    PA11     ------> CAN1_RX
    PA12     ------> CAN1_TX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_11|GPIO_PIN_12);

    /* CAN1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(CAN1_RX0_IRQn);
    HAL_NVIC_DisableIRQ(CAN1_RX1_IRQn);
  /* USER CODE BEGIN CAN1_MspDeInit 1 */

  /* USER CODE END CAN1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
void send_can_message(CAN_HandleTypeDef* hcan,CAN_Message_t* CAN_Message_Tx)
{
	  uint32_t TxMailbox=CAN_TX_MAILBOX0;
	 
    if (HAL_CAN_AddTxMessage(hcan, &(CAN_Message_Tx->TxHeader), CAN_Message_Tx->TxDataBuf, &TxMailbox) == HAL_OK) 
		{					
			//SAFE_PRINTF("CAN message sent.\r\n");//包含了
    } 
		else 
		{
      SAFE_PRINTF("Failed to send CAN message.\r\n");//串口提示消息发出
    }	
}

uint8_t receive_can_message(CAN_HandleTypeDef* hcan,CAN_Message_t* CAN_Message_Rx) 
{       

    if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &CAN_Message_Rx->RxHeader, CAN_Message_Rx->RxDataBuf) == HAL_OK)
		{
			CAN_Message_Rx->RxDataLength=CAN_Message_Rx->RxHeader.DLC;   //接受长度成员赋值为DLC
      //printf("CAN message received.\r\n");//串口提示消息接收到   	
    }		
		return 	CAN_Message_Rx->RxDataLength;	
}
/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
