#ifndef __CMD_LINK_H_
#define __CMD_LINK_H_
#include "main.h"

#define SYSTEM_EXTERN   extern

//=============================================================================

//=============================================================================
#define         RESET_FP_ERROR                0
#define         RESET_FP_SUCCESS              1

//=============================================================================
//wifi配置复位状态
//=============================================================================
#define         FP_ERROR            0
#define         FP_SUCCESS          1



#define FP_UART_RECV_BUF_LMT          20              //串口数据接收缓存区大小,
#define FP_DATA_PROCESS_LMT           50              //串口数据处理缓存区大小,
#define FP_UART_SEND_BUF_LMT     	  4           //根据用户SEND数据大小量定,必须大1



#define USART1_MAX_RECV_LEN			40					//×î´ó½ÓÊÕ»º´æ×Ö½ÚÊý
#define USART1_MAX_SEND_LEN			20					//×î´ó·¢ËÍ»º´æ×Ö½ÚÊý
#define USART11_RX_EN 				1					//0,²»½ÓÊÕ;1,½ÓÊÕ.

extern uint8_t  USART1_RX_BUF[USART1_MAX_RECV_LEN]; 		//½ÓÊÕ»º³å,×î´óUSART3_MAX_RECV_LEN×Ö½Ú
extern uint8_t  USART1_TX_BUF[USART1_MAX_SEND_LEN]; 		//·¢ËÍ»º³å,×î´óUSART3_MAX_SEND_LEN×Ö½Ú
extern uint8_t  UART1_RX_DataBuf[1];
extern uint16_t USART1_RX_STA;   						//½ÓÊÕÊý¾Ý×´Ì¬


//=============================================================================
//Byte order of the frame
//=============================================================================
#define         HEAD_FIRST                      0  // 1byte
#define         HEAD_SECOND                     1  // 1byte      
#define         DEVICE_ADDRESS                  4   // 4 bytes 
#define         PACKAGE_ID                      6   // 1 bytes
#define         CONFIRM_CODE                    9
#define         PACKAGE_LENGTH                  2   // 2 bytes 
#define         CHECK_SUM                       12   // 2 bytes 



//=============================================================================
//Byte order of the frame
//=============================================================================
#define         PACKAGE_CMD                   			0x01
#define         PACKAGE_DATA_CONTINUE                	0x02        
#define         PACKAGE_DATA_END                		0x08
#define         RESPONSE_PACKAGE_LEN                    0x0C
#define         PACKAGE_ID_DATA                         0x07
#define         LENGTH_HIGH                     		4
#define         LENGTH_LOW                     	 		5
#define         DATA_START                     		 	6

#define         PACKAGE_SEND_ID(x)                      x
#define         PACKAGE_SEND_ORDER(x)                   x  

//=============================================================================

#define PROTOCOL_HEAD           0x55//0x07                     //握手协议
#define FRAME_FIRST             0xEF// 0x55                    //帧头第一字节
#define FRAME_SECOND            0x01// 0xaa                    //帧头第二字节
//============================================================================= 
SYSTEM_EXTERN volatile unsigned char fp_data_process_buf[FP_DATA_PROCESS_LMT];     //串口数据处理缓存
SYSTEM_EXTERN volatile unsigned char fp_uart_rx_buf[FP_UART_RECV_BUF_LMT];         //串口接收缓存
SYSTEM_EXTERN volatile unsigned char fp_uart_tx_buf[FP_UART_SEND_BUF_LMT];        //串口发送缓存
//
SYSTEM_EXTERN volatile unsigned char *rx_buf_in;
SYSTEM_EXTERN volatile unsigned char *rx_buf_out;

SYSTEM_EXTERN volatile unsigned char stop_update_flag;      


SYSTEM_EXTERN uint8_t fp_OutputBuf[];


void Fp_uart_write_frame(unsigned char fr_type, unsigned char fr_ver, unsigned short len);
void Fp_uart_service(void);
void uart_receive_input(unsigned char value);
void Fp_uart_service(void);
void Fp_usart_receive_data_handle(unsigned short offset);


#endif 
