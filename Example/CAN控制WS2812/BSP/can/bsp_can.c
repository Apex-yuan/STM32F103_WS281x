#include "bsp_can.h"

CanTxMsg can_tx_msg;
CanRxMsg can_rx_msg;
uint8_t can_rx_flag;

void can_init(void)
{
  /* GPIO configuration */
  GPIO_InitTypeDef GPIO_InitStruct;
  
  CAN_GPIO_CLK_FUNCTION(CAN_GPIO_CLK, ENABLE);
  
  GPIO_InitStruct.GPIO_Pin = CAN_RX_PIN;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(CAN_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.GPIO_Pin = CAN_TX_PIN;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(CAN_GPIO_PORT, &GPIO_InitStruct);
  
  /* CAN configuration */
  CAN_InitTypeDef CAN_InitStruct;
  
  CAN_CLK_FUNCTION(CAN_CLK, ENABLE);
 
  CAN_DeInit(CANx);
  
  CAN_InitStruct.CAN_TTCM = DISABLE;
  CAN_InitStruct.CAN_ABOM = DISABLE;
  CAN_InitStruct.CAN_AWUM = ENABLE;
  CAN_InitStruct.CAN_NART = ENABLE;
  CAN_InitStruct.CAN_RFLM = DISABLE;
  CAN_InitStruct.CAN_TXFP = DISABLE;
  CAN_InitStruct.CAN_Mode = CAN_Mode_LoopBack;
  CAN_InitStruct.CAN_SJW = CAN_SJW_2tq;
  /* CAN Baudrate 1Mbps */
  CAN_InitStruct.CAN_BS1 = CAN_BS1_6tq;
  CAN_InitStruct.CAN_BS2 = CAN_BS2_3tq;
  CAN_InitStruct.CAN_Prescaler = 4;
  CAN_Init(CANx, &CAN_InitStruct);
  
  /* CAN filter configuration */
  CAN_FilterInitTypeDef CAN_FilterInitStruct;
  
  CAN_FilterInitStruct.CAN_FilterNumber = 0;
  CAN_FilterInitStruct.CAN_FilterMode = CAN_FilterMode_IdMask;
  CAN_FilterInitStruct.CAN_FilterScale = CAN_FilterScale_32bit;
  CAN_FilterInitStruct.CAN_FilterIdHigh = 0x0000;
  CAN_FilterInitStruct.CAN_FilterIdLow = 0x0000;
  CAN_FilterInitStruct.CAN_FilterMaskIdHigh = 0x0000;
  CAN_FilterInitStruct.CAN_FilterMaskIdLow = 0x0000;
  CAN_FilterInitStruct.CAN_FilterFIFOAssignment = CAN_FilterFIFO0;
  CAN_FilterInitStruct.CAN_FilterActivation = ENABLE;
  CAN_FilterInit(&CAN_FilterInitStruct);
  
  /* NVIC configuration */
  NVIC_InitTypeDef NVIC_InitStruct;
  
  NVIC_InitStruct.NVIC_IRQChannel = CAN_RX_IRQn;
  NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStruct);
  
  /* ENABLE FIFO 0 RX Interrupt */
  CAN_ITConfig(CANx, CAN_IT_FMP0, ENABLE);
}

void USB_LP_CAN1_RX0_IRQHandler(void)
{
  CAN_Receive(CANx, CAN_FIFO0, &can_rx_msg);
  can_rx_flag = 1;
}


