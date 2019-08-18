#include "bsp.h"

void bsp_init(void)
{ 
  /* 配置中断优先级分组 */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

  systick_init();
  ws281x_init();
}


