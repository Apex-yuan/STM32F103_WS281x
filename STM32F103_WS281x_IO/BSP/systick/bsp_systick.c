/**
  ******************************************************************************
  * @file    bsp_systick.c 
  * @author  Apex yuan
  * @version V1.0.0
  * @date    2019-8-16
  * @brief   Main program body
  ******************************************************************************
  * @attention 
  * 1.由系统滴答定时器同时实现系统时间记录和us ms延时。
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "bsp_systick.h"

#if USE_SYSTICK_IRQ  //使用SYSTICK中断方式实现延时函数

volatile uint32_t _us_tick = 0;
volatile uint32_t _ms_tick = 0;

/**
  * @brief  initialize systick
  * @param  None
  * @retval None
  */
void systick_init(void) 
{
  _us_tick = 0;
  _ms_tick = 0;  
  
  /* SystemCoreClock / 1000000  1us中断一次 */
  /* SystemCoreClock / 1000     1ms中断一次 */
	if(SysTick_Config(SystemCoreClock / 1000000))
  {
    /*capture error*/
    while(1);
  }
  
  /* Configure the SysTick handler priority */
  //NVIC_SetPriority(SysTick_IRQn, 0x0);
}

/**
  * @brief  delay (ms)
  * @param  delay time
  * @retval None
  */
void delay_ms(uint32_t millis) 
{ 
	uint32_t target;
	
	target = _ms_tick + millis;
	while(_ms_tick < target);
} 

/**
  * @brief  delay (us)
  * @param  delay time
  * @retval None
  */
void delay_us(uint32_t uillis)
{ 
	uint32_t target;
	target = _us_tick + uillis;
	while(_us_tick < target);
}

/**
  * @brief  获取系统当前的ms计数值
  * @param  None
  * @retval 系统当前时间ms
  */
uint32_t millis(void) 
{
	return _ms_tick;
}

/**
  * @brief  获取系统当前的us计数值
  * @param  delay time
  * @retval 系统当前时间us
  */
uint32_t micros(void)
{
  return _us_tick;
}

/**
  * @brief  复位系统计数
  * @param  None
  * @retval None
  */
void systick_reset(void) 
{
	_us_tick = 0;
  _ms_tick = 0;
}

/**
  * @brief  SYSTICK中断服务函数
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
	_us_tick++;
  _ms_tick = _us_tick / 1000;
  //_ms_tick++;

}

#else //使用systick 轮询方式实现延时函数

static uint8_t  fac_us=0;							//us延时倍乘数			   
static uint16_t fac_ms=0;							//ms延时倍乘数

void systick_init(void)
{
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);	//选择外部时钟  HCLK/8
	fac_us = SystemCoreClock/8000000;				//为系统时钟的1/8  
	fac_ms = (u16)fac_us*1000;					//每个ms需要的systick时钟数   
}	

//延时nus
//nus为要延时的us数.
void delay_us(uint32_t nus)
{		
	uint32_t temp;	    	 
	SysTick->LOAD=nus*fac_us; 					//时间加载	  		 
	SysTick->VAL=0x00;        					//清空计数器
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;	//开始倒数	  
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));		//等待时间到达   
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;	//关闭计数器
	SysTick->VAL =0X00;      					 //清空计数器	 
}
//延时nms
//注意nms的范围
//SysTick->LOAD为24位寄存器,所以,最大延时为:
//nms<=0xffffff*8*1000/SYSCLK
//SYSCLK单位为Hz,nms单位为ms
//对72M条件下,nms<=1864 
void delay_ms(uint16_t nms)
{	 		  	  
	uint32_t temp;		   
	SysTick->LOAD=(uint32_t)nms*fac_ms;				//时间加载(SysTick->LOAD为24bit)
	SysTick->VAL =0x00;							//清空计数器
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;	//开始倒数  
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));		//等待时间到达   
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;	//关闭计数器
	SysTick->VAL =0X00;       					//清空计数器	  	    
} 

#endif /* USE_SYSTICK_IRQ */
