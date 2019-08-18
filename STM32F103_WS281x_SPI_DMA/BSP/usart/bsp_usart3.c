/**
  ******************************************************************************
  * @file    bsp_usart3.c
  * @author  xiaoyuan
  * @version V1.0
  * @date    2018-11-06
  * @brief   config usart3
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */

#include "bsp_usart3.h"
#include <stdarg.h>
#include <string.h> 
#include <stdio.h>


uint8_t usart3_rx_buffer[USART3_RX_MAX_LEN] = {0};
uint16_t usart3_rx_cnt = 0;

/**
  * @brief  初始化串口3
  * @param  bound：串口波特率，一般设置为115200
  * @retval None
  */
void usart3_init(uint32_t bound) 
{
  /*定义初始化结构体*/
  GPIO_InitTypeDef   GPIO_InitStructure;
	USART_InitTypeDef  USART_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;
	
	/* 开启GPIOB端口时钟及USART3串口时钟 */
  RCC_APB1PeriphClockCmd(USART3_CLK, ENABLE);
	RCC_APB2PeriphClockCmd(USART3_GPIO_CLK, ENABLE); 
 	
	/* 复位串口1 */
	USART_DeInit(USART3);  //复位串口1
	
	/* 配置USART3_TX（PB10）引脚GPIO状态 */
  GPIO_InitStructure.GPIO_Pin = USART3_TX_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(USART3_TX_PORT, &GPIO_InitStructure);   

  /* 配置USART3_RX（PB11）引脚GPIO状态 */
  GPIO_InitStructure.GPIO_Pin = USART3_RX_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_Init(USART3_RX_PORT, &GPIO_InitStructure);  

  /* USART 初始化设置 */
	USART_InitStructure.USART_BaudRate = bound;  //波特率：一般设置为115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;  //字长：为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;  //停止位：一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;  //校验位：无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//硬件流控制:无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//工作模式：收发模式
  USART_Init(USART3, &USART_InitStructure);  //根据指定的参数初始化串口1
  
  /* 配置嵌套向量中断控制器NVIC */ 
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;  //配置USART1为中断源
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2; //抢占优先级2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  //子优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  //IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	 //根据指定的参数初始化NVIC寄存器
	
  /* 使能串口接收中断 */ 
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
  /* 使能串口总线空闲中断 */
//  USART_ITConfig (USART3, USART_IT_IDLE, ENABLE );
  /* 串口3使能 */
  USART_Cmd(USART3, ENABLE);
}

void usart3_sendByte(uint8_t c)
{
  while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
  USART_SendData(USART3, c);
  
}

void usart3_sendStr(char * str)
{
  while(*str != '\0')
  {
    usart3_sendByte(*str++);
  }
}

//串口3,printf 函数
//确保一次发送数据不超过USART3_BUFFER_LEN字节
void usart3_printf(char* fmt,...)  
{
  char buffer[USART3_BUFFER_LEN+1];  
	uint16_t i,j; 

	va_list ap; 
	va_start(ap, fmt);
	vsprintf((char*)buffer, fmt, ap);
	va_end(ap);
	i = strlen((const char*)buffer);		//此次发送数据的长度
	for(j = 0; j < i; j++)							//循环发送数据
	{
	  while(USART_GetFlagStatus(USART3,USART_FLAG_TC) == RESET); //循环发送,直到发送完毕   
		USART_SendData(USART3,buffer[j]); 
	} 
}


/**
  * @brief  USART3串口中断服务函数
  * @param  none
  * @retval none
  */ 
void USART3_IRQHandler(void) 
{
	uint8_t rec; 
	
  /*串口接收中断*/
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //接收中断
	{
		rec = USART_ReceiveData(USART3);//(USART1->DR);	//读取接收到的数据
    usart3_rx_buffer[usart3_rx_cnt++] = rec;
  } 
} 
