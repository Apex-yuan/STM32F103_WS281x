/**
  ******************************************************************************
  * @file    bsp_systick.c 
  * @author  Apex yuan
  * @version V1.0.0
  * @date    2019-8-16
  * @brief   systick
  ******************************************************************************
  * @attention
  ******************************************************************************
  */
#ifndef __SYSTICK_H
#define __SYSTICK_H

#ifdef __cplusplus
 extern "C" {
#endif
   
/* Includes ------------------------------------------------------------------*/  
#include "stm32f10x.h"  
   
#define delay(ms) delay_ms(ms)
   
void systick_init(void);
void systick_reset(void);    
void delay_ms(uint32_t _millis);
void delay_us(uint32_t uillis);
uint32_t millis(void);
uint32_t micros(void);
  
#ifdef __cplusplus
 }
#endif

#endif /*__SYSTICK_H */


