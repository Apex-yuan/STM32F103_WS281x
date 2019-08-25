#ifndef __BSP_WS281X_TEST_H
#define __BSP_WS281X_TEST_H

#include "stm32f10x.h"

#define WS281x_GPIO_CLK           RCC_APB2Periph_GPIOA
#define WS281x_GPIO_CLK_FUNCTION  RCC_APB2PeriphClockCmd
#define WS281x_GPIOx              GPIOA
#define WS281x_TIMx               TIM2
#define WS281x_TIM_CLK            RCC_APB1Periph_TIM2
#define WS281x_TIM_CLK_FUNCTION   RCC_APB1PeriphClockCmd
#define WS281x_DMA_CLK            RCC_AHBPeriph_DMA1
#define WS281x_DMA_CLK_FUNCTION   RCC_AHBPeriphClockCmd


#define EYE_GPIO_PIN           GPIO_Pin_0
#define EYE_TIM_DMA_SOURCE     TIM_DMA_CC1
#define EYE_DMA_PERIPH_ADDR    (uint32_t)&(TIM2->CCR1)
#define EYE_DMAx_CHANNELx      DMA1_Channel5
#define EYE_DMA_FLAG           DMA1_FLAG_TC5

#define EAR_GPIO_PIN           GPIO_Pin_1
#define EAR_TIM_DMA_SOURCE     TIM_DMA_CC2
#define EAR_DMA_PERIPH_ADDR    (uint32_t)&(TIM2->CCR2)
#define EAR_DMAx_CHANNELx      DMA1_Channel7
#define EAR_DMA_FLAG           DMA1_FLAG_TC7

/* 定义不同灯板代号 */
#define EYE   0x00
#define EAR   0x01

/* 定义各灯板灯珠数量 */
#define PIXEL_NUM  15
#define EYE_PIXEL_NUM  16
#define EAR_PIXEL_NUM  10

/* 定义当前灯珠的适配属性 */
#define GRB  24      //3*8
#define WS_HIGH  45  //根据通信时序确定此占空比
#define WS_LOW   30  //根据通信时序确定此占空比

/* 定义灯珠状态标记 */
#define WS281x_ON  1
#define WS281x_OFF 0

extern uint8_t ws281x_status[2];

void ws281x_init(void);
void ws281x_closeAll(uint8_t name);
void ws281x_rainbowCycle(uint8_t name, uint16_t num, uint8_t wait);
uint32_t ws281x_color(uint8_t red, uint8_t green, uint8_t blue);
void ws281x_setPixelColor(uint8_t name, uint16_t n ,uint32_t GRBcolor);
void ws281x_setPixelRGB(uint8_t name, uint16_t n ,uint8_t red, uint8_t green, uint8_t blue);
void ws281x_show(uint8_t name);

void ws281x_theaterChase(uint8_t name, uint16_t num, uint32_t c, uint8_t wait);
void ws281x_colorWipe(uint8_t name, uint16_t num, uint32_t c, uint8_t wait);
void ws281x_rainbow(uint8_t name, uint16_t num, uint8_t wait);
void ws281x_theaterChaseRainbow(uint8_t name, uint16_t num, uint8_t wait);
#endif /* __BSP_WS281X_TEST_H */

