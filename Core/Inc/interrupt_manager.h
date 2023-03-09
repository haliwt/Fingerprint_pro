#ifndef __INTERRUPT_MANAGER_H_
#define __INTERRUPT_MANAGER_H_
#include "main.h"

typedef enum{

   RX_FAIL,
   RX_SUCCESS,

}fp_rx_state;

typedef enum{

   package_head_one,
   package_head_two,
   device_address_one,
   device_address_two,
   device_address_three,
   device_address_four,
   package_id,
   package_length_one,
   package_length_two,
   confirm_code,
   ref_one,
   ref_two,
   checksum_one,
   checksum_two, //13 =0x0d
   rx_receive_data_over
   
}fp_login_t;

typedef struct _rx_data{

     uint8_t decoder_flag ;
	 uint8_t auto_register_flag;
     uint8_t fp_rx_receive_error_data_flag;
	 uint8_t buzzer_flag;
	 uint8_t rx_fp_package_length_one;
	 uint8_t rx_fp_package_length_two;
	 uint8_t confirm_code;
	 
   

	 uint8_t fp_rx_data[3];
	 uint8_t fp_rx_checksum[2];



}RX_T;

extern RX_T rx_t;

#define USART1_MAX_RECV_LEN			20					//×î´ó½ÓÊÕ»º´æ×Ö½ÚÊý
#define USART1_MAX_SEND_LEN			50					//×î´ó·¢ËÍ»º´æ×Ö½ÚÊý
#define USART11_RX_EN 				1					//0,²»½ÓÊÕ;1,½ÓÊÕ.

extern uint8_t  USART1_RX_BUF[USART1_MAX_RECV_LEN]; 		//½ÓÊÕ»º³å,×î´óUSART3_MAX_RECV_LEN×Ö½Ú
extern uint8_t  USART1_TX_BUF[USART1_MAX_SEND_LEN]; 		//·¢ËÍ»º³å,×î´óUSART3_MAX_SEND_LEN×Ö½Ú
extern uint8_t  UART1_RX_DataBuf[1];
extern uint16_t USART1_RX_STA;   						//½ÓÊÕÊý¾Ý×´Ì¬



#endif 

