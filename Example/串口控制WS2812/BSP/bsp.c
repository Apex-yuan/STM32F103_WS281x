#include "bsp.h"

void bsp_init(void)
{
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

  systick_init();
  usart1_init(115200);
  //delay_init();
  WS281x_Init();
}


