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


typedef struct __RUN_T{
	//enum State state_;
    //unsigned char cmdCtr_;   //event order cunter

    unsigned char powerOn;
	uint8_t   getTouchkey;
	uint8_t   getSpecial_1_key;
	uint8_t   getSpecial_2_key;
	uint8_t   getNumbers_key;
	uint8_t   RunSequence;
	


  
	uint8_t  saveEEPROM_fail_flag ; //WT.EDIT 2022.10.06	
	uint8_t  eeprom_Reset_flag;
	uint8_t  clearEeeprom_count;
	
	
	
	unsigned char Confirm_newPassword;
	unsigned char inputNewPasswordTimes;

	//password

	unsigned char motor_return_homePosition;
    unsigned char error_times;
	unsigned char panel_lock;
	unsigned char lock_fail;
	unsigned char led_blank ;
	unsigned char inputDeepSleep_times;
	unsigned char clearEeprom;
    
    unsigned char eepromAddress;
    uint8_t ADC_times;
	
	//touchkey 
	uint8_t touchkey_first_turn_on_led;
	uint8_t touchkey_first;
	uint8_t readI2C_data;
    uint8_t inputNewPassword_Enable;

	
	//motor
    uint8_t clear_inputNumbers_newpassword;
	uint8_t inputNewPwd_OK_led_blank_times;
	uint8_t motor_doing_flag;
	uint8_t motor_returnRun_flag;

    
    
	unsigned char BackLight;
	uint8_t login_in_success;
	
	//buzzer sound
	unsigned char buzzer_key_sound_flag;
	uint8_t buzzer_two_short;
    uint8_t buzzer_longsound_flag;
	uint8_t buzzer_highsound_flag;
	uint8_t buzzer_fail_sound_flag;

	//pass word

	uint8_t  open_lock_fail;
	uint8_t  open_lock_success;
	unsigned char Numbers_counter;
	unsigned char passwordsMatch;
    uint8_t password_unlock_model;
	uint8_t       Led_OK_flag;
	uint8_t 	  Led_ERR_flag ;
	
    

	unsigned char factory_test;
	uint8_t lowPower_flag;
	uint8_t oneself_copy_behavior;
	uint8_t detection_input_flag;
  
	
	uint8_t NumbersKey_pressedNumbers;

	uint8_t clearEeeprom_done;
	uint8_t normal_works_state ;
	uint8_t input_newPassword_over_number;
	uint8_t stop_gTimer_8s;
	uint8_t input_lowpower_stopmodel_flag;


	uint8_t readEepromData;
   
	

	uint8_t gTimer_200ms;
	unsigned char gTimer_10s;
	unsigned char gTimer_1s;
	unsigned char gTimer_8s;
	unsigned char gTimer_60s;
	unsigned char gTimer_ADC;
	
	uint8_t  	gTimer_input_error_times_60s;
	uint8_t 	gTimes_s;
	uint8_t 	gTimer_10s_start;


	uint16_t 	motorRunCount;
	uint16_t   	returnHomePosition_Count;

	uint32_t userId;
   
	
}RUN_T;


extern RUN_T run_t;
extern uint8_t pwd1[6];
extern uint8_t pwd2[6];
extern uint8_t Readpwd[6];




extern void (*RunChed_KeyMode)(uint16_t keydat);
void RunCheck_Mode(uint16_t dat);
void RunCommand_Unlock_Keyboard(void);



void SavePassword_To_EEPROM(void);

void CParserDispatch(void);


void RunCheck_KeyMode_Handler(void(*keymode_handler)(uint16_t keydat));

void ReadPassword_EEPROM_SaveData(void);

unsigned char CompareValue(uint8_t *pt1,uint8_t *pt2);

unsigned char  InputNumber_ToSpecialNumbers(TouchKey_Numbers number);
void Lock_Open_Order(void);

void RunCommand_Unlock_Fingerprint(void);

#endif 



