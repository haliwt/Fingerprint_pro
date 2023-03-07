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


extern uint8_t virtualPwd[20];

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

typedef enum _tk_sate{
   INPUT_NORMAL_NUMBERS,
   INPUT_NEW_PWD
 }touchkey_state_t;




typedef struct __RUN_T{

   //power ref
   unsigned char powerOn;
	unsigned char error_times;
	unsigned char panel_lock;
	uint8_t ADC_times;
	uint8_t pwd_fp_label;
	

	//lower power ref
	uint8_t lowPower_flag;

	
	
  
	//touchkey 
	uint8_t touchkey_first;
   	uint8_t inputNewPassword_Enable;
	uint8_t  getSpecial_1_key;
	uint8_t  getSpecial_2_key;
	uint8_t  getNumbers_key;
	uint8_t NumbersKey_pressedNumbers;
	uint8_t enter_key ;
	uint8_t confirm_key_label;
	uint8_t cancel_key_label;
	uint8_t special_key_flag;
	uint8_t readI2C_data;
	uint8_t touchkey_first_turn_on_led;

	
	//motor ref
	uint8_t motor_returnRun_flag;
	uint8_t motor_return_homePosition;
	uint8_t Motor_RunCmd_Label;

    //led ref
    uint8_t inputNewPwd_OK_led_blank_times;
	uint8_t works_led_label;


	//buzzer sound

	uint8_t buzzer_sound_label;

	//pass word ref
   unsigned char Numbers_counter;

   uint8_t password_unlock_model;
 
   unsigned char Confirm_newPassword;
	unsigned char inputNewPasswordTimes;
	uint8_t clear_inputNumbers_newpassword;
	
	//factory ref
   unsigned char factory_test;

   //eeprom ref
   unsigned char eepromAddress;
   uint8_t  saveEEPROM_fail_flag ; //WT.EDIT 2022.10.06	

	uint8_t  clearEeeprom_count;
	unsigned char clearEeprom;
	uint8_t readEepromData;

	

	//run ref

	uint8_t open_lock_lable;

   
	//timging
    uint8_t gTimer_motor_reverse;

	unsigned char gTimer_60s;
	unsigned char gTimer_8s;
	unsigned char gTimer_ADC;
	uint8_t  gTimer_factory;

	
	

	uint8_t gTimer_led_blink_500ms;
	
	uint8_t 	motorRunCount;
	

	uint32_t userId;
   
	
}RUN_T;

typedef struct _tag_pwd_fpStruct{

   void (*Pwd_Handler)(void);
   void (*Fp_Handler)(void);


}pwd_fp_t;

extern RUN_T run_t;    //password struct 
extern RUN_T fp_t;     //fingerprint struct 

extern pwd_fp_t pwd_fun_t;
extern pwd_fp_t fp_fun_t;

extern uint8_t pwd1[6];
extern uint8_t pwd2[6];
extern uint8_t Readpwd[6];



extern void (*Pwd_fun)(void);
extern void (*fp_fun)(void);
extern void (*RunChed_KeyMode)(uint16_t keydat);
void RunCheck_Mode(uint16_t dat);
void RunCommand_Unlock_Keyboard(void);





void CParserDispatch(void);


void RunCheck_KeyMode_Handler(void(*keymode_handler)(uint16_t keydat));



unsigned char CompareValue(uint8_t *pt1,uint8_t *pt2);

unsigned char  InputNumber_ToSpecialNumbers(TouchKey_Numbers number);

uint8_t OverNumbers_Password_Handler(void);

#endif 



