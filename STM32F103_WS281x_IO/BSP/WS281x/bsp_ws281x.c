/**
  ******************************************************************************
  * @file    bsp_ws281x.c 
  * @author  Apex yuan
  * @version V1.0.0
  * @date    2019-8-16
  * @brief   Main program body
  ******************************************************************************
  * @attention
  * 1. 利用IO电平反转来模拟WS281x时序，对时间要求很高，基本上整个单片机不能做其他
  *    事情。
  * 2. 延时函数的实现，必须使用SYSTICK轮询的方式实现，中断方式会干扰通信时序
  * 3. 发送0、1码必须通过操作寄存器的方式，才可以达到需求。
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "bsp_ws281x.h"
#include "bsp_systick.h"

uint8_t pixelBuffer[PIXEL_NUM][GRB];

/**
  * @brief  initialize WS281x LED
  * @param  None
  * @retval None
  */
void ws281x_init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  
  WS281x_GPIO_CLK_FUNCTION(WS281x_GPIO_CLK, ENABLE);
  
  GPIO_InitStruct.GPIO_Pin = WS281x_GPIO_PIN;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(WS281x_GPIOx, &GPIO_InitStruct);
  GPIO_ResetBits(WS281x_GPIOx, WS281x_GPIO_PIN);
  
  ws281x_closeAll();
  delay_ms(100);
}

/**
  * @brief  专用于WS281x灯珠驱动的延时函数
  * @param  count：延时数 （示波器测量延时时间 = delay_num * 440ns ）
  * @retval None
  */
static void ws281x_delay(uint32_t count)
{
  while(count--);
}

/**
  * @brief  发送0码，此处对时间要求很高，操作IO端口只能通过寄存器方式操作，
  * @param  None
  * @retval None
  */
void ws281x_sendTero(void)
{
  //GPIO_SetBits(WS281x_GPIOx, WS281x_GPIO_PIN);
  GPIOA->BSRR = WS281x_GPIO_PIN; 
  ws281x_delay(1);
  //GPIO_ResetBits(WS281x_GPIOx, WS281x_GPIO_PIN);
  GPIOA->BRR = WS281x_GPIO_PIN;
  ws281x_delay(2);
}

/**
  * @brief  发送1码，此处对时间要求很高，操作IO端口只能通过寄存器方式操作，
  * @param  None
  * @retval None
  */
void ws281x_sendOne(void)
{
  //GPIO_SetBits(WS281x_GPIOx, WS281x_GPIO_PIN);
  GPIOA->BSRR = WS281x_GPIO_PIN;
  ws281x_delay(2);
  //GPIO_ResetBits(WS281x_GPIOx, WS281x_GPIO_PIN);
  GPIOA->BRR = WS281x_GPIO_PIN;
  ws281x_delay(1);
}

/**
  * @brief  发送复位
  * @param  None
  * @retval None
  */
void ws281x_sendReset(void)
{
  GPIO_ResetBits(WS281x_GPIOx, WS281x_GPIO_PIN);
  ws281x_delay(114);  //延时50us
  //GPIO_SetBits(WS281x_GPIOx, WS281x_GPIO_PIN);
}

/**
  * @brief  显示函数，将显存数据显示到灯珠上。
  * @param  None
  * @retval None
  */
void ws281x_show(void)
{
  for(uint16_t i = 0; i < PIXEL_NUM; ++i)
  {
    for(uint8_t j = 0; j < GRB; ++j)
    {
      if(pixelBuffer[i][j] == WS_HIGH)
      {
        ws281x_sendOne();
      }
      else
      {
        ws281x_sendTero();
      }
    }
  }
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

