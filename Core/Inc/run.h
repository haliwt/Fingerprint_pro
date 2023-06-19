#ifndef __RUN_H_
#define __RUN_H_
#include "main.h"

#define ADMINI     		0x00 //0X00
#define USER_1     		0X08
#define USER_2     		0X10
#define USER_3     		0X18
#define USER_4    	 	0X20  
#define USER_5     		0X28
#define USER_6    	 	0X30
#define USER_7    	 	0X38  
#define USER_8     		0X40
#define USER_9     		0x48
#define USER_10    	 	0x50
#define USER_11         0x58

#define ADMIN_SAVE_ADD         0x80  //administrator of be save 
#define USER_SAVE_ADD_1        0X81
#define USER_SAVE_ADD_2        0x82
enum Signal {                   /* enumeration for CParser signals */
     TOUCH_KEY_SIG,IN_NUMBER_SIG,  ZERO_SIG
};


enum State {                     /* enumeration for CParser states */
   INPUTKEY, MODIFYPSWD, EXIT
};

//new FPC board
typedef enum 
{
   SPECIAL_1 =0x8000,KEY_1=0x100, KEY_2=0x80, KEY_3=0x40, KEY_4=0x20, KEY_5=0x10,
   KEY_6= 0x4000, KEY_7=0x2000, KEY_8=0x1000, KEY_9=0x800, KEY_0=0x400, SPECIAL_2=0x200

}TouchKey_Numbers;

typedef enum _open_state{
	open_lock_success = 0x01,
	open_lock_fail,
	open_lock_null

}open_state;

typedef enum{

    LOGIN_FAIL =0x01,
	LOGIN_SUCCESS

}login_new_password;


typedef enum _unlock_state{

	UNLOCK_NULL,
	UNLOCK_FAIL,
	UNLOCK_SUCCESS,
	
}unlock_state_t;

typedef enum _new_password_save_t{

    NEW_PWD_SAVE_DATA_TO_EEPROM=0x01,
	UNLOCK_OVER_MAX_PWD_NUMBERS



}new_password_save_t;



typedef struct __RUN_T{

   //power ref
   unsigned char powerOn;
	unsigned char error_times;
	unsigned char panel_lock;
	uint8_t ADC_times;
	uint8_t keyPressed_flag;
	uint8_t thefirst_side_key;

	//usart
	 uint8_t gTimer_usart_error ;

	//lower power ref
	uint8_t lowPower_flag;
	uint8_t inputDeepSleep_times;
	uint8_t confirm_button_flag;

	//numbers ref
	uint8_t clear_virtual_numbers;
	uint8_t readpwd_array_length;
	uint8_t new_password_the_first_numbers;

	
  
	//touchkey 
	uint8_t touchkey_first;
   uint8_t inputNewPassword_Enable;
	uint8_t   getSpecial_1_key;
	uint8_t   getSpecial_2_key;
	uint8_t   getNumbers_key;
	uint8_t NumbersKey_pressedNumbers;
	uint8_t password_unlock;
    uint8_t oneself_copy_behavior;

	
	//motor ref
   uint8_t motor_doing_flag;
	uint8_t motor_returnRun_flag;
	uint8_t motor_return_homePosition;
	uint8_t Motor_RunCmd_Label;
	uint8_t returnHomePosition_Count;
	uint8_t gTimer_total_backling_off;

    //led ref
    uint8_t backlight_run_flag;
	uint8_t backlight_Cmd_lable;
	uint8_t inputNewPwd_OK_led_blank_times;
	uint8_t works_led_lable;
   // backlight ref
    uint8_t backlight_label;

	//login ref
    uint8_t input_digital_key_number_counter;

	
	//buzzer sound
	uint8_t buzzer_sound_tag;

	uint8_t buzzer_sound_lable;

	//pass word ref
   unsigned char Numbers_counter;
	unsigned char passwordsMatch;
   uint8_t password_unlock_model;
   uint8_t input_newPassword_over_number;
   unsigned char Confirm_newPassword;
	unsigned char inputNewPasswordTimes;
	uint8_t clear_inputNumbers_newpassword;
	uint8_t add_new_password_success;
	
	//factory ref
   unsigned char factory_test;

   //eeprom ref
   unsigned char eepromAddress;
   uint8_t  saveEEPROM_fail_flag ; //WT.EDIT 2022.10.06	

	uint8_t  clearEeeprom_count;
	unsigned char clearEeprom;
	uint8_t readEepromData;

	//run ref
	uint8_t clearEeeprom_done;
	uint8_t open_lock_lable;
	uint8_t touchkey_save_data_flag;

   
	//timging

	unsigned char gTimer_10s;
	unsigned char gTimer_8s;
	unsigned char gTimer_60s;
	unsigned char gTimer_ADC;
	uint8_t  gTimer_input_standby_cnt;
	
	uint8_t  gTimer_input_error_times_60s;
	uint8_t gTimer_10s_start;
	uint8_t gTimer_led_blink_500ms;
	uint8_t gTimer_factory_times_60s;
	
	uint16_t 	motorRunCount;
	

	uint32_t userId;
   
	
}RUN_T;


extern RUN_T run_t;
extern uint8_t pwd1[6];
extern uint8_t pwd2[6];
extern uint8_t Readpwd[6];



extern void (*ClearVirtual_Numbers)(void);

extern void (*RunChed_KeyMode)(uint16_t keydat);
extern uint8_t (*Default_Read_Administrator_Pwd)(void);



void Run_Init(void);



void RunCommand_Unlock_Keyboard(void);

void RunCommand_Unlock(void);


void SavePassword_To_EEPROM(void);

void CParserDispatch(void);


void RunCheck_KeyMode_Handler(void(*keymode_handler)(uint16_t keydat));

void ReadPassword_EEPROM_SaveData(void);

uint8_t CompareValue(uint8_t *pt1,uint8_t *pt2,uint8_t length);


uint8_t  InputNumber_ToSpecialNumbers(TouchKey_Numbers number);

uint8_t OverNumbers_Password_Handler(void);

void Clear_VirtualArray_Numbers(void(*clear_virtual)(void));
void Default_Read_Administrator_Handler( uint8_t (*default_read_pwd)(void));

void Enter_Key_Fun(void);

#endif 



