/**
  ******************************************************************************
  * @file    bsp_ws281x.c 
  * @author  Apex yuan
  * @version V1.0.0
  * @date    2019-8-16
  * @brief   Main program body
  ******************************************************************************
  * @attention
  * 1. ws281x_show()函数内部的执行流程非常重要，不要随意修改，可能会导致灯珠颜色
  *    错乱的情况
  * 2. 为了使WS2812灯珠变换函数正常执行，修改了bsp_systick.c文件。
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "bsp_ws281x.h"
#include "bsp_systick.h"

uint16_t pixelBuffer[PIXEL_NUM][GRB];

/**
  * @brief  initialize WS281x LED
  * @param  None
  * @retval None
  */
void ws281x_init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
  TIM_OCInitTypeDef TIM_OCInitStruct;
  DMA_InitTypeDef DMA_InitStruct;
  
  /* 开启GPIO TIM DMA 外设时钟 */
  WS281x_GPIO_CLK_FUNCTION(WS281x_GPIO_CLK, ENABLE);
  WS281x_TIM_CLK_FUNCTION(WS281x_TIM_CLK, ENABLE);
  WS281x_DMA_CLK_FUNCTION(WS281x_DMA_CLK, ENABLE);
  
  /* GPIO端口配置 */
  GPIO_InitStruct.GPIO_Pin = WS281x_GPIO_PIN;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(WS281x_GPIOx, &GPIO_InitStruct);
  
  /* 定时器时基配置 */
  TIM_TimeBaseInitStruct.TIM_Prescaler = 0;
  TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInitStruct.TIM_Period = 90 - 1;  //72M / 90 = 800K
  TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseInit(WS281x_TIMx, &TIM_TimeBaseInitStruct);
  
  /* 定时器输出比较配置 */
  TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStruct.TIM_Pulse = 0;
  TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OCInitStruct.TIM_OCIdleState = TIM_OCIdleState_Reset;
  TIM_OC1Init(WS281x_TIMx, &TIM_OCInitStruct);
  
  TIM_CtrlPWMOutputs(WS281x_TIMx, ENABLE);	//PWM输出使能	

  TIM_Cmd(WS281x_TIMx, DISABLE);
  TIM_DMACmd(WS281x_TIMx, WS281x_TIM_DMA_SOURCE, ENABLE);
  
  /* DMA配置 */
  DMA_InitStruct.DMA_PeripheralBaseAddr = WS281x_DMA_PERIPH_ADDR;
  DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)pixelBuffer;
  DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralDST;
  DMA_InitStruct.DMA_BufferSize = PIXEL_NUM * GRB;
  DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStruct.DMA_Priority = DMA_Priority_Medium;
  DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(WS281x_DMAx_CHANNELx, &DMA_InitStruct);
    
  DMA_Cmd(WS281x_DMAx_CHANNELx, DISABLE);
  
  /*关闭所有灯珠*/
  ws281x_closeAll();
  delay_ms(100);
}

/*
 * bref ：经测试这么写显示函数，可以使用。
 *        
 */
/**
  * @brief  显示函数，将显存数据显示到灯珠上。
  *         TIM_OC1PreloadConfig函数如果只放在初始化函数中使能，灯珠会错位显示。如果没有该函数，在动态变换过程中灯珠的颜色并不能正常显示。
  * @param  None
  * @retval None
  */
void ws281x_show(void)
{
  DMA_SetCurrDataCounter(WS281x_DMAx_CHANNELx, (uint16_t)(PIXEL_NUM * GRB));  //设定DMA要传输数据的大小 
  DMA_Cmd(WS281x_DMAx_CHANNELx, ENABLE);
  TIM_OC1PreloadConfig(WS281x_TIMx, TIM_OCPreload_Enable);   //CH1预装载器使能
  TIM_Cmd(WS281x_TIMx, ENABLE);
  while(DMA_GetFlagStatus(WS281x_DMA_FLAG) != SET);    //等待传输完成
  DMA_Cmd(WS281x_DMAx_CHANNELx, DISABLE);
  TIM_OC1PreloadConfig(WS281x_TIMx, TIM_OCPreload_Disable); //关闭CH1预装载器
  TIM_Cmd(WS281x_TIMx, DISABLE);
  DMA_ClearFlag(WS281x_DMA_FLAG);
}

/**
  * @brief  close all led
  * @param  None
  * @retval None
  */
void ws281x_closeAll(void)
{
  uint16_t i;
  uint8_t j;
  
  for(i = 0; i < PIXEL_NUM; ++i)
  {
    for(j = 0; j < 24; ++j)
    {
      pixelBuffer[i][j] = WS_LOW;
    }
  }
  ws281x_show(); 
}

/**
  * @brief  将RGB颜色组合到一个32位数据中存储
  * @param  red：  0-255
  *         green：0-255
  *         blue： 0-255
  * @retval None
  */
uint32_t ws281x_color(uint8_t red, uint8_t green, uint8_t blue)
{
  return green << 16 | red << 8 | blue;
}

/**
  * @brief  给特定LED灯设定颜色
  * @param  ｎ：LED灯号
　*         GRBClor: 32位的颜色值
  * @retval None
  */
void ws281x_setPixelColor(uint16_t n, uint32_t GRBColor)
{
  uint8_t i;
  if(n < PIXEL_NUM)
  {
    for(i = 0; i < GRB; i++)
    {
      pixelBuffer[n][i] = ((GRBColor << i) & 0x800000) ? WS_HIGH : WS_LOW;
    }
  }
}

void ws281x_setPixelRGB(uint16_t n ,uint8_t red, uint8_t green, uint8_t blue)
{
  uint8_t i;
  
  if(n < PIXEL_NUM)
  {
    for(i = 0; i < GRB; ++i)
    {
      pixelBuffer[n][i] = (((ws281x_color(red,green,blue) << i) & 0X800000) ? WS_HIGH : WS_LOW);
    }
  }
}



// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t ws281x_wheel(uint8_t wheelPos) {
  wheelPos = 255 - wheelPos;
  if(wheelPos < 85) {
    return ws281x_color(255 - wheelPos * 3, 0, wheelPos * 3);
  }
  if(wheelPos < 170) {
    wheelPos -= 85;
    return ws281x_color(0, wheelPos * 3, 255 - wheelPos * 3);
  }
  wheelPos -= 170;
  return ws281x_color(wheelPos * 3, 255 - wheelPos * 3, 0);
}

// Fill the dots one after the other with a color
void ws281x_colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<PIXEL_NUM; i++) {
    ws281x_setPixelColor(i, c);
    ws281x_show();
    delay_ms(wait);
  }
}

void ws281x_rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<PIXEL_NUM; i++) {
      ws281x_setPixelColor(i, ws281x_wheel((i+j) & 255));
    }
    ws281x_show();
    delay_ms(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void ws281x_rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< PIXEL_NUM; i++) {
      ws281x_setPixelColor(i,ws281x_wheel(((i * 256 / PIXEL_NUM) + j) & 255));
    }
    ws281x_show();
    delay_ms(wait);
  }
}

//Theatre-style crawling lights.
void ws281x_theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < PIXEL_NUM; i=i+3) {
        ws281x_setPixelColor(i+q, c);    //turn every third pixel on
      }
      ws281x_show();

      delay_ms(wait);

      for (uint16_t i=0; i < PIXEL_NUM; i=i+3) {
        ws281x_setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void ws281x_theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < PIXEL_NUM; i=i+3) {
        ws281x_setPixelColor(i+q, ws281x_wheel( (i+j) % 255));    //turn every third pixel on
      }
      ws281x_show();

      delay_ms(wait);

      for (uint16_t i=0; i < PIXEL_NUM; i=i+3) {
        ws281x_setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

