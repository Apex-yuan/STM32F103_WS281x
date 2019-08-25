#ifndef __ESP8266_H
#define __ESP8266_H

#include "stm32f10x.h"
#include "stdbool.h"

/******************************* ESP8266 数据类型定义 ***************************/
typedef enum{
	STA,
  AP,
  STA_AP  
} ESP8266_ModeEnumDef;


typedef enum{
	 enumTCP,
	 enumUDP,
} ESP8266_ProtocolEnumDef;
	

typedef enum{
	Multiple_ID_0 = 0,
	Multiple_ID_1 = 1,
	Multiple_ID_2 = 2,
	Multiple_ID_3 = 3,
	Multiple_ID_4 = 4,
	Single_ID_0 = 5,
} ENUM_ID_NO_TypeDef;
	

typedef enum{
	OPEN = 0,
	WEP = 1,
	WPA_PSK = 2,
	WPA2_PSK = 3,
	WPA_WPA2_PSK = 4,
} ESP8266_ApPwdModeEnumDef;


#define ESP8266_USART_BOUND  115200

/******************************** ESP8266 连接引脚定义 ***********************************/
#define      ESP8266_CH_PD_CLK                             RCC_APB2Periph_GPIOB  
#define      ESP8266_CH_PD_PORT                            GPIOB
#define      ESP8266_CH_PD_PIN                             GPIO_Pin_13

#define      ESP8266_RST_CLK                               RCC_APB2Periph_GPIOB
#define      ESP8266_RST_PORT                              GPIOB
#define      ESP8266_RST_PIN                               GPIO_Pin_14


#define   ESP8266_BUILD_AP_SSID         "ESP8266"      //要建立的热点的名称
#define   ESP8266_BUILD_AP_ECN          (ESP8266_ApPwdModeEnumDef)4               //要建立的热点的加密方式
#define   ESP8266_BUILD_AP_PWD           "12345678"         //要建立的热点的密钥

#define   ESP8266_TCPSERVER_IP         "192.168.1.115"    //服务器开启的IP地址
#define   ESP8266_TCPSERVER_PORT       "8086"             //服务器开启的端口   

#define   ESP8266_TCPSERVER_TIMEOUT    "5000"             //服务器超时时间（单位：秒）

#define   ESP8266_AP_SSID              "stupid-kou"
#define   ESP8266_AP_PWD               "wmk6053003"



void esp8266_init(void);
void esp8266_reset(void);
bool esp8266_sendCmd(char *cmd, char *reply, uint16_t wait);
bool esp8266_joinAP(char *pSSID, char *pPassword);
bool esp8266_buildAP(char * pSSID, char * pPwd, ESP8266_ApPwdModeEnumDef pwdMode);
bool esp8266_multipleIdCmd(FunctionalState state);
bool esp8266_startServer ( char * pPortNum );
bool esp8266_setTimeout(char * timeout);
bool esp8266_getAPIP (char * pApIp);
bool esp8266_modeChoose(ESP8266_ModeEnumDef mode);


#endif /* __ESP8266_H */

