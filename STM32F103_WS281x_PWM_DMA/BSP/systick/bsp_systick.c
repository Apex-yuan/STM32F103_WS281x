/**
  ******************************************************************************
  * @file    bsp_systick.c 
  * @author  Apex yuan
  * @version V1.0.0
  * @date    2019-8-16
  * @brief   Main program body
  ******************************************************************************
  * @attention
  * 1. 修改了SYSTICK的配置值（SystemCoreClock / 1000000 改为 SystemCoreClock / 1000）
  *    系统定时器变为1ms中断一次，目前该文件仅能实现ms级延时。
  * 2. 上述修改是为了配合实现PWM+DMA的方式控制WS2812灯珠，若为1us中断一次，由此实
  *    现的delay_ms()函数，用在WS2812灯珠的显示改变过程中会出现灯珠颜色混乱的情况。
  *    从示波器上也没有看出什么问题，不知到是哪里影响了。（后续使用时要注意）
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "bsp_systick.h"

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
	if(SysTick_Config(SystemCoreClock / 1000))
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
  //_ms_tick = _us_tick / 1000;
  _ms_tick++;

}