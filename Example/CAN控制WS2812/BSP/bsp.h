#ifndef __BSP_H
#define __BSP_H

#ifdef __cplusplus
 extern "C" {
#endif
   
#include "stm32f10x.h"
   
#include "bsp_systick.h"
#include "bsp_usart1.h"
#include "bsp_ws281x_test.h"
#include "bsp_can.h"

void bsp_init(void);

#ifdef __cplusplus
 }
#endif

#endif /*__BSP_H */

