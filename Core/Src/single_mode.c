#include "single_mode.h"
#include "run.h"
#include "led.h"
#include "touchkey.h"
#include "motor.h"
#include "buzzer.h"
#include "key.h"
#include "delay.h"
#include "tim.h"
#include "as608.h"
#include "fingerprint.h"





static void Save_To_EeepromNewPwd(void);
static void NewPwd_SaveData_Handler(void);


void (*FP_ReadData_Handler)(void);
static void Save_NewPwd_To_Eeeprom_Handler(void);
static void Over_Input_MaxDigital_Fun(void);



/*******************************************************
 * 
 * Function Name: void Start_PowerOn_Handler(void)
 * Function: start power on handler
 * INPUT Ref:NO
 * Return Ref:NO
 * 
********************************************************/
void Start_PowerOn_Handler(void)
{
    
 if(HAL_GPIO_ReadPin(KEY_INPUT_GPIO_Port,KEY_INPUT_Pin) ==0 && run_t.powerOn ==0){
 	run_t.powerOn++;
		run_t.factory_test = 1;

		run_t.gTimer_input_error_times_60s =0;

		run_t.buzzer_sound_tag = key_sound;
		POWER_ON();
		run_t.confirm_button_flag =confirm_button_donot_pressed;
		run_t.backlight_label  = BACKLIGHT_FACTORY_LED;
	  
	  }
	 else{
	 	if(run_t.powerOn ==0){

			run_t.powerOn++;
			run_t.confirm_button_flag =confirm_button_donot_pressed;
			run_t.gTimer_8s=0;
		
			POWER_ON();
			BACKLIGHT_ON();
			OK_LED_OFF();
			ERR_LED_OFF();
			BUZZER_KeySound();//WT.EDIT 2022.09.12
		           
		} 
	 }
}

/**************************************************************************
	*
	*Funtcion Name:static void NewPwd_SaveData_Handler(void)
	*Function : to special action process 
	*Input Ref:NO
	*Return Ref:NO
	*
**************************************************************************/
static void NewPwd_SaveData_Handler(void)
{
   switch(run_t.password_unlock_model){

    case STORE_MODEL_EEPROM:
        run_t.inputDeepSleep_times=0;
		run_t.passwordsMatch=0  ;
	    run_t.gTimer_8s=0;
	     if(run_t.touchkey_save_data_flag==1){ //
		     Save_To_EeepromNewPwd();
	     }
         else{
		      Fingerprint_NewClinet_Login_Fun();
		      run_t.motor_return_homePosition=0;
         	}
		 
	    if(syspara_t.ps_login_new_fp_success==1){
			syspara_t.ps_login_new_fp_success=0;
			 run_t.Confirm_newPassword=0;
		     run_t.inputNewPassword_Enable =0;
             run_t.password_unlock_model = EXIT_STORE_MODEL;
             run_t.works_led_lable=works_ok_led_on;
        }
		if( run_t.add_new_password_success == LOGIN_SUCCESS){
			run_t.password_unlock_model = EXIT_STORE_MODEL;
             run_t.works_led_lable=works_ok_led_on; 
		    run_t.touchkey_save_data_flag=0;
		}
		else if(run_t.add_new_password_success == LOGIN_FAIL){
		      run_t.password_unlock_model = EXIT_STORE_MODEL;
			  run_t.works_led_lable=works_led_error_blink; 
			  run_t.touchkey_save_data_flag=0;
         }
		
     break;

	case EXIT_STORE_MODEL :
           run_t.password_unlock_model = 0xff;
	break;

	default :

	break;

    }


}
/*******************************************************
 * 
 * Function Name:void Save_To_EeepromNewPwd(void)
 * Function: start power on handler
 * INPUT Ref:NO
 * Return Ref:NO
 * 
********************************************************/
static void Save_To_EeepromNewPwd(void)
{
	if(run_t.inputNewPassword_Enable ==1 && run_t.inputNewPasswordTimes ==2 ){//WT.EDIT .2022.09.28.if(run_t.adminiId==1){
		
		   run_t.gTimer_8s=0;

		   SavePassword_To_EEPROM();
			
	 }
	run_t.motor_return_homePosition=0;
	run_t.inputDeepSleep_times =0;
	run_t.gTimer_8s=0;
	
	
}


/*******************************************************
 * 
 * Function Name: void Start_PowerOn_Handler(void)
 * Function: start power on handler
 * INPUT Ref:NO
 * Return Ref:NO
 * 
********************************************************/
void CheckPassword_UnLock_Handler(void)
{


	switch(run_t.confirm_button_flag){

	    case confirm_button_donot_pressed:
	     if(run_t.factory_test == 0)
		    TouchKey_Handler();//TouchKey_Handler();
	   

       break;

	   case confirm_button_pressed://1
	        Enter_Key_Fun();//RunCommand_Unlock();
	    
	   break;

	   case confirm_button_unlock://2
			RunCommand_Unlock();


	   break;

	   case confirm_button_save_new_password://3
	   	
	      Save_NewPwd_To_Eeeprom_Handler();//UnLock_And_SaveData_Handler();

	   break;
       
       case confirm_button_clearn_eeprom://4
	   	  ClearEEPROM_Data_Fun();
       
       break;
       
      case confirm_button_over_numbers: //6
			 Over_Input_MaxDigital_Fun();
	   break;
       
       case confirm_button_error_times: //6
           run_t.gTimer_input_error_times_60s =0;
           run_t.panel_lock=1;
           run_t.gTimer_8s=0;
           run_t.backlight_label= BACKLIGHT_ERROR_OVER_INPUT_TIMES; 
           run_t.confirm_button_flag=confirm_button_lock_panel;
       
       break;

	   case confirm_button_lock_panel: //5
          Panel_Lock_Handler();
       break;

	  
	}
	
}





/***********************************************************************

************************************************************************/

void Fingerprint_ReadData_Handler(void(*fp_read_handler)(void))
{

      FP_ReadData_Handler=fp_read_handler;


}

/*******************************************************
 * 
 * Function Name:void Save_NewPwd_To_Eeeprom_Handler(void)
 * Function: start power on handler
 * INPUT Ref:NO
 * Return Ref:NO
 * 
********************************************************/
static void Save_NewPwd_To_Eeeprom_Handler(void)
{

	if(run_t.inputNewPassword_Enable ==1 && run_t.inputNewPasswordTimes ==2 ){//WT.EDIT .2022.09.28.if(run_t.adminiId==1){
		
		   SavePassword_To_EEPROM();
			
	 }
	run_t.gTimer_8s =0;
	run_t.inputDeepSleep_times =0;
	
	
}
/**************************************************************************
	*
	*Funtcion Name:static void Over_Input_MaxDigital_Fun(void)
	*Function : to special action process 
	*Input Ref: NO
	*Return Ref:NO
	*
**************************************************************************/
static void Over_Input_MaxDigital_Fun(void)
{
	run_t.gTimer_8s=0;
	run_t.input_digital_key_number_counter =0;
	run_t.Confirm_newPassword =0; //to save new password of flag 
	run_t.inputNewPasswordTimes =0;
    run_t.inputNewPassword_Enable =0; 
	run_t.buzzer_sound_tag = fail_sound;

	run_t.clear_inputNumbers_newpassword=0;

    run_t.backlight_label = BACKLIGHT_ERROR_BLINK;
	run_t.confirm_button_flag=confirm_button_donot_pressed  ; 




}



