#ifndef __BSP_WS281X_H
#define __BSP_WS281X_H

#include "stm32f10x.h"


#define WS281x_GPIO_CLK           RCC_APB2Periph_GPIOA
#define WS281x_GPIO_CLK_FUNCTION  RCC_APB2PeriphClockCmd
#define WS281x_GPIOx              GPIOA
#define WS281x_GPIO_PIN           GPIO_Pin_7

#define WS281x_SPIx               SPI1
#define WS281x_SPI_CLK            RCC_APB2Periph_SPI1
#define WS281x_SPI_CLK_FUNCTION   RCC_APB2PeriphClockCmd
#define WS281x_SPI_DMA_SOURCE     SPI_I2S_DMAReq_Tx

#define WS281x_DMA_CLK            RCC_AHBPeriph_DMA1
#define WS281x_DMA_CLK_FUNCTION   RCC_AHBPeriphClockCmd
#define WS281x_DMA_PERIPH_ADDR    (uint32_t)&(SPI1->DR)
#define WS281x_DMAx_CHANNELx      DMA1_Channel3
#define WS281x_DMA_FLAG           DMA1_FLAG_TC3


#define PIXEL_NUM  15
#define GRB  24   //3*8

//硬件spi模拟ws2811时序（用spi的8位数据模拟ws281x的一位数据）
//要将系统时钟设置为56M，分频数设置为8，则SPI的通信频率为7M，传输一位数据的时间约为143纳秒（ns）
//3*143 = 429ns   5*143 = 715ns  符合WS281X芯片的通信时序。
//  _____   
// |     |___|   11111000  high level
//  ___         
// |   |_____|   11100000  low level

#define WS_HIGH 0XF8
#define WS_LOW  0XE0

void ws281x_init(void);
void ws281x_closeAll(void);
void ws281x_rainbowCycle(uint8_t wait);
uint32_t ws281x_color(uint8_t red, uint8_t green, uint8_t blue);
void ws281x_setPixelColor(uint16_t n ,uint32_t GRBcolor);
void ws281x_show(void);

void ws281x_theaterChase(uint32_t c, uint8_t wait);
void ws281x_colorWipe(uint32_t c, uint8_t wait);
void ws281x_rainbow(uint8_t wait);
void ws281x_theaterChaseRainbow(uint8_t wait);
#endif /* __BSP_WS281X_H */

