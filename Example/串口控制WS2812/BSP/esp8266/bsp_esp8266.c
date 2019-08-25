#include "bsp_esp8266.h"
#include "bsp_usart1.h"
#include "bsp_usart3.h"
#include "bsp_systick.h"
#include <string.h>


void esp8266_init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  usart3_init(ESP8266_USART_BOUND);
  
  RCC_APB2PeriphClockCmd(ESP8266_CH_PD_CLK | ESP8266_RST_CLK, ENABLE);
  
  GPIO_InitStructure.GPIO_Pin = ESP8266_CH_PD_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(ESP8266_CH_PD_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = ESP8266_RST_PIN;
  GPIO_Init(ESP8266_RST_PORT, &GPIO_InitStructure);
  
  GPIO_SetBits(ESP8266_RST_PORT, ESP8266_RST_PIN); //低电平复位，高电平工作
  GPIO_SetBits(ESP8266_CH_PD_PORT, ESP8266_CH_PD_PIN); //CH_PD:高电平工作，低电平供电关掉

}

bool esp8266_sendCmd(char *cmd, char *reply, uint16_t wait)
{
  rxFram.length = 0; //清空数据帧，重新接收
  usart3_printf("%s\r\n", cmd);
  if(reply == 0)
    return true;

  delay_ms(wait);
  
  rxFram.rxbuffer[rxFram.length] = '\0'; //为接收到的帧数据添加字符串结束标记
  printf("%s",rxFram.rxbuffer);  //将数据发送到串口上位机
  return ((bool) strstr(rxFram.rxbuffer,reply));
}

void esp8266_reset(void)
{
  GPIO_ResetBits(ESP8266_RST_PORT,ESP8266_RST_PIN);
  delay_ms(500);
  GPIO_SetBits(ESP8266_CH_PD_PORT, ESP8266_CH_PD_PIN);
}

bool esp8266_modeChoose(ESP8266_ModeEnumDef mode)
{
  switch(mode)
  {
    case STA:
      return esp8266_sendCmd("AT+CWMODE=1", "OK", 1500);
    case AP:
      return esp8266_sendCmd("AT+CWMODE=2", "OK", 1500);
    case STA_AP:
      return esp8266_sendCmd("AT+CWMODE=3", "OK", 1500);
    default:
      return false;
  }
}

bool esp8266_joinAP(char *pSSID, char *pPassword)
{
  char cmd[120];
  sprintf(cmd, "AT+CWJAP=\"%s\",\"%s\"", pSSID, pPassword);
  return esp8266_sendCmd(cmd,"OK", 5000);
}

bool esp8266_buildAP(char * pSSID, char * pPwd, ESP8266_ApPwdModeEnumDef pwdMode)
{
  char cmd[120];
  sprintf(cmd, "AT+CWSAP=\"%s\",\"%s\",1,%d",pSSID, pPwd, pwdMode);
  return esp8266_sendCmd(cmd, "OK", 1000);
}

bool esp8266_multipleIdCmd(FunctionalState state)
{
  char cmd[20];
  sprintf(cmd, "AT+CIPMUX=%d",state);
  return esp8266_sendCmd(cmd, "OK", 500);
}

bool esp8266_startServer ( char * pPortNum )
{
	char cmd [120];
	sprintf (cmd, "AT+CIPSERVER=%d,%s", 1, pPortNum );
	return( esp8266_sendCmd(cmd, "OK", 500));
}

bool esp8266_setTimeout(char * timeout)
{
  char cmd[20];
  sprintf(cmd, "AT+CIPSTO=%s", timeout);
  return esp8266_sendCmd(cmd, "OK", 500);
}

//bool esp8266_getApIp()
bool esp8266_getAPIP (char * pApIp)
{
	char uc;
	char * pCh;
	
	
  esp8266_sendCmd("AT+CIFSR", "OK",500);
	
	pCh = strstr ( rxFram.rxbuffer, "APIP,\"" );
	
	if(pCh != 0)
		pCh += 6;
	else
		return 0;
	
	for ( uc = 0; uc < 20; uc ++ )
	{
		pApIp [ uc ] = * ( pCh + uc);
		
		if ( pApIp [ uc ] == '\"' )
		{
			pApIp [ uc ] = '\0';
			break;
		}		
	}
	return 1;
}

void esp8266_AT_Test ( void )
{
	char count=0;
	
	GPIO_SetBits(ESP8266_RST_PORT,ESP8266_RST_PIN);//macESP8266_RST_HIGH_LEVEL();	
	delay_ms ( 1000 );
	while ( count < 10 )
	{
		if( esp8266_sendCmd ( "AT", "OK", 500 ) ) return;
		esp8266_reset();
		++ count;
	}
}


