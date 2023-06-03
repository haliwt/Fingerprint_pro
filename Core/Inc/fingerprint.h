#ifndef __FINGERPRINT_H_
#define __FINGERPRINT_H_
#include "main.h"





typedef enum _color{

  
  color_off=0x00,
  blue_on = 0x01,
  green_on = 0x02,
  red_on = 0x04
  
  
}color;

typedef enum _fun_color{

   breath_led =0x01,
	 blank_led ,
	 open_led,
	 close_led
}fun_color;

typedef enum _runcmd{
	FP_SEARCH=0x01,
	FP_GEN_CHAR,
	FP_SEARCH_FAIL,
	FP_SEARCH_INIT,
    FP_SET_NEW_FP,
    FP_SET_INIT
}runcmd;

typedef struct _syspara
{

uint8_t 	PS_clear_ps_success;
	uint8_t 	PS_clear_ps_fail;
	uint8_t     ps_pre_detector;
	uint8_t     fp_rx_times;

  uint8_t PS_login_flag;
 
  uint8_t handler_read_data_flag;
  uint8_t ps_login_new_fp_success;
    
	
    
  uint8_t uart1_rx_data;
  uint8_t PS_read_template;
	
  uint8_t ps_error_times_key;
  uint8_t ps_judeg_read_templete_flag;
	
	//eeprom 
	uint8_t ps_readEeprom_data;
  uint8_t FP_RunCmd_Lable;
  uint8_t ps_serch_lable;
  uint8_t fp_administrator_flag;


	//search
	uint8_t ps_serch_getimage;
	uint8_t ps_serch_genchar;
	uint8_t ps_serach_result;

	uint8_t PS_login_times;
    uint8_t fp_login_key;

	uint8_t PS_save_numbers;
    uint8_t PS_exit_login_flag;
    uint8_t PS_exit_times;
    
    uint16_t ps_read_templete_numbers;
	
    
}SysPara;


extern SysPara syspara_t;


void Fingerprint_NewClinet_Login_Fun(void);

void RunCommand_Unlock_Fingerprint(void);





#endif 
