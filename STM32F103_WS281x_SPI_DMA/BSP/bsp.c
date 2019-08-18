#include "bsp.h"

void bsp_init(void)
{
  /*为了配合SPI_DMA方式驱动WS2812灯珠，通过修改system_stm32f10x.c文件，将系统主频配置为56M*/
  
  /* 配置中断优先级分组 */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

  systick_init();
  //delay_init();
  ws281x_init();
}


