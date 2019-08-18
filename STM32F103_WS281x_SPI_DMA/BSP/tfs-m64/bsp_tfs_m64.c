#include "bsp_tfs_m64.h"
#include "bsp_usart3.h"
#include "bsp_usart1.h"
#include "bsp_systick.h"

///指纹模块 ：Fingerprint module

/**
  * @brief  通过串口1接收到的指令，再通过串口3向指纹模块发送命令并接收响应指令
  * @param  none
  * @retval none
  */
void fpm_sendCmdBySerial(void)
{
  if(usart1_rx_cnt >= 8)
  {
    usart1_rx_cnt = 0;
    fpm_sendAndReceive(usart1_rx_buffer, 500);
  }
}
/**
  * @brief  通过串口3向指纹模块发送命令并接收响应指令
  * @param  cmdBuffer:命令数组，在调用该函数时只需将命令数组的CMD P1 P2 P3位赋值即可 
  * @param  timeOut：超时时间（单位：ms），在此时间内如果仍未接收到响应指令按超时处理
  * @retval 应答信息
  */
uint8_t fpm_sendAndReceive(uint8_t *cmdBuffer, uint16_t timeout)
{
  uint8_t  i, j;
  uint8_t checkSum = 0;  //校验码
	 
  usart3_rx_cnt = 0; //接收计数清零，相当于清空缓冲区
  
  cmdBuffer[HEAD] = 0xF5;
  cmdBuffer[TAIL] = 0xF5;
  cmdBuffer[5] = 0; //命令数据的第六位始终为零
  for(i = 1; i < 6; i++)
  {
    checkSum ^= cmdBuffer[i];
  }
  cmdBuffer[CHK]  = checkSum;
  
  USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);
  for(i = 0; i < 8; i++)
  {
    usart3_sendByte(cmdBuffer[i]);
  }
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
  
  while(usart3_rx_cnt < 8 && timeout > 0)
  {
    delay_ms(1);
    timeout--;		   
  }
  /*打印接收到的响应，调试用*/
   for(i = 0; i < 8; ++i)
  {
    printf("%02x ",usart3_rx_buffer[i]);
  }
  
  if (usart3_rx_cnt != 8) return ACK_TIMEOUT;
  if (usart3_rx_buffer[HEAD] != CMD_HEAD) return ACK_FAIL;
  if (usart3_rx_buffer[TAIL] != CMD_TAIL) return ACK_FAIL;
  if (usart3_rx_buffer[CMD] != (usart3_rx_buffer[CMD])) return ACK_FAIL;

  checkSum = 0; //校验码计算前必须清零
  for (j = 1; j < CHK; j++) 
  {   
    checkSum ^= usart3_rx_buffer[j];
  }
	if (checkSum != usart3_rx_buffer[CHK]) 
  {
    return ACK_FAIL;
  }
  
	 return ACK_SUCCESS;
}	 

/**
  * @brief  使指纹模块进入休眠模式
  * @param  none
  * @retval 应答信息（ACK_SUCCESS  ACK_FAIL）
  */
uint8_t fpm_sleep(void)
{
  uint8_t res;
  uint8_t cmdBuffer[8];
  
  cmdBuffer[CMD] = CMD_SLEEP_MODE;
  cmdBuffer[P1] = 0;
  cmdBuffer[P2] = 0;
  cmdBuffer[P3] = 0;
  
  res = fpm_sendAndReceive(cmdBuffer, 500);
  
  if(res == ACK_SUCCESS) {
    return ACK_SUCCESS;
  }
  else {
    return ACK_FAIL;
  }
  
}

/**
  * @brief  设置指纹的添加模式
  * @param  mode：指纹的添加模式（0：允许重复 1：禁止重复）
  * @retval 应答信息（ACK_SUCCESS  ACK_FAIL）
  */
uint8_t fpm_setAddMode(uint8_t mode)
{
  uint8_t res;
  uint8_t cmdBuffer[8];
  
  cmdBuffer[CMD] = CMD_ADD_MODE;
  cmdBuffer[P1] = 0;
  cmdBuffer[P2] = mode;
  cmdBuffer[P3] = 0;
  
  res = fpm_sendAndReceive(cmdBuffer, 200);
  
  if(res == ACK_SUCCESS && usart3_rx_buffer[Q3] == ACK_SUCCESS) {
    return ACK_SUCCESS;
  }
  else {
    return ACK_FAIL;
  }
}

/**
  * @brief  读取指纹的添加模式
  * @param  none
  * @retval 应答信息（0：允许重复 1：禁止重复）
  */
uint8_t fpm_readAddMode(void)
{ 
  uint8_t cmdBuffer[8];
  
  cmdBuffer[CMD] = CMD_ADD_MODE;
  cmdBuffer[P1] = 0;
  cmdBuffer[P2] = 0;
  cmdBuffer[P3] = 0X01;
  
  fpm_sendAndReceive(cmdBuffer, 200);
  
  return usart3_rx_buffer[Q2];
}

/**
  * @brief  获取用户数（实际上应该是指纹数，这里用用户数表示是为了与通信协议表述一致）
  * @param  none
  * @retval 应答信息（指纹的数目）
  */
uint16_t fpm_getUserNum(void)
{
  uint8_t res;
  uint8_t cmdBuffer[8];
  
  cmdBuffer[CMD] = CMD_USER_CNT;
  cmdBuffer[P1] = 0;
  cmdBuffer[P2] = 0;
  cmdBuffer[P3] = 0;
  
  res = fpm_sendAndReceive(cmdBuffer, 200);
  
  if(res == ACK_SUCCESS && usart3_rx_buffer[Q3] == ACK_SUCCESS) {
    return usart3_rx_buffer[Q2];
  }
  else {
    return 0XFF;
  }
  
}

/**
  * @brief  删除所有的指纹
  * @param  none
  * @retval 应答信息（ACK_SUCCESS  ACK_FAIL）
  */
uint8_t fpm_deleteAllUser(void)
{
  uint8_t res;
  uint8_t cmdBuffer[8];
  
  cmdBuffer[CMD] = CMD_DEL_ALL;
  cmdBuffer[P1] = 0;
  cmdBuffer[P2] = 0;
  cmdBuffer[P3] = 0;
  
  res = fpm_sendAndReceive(cmdBuffer, 200);
  
  if(res == ACK_SUCCESS && usart3_rx_buffer[Q3] == ACK_SUCCESS) {
    return ACK_SUCCESS;
  }
  else {
    return ACK_FAIL;
  }
}

/**
  * @brief  删除制定位置的指纹
  * @param  userNum：指纹的位置（1-255）
  * @retval 应答信息（ACK_SUCCESS  ACK_FAIL）
  */
uint8_t fpm_deleteUser(uint8_t userNum)
{
  uint8_t res;
  uint8_t cmdBuffer[8];
  
  cmdBuffer[CMD] = CMD_DEL;
  cmdBuffer[P1] = 0;
  cmdBuffer[P2] = userNum;
  cmdBuffer[P3] = 0;
  
  res = fpm_sendAndReceive(cmdBuffer, 200);
  
  if(res == ACK_SUCCESS && usart3_rx_buffer[Q3] == ACK_SUCCESS) {
    return ACK_SUCCESS;
  }
  else {
    return ACK_FAIL;
  }
}

/**
  * @brief  录入指纹
  * @param  userNum：指纹存入的位置（1-255）
  * @param  userPermission：用户权限（1-3），具体含义自行定义。
  * @retval 应答信息（ACK_SUCCESS  ACK_FAIL  ACK_USER_EXIST  ACK_TIMEOUT）
  */
uint8_t fpm_addUser(uint8_t userNum, uint8_t userPermission)
{
  uint8_t res;
  uint8_t cmdBuffer[8];
  
  cmdBuffer[CMD] = CMD_ADD_1;
  cmdBuffer[P1] = 0;
  cmdBuffer[P2] = userNum;
  cmdBuffer[P3] = userPermission;
  
  res = fpm_sendAndReceive(cmdBuffer, 30000);

  if(res == ACK_SUCCESS) {
    if(usart3_rx_buffer[Q3] == ACK_SUCCESS) {
      cmdBuffer[CMD] = CMD_ADD_2;
      
      res = fpm_sendAndReceive(cmdBuffer, 30000);
      
      if(res == ACK_SUCCESS) {
        if(usart3_rx_buffer[Q3] == ACK_SUCCESS) {
          cmdBuffer[CMD] = CMD_ADD_3;
          
          res = fpm_sendAndReceive(cmdBuffer, 30000);
          
          if(res == ACK_SUCCESS) {
            return usart3_rx_buffer[Q3];
          }  
        }
      }
    }
  }
  return res;
 
}

/**
  * @brief  通过1：N的方式比较指纹
  * @param  none
  * @retval 匹配指纹的信息
  */
uint8_t fpm_compareFinger(void)
{
  uint8_t res;
  uint8_t cmdBuffer[8];
  
  cmdBuffer[CMD] = CMD_MATCH;
  cmdBuffer[P1] = 0;
  cmdBuffer[P2] = 0;
  cmdBuffer[P3] = 0;
  
  res = fpm_sendAndReceive(cmdBuffer, 30000);
  
  if(res == ACK_SUCCESS) 
  {
    if(usart3_rx_buffer[Q3] == ACK_NOUSER) {
      return ACK_NOUSER;
    }
    if(usart3_rx_buffer[Q3] == ACK_TIMEOUT) {
      return ACK_TIMEOUT;
    }
    if((usart3_rx_buffer[Q2] != 0) && (usart3_rx_buffer[Q3] == 1 || usart3_rx_buffer[Q3] == 2 || usart3_rx_buffer[Q3] == 3)) {
      return ACK_SUCCESS;
    }
  }
  return res;
}

