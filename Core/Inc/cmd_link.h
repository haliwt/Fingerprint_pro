#ifndef __CMD_LINK_H_
#define __CMD_LINK_H_
#include "main.h"


#define USART1_MAX_RECV_LEN			20					//×î´ó½ÓÊÕ»º´æ×Ö½ÚÊý
#define USART1_MAX_SEND_LEN			20					//×î´ó·¢ËÍ»º´æ×Ö½ÚÊý
#define USART11_RX_EN 				1					//0,²»½ÓÊÕ;1,½ÓÊÕ.

extern uint8_t  USART1_RX_BUF[USART1_MAX_RECV_LEN]; 		//½ÓÊÕ»º³å,×î´óUSART3_MAX_RECV_LEN×Ö½Ú
extern uint8_t  USART1_TX_BUF[USART1_MAX_SEND_LEN]; 		//·¢ËÍ»º³å,×î´óUSART3_MAX_SEND_LEN×Ö½Ú
extern uint8_t  UART1_RX_DataBuf[1];
extern uint16_t USART1_RX_STA;   						//½ÓÊÕÊý¾Ý×´Ì¬








#endif 
