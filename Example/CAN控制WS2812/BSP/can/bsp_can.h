#ifndef __BSP_CAN_H
#define __BSP_CAN_H

#include "stm32f10x.h"

#define CANx                    CAN1
#define CAN_CLK                 RCC_APB1Periph_CAN1
#define CAN_CLK_FUNCTION        RCC_APB1PeriphClockCmd
#define CAN_RX_PIN              GPIO_Pin_11
#define CAN_TX_PIN              GPIO_Pin_12
#define CAN_GPIO_PORT           GPIOA
#define CAN_GPIO_CLK            RCC_APB2Periph_GPIOA
#define CAN_GPIO_CLK_FUNCTION   RCC_APB2PeriphClockCmd
//#define CAN_AF_PORT             GPIO_AF_CAN1
#define CAN_RX_SOURCE           GPIO_PinSource11   
#define CAN_TX_SOURCE           GPIO_PinSource12

#define CAN_RX_IRQn           USB_LP_CAN1_RX0_IRQn

extern CanTxMsg can_tx_msg;
extern CanRxMsg can_rx_msg;
extern uint8_t can_rx_flag;

void can_init(void);

#endif /* __BSP_CAN_H */

