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
#include "lock.h"
 uint16_t KeyValue;
uint8_t fp_cnt;




void (*TouchKey_Handler)(void);
void (*FP_ReadData_Handler)(void);


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
    
 if(HAL_GPIO_ReadPin(KEY_INPUT_GPIO_Port,KEY_INPUT_Pin) ==0   && run_t.powerOn ==0){
	run_t.powerOn++;
	run_t.buzzer_sound_label = sound_key;//run_t.buzzer_key_sound_flag =1;
    run_t.factory_test = 1;
	run_t.works_led_label=factory_led_test;
      
	  POWER_ON();
	  FP_POWER_ON(); //FP_power
      
   }
   else{
	 if(run_t.powerOn ==0){

				run_t.powerOn++;
			    run_t.motor_return_homePosition=0; //
				run_t.gTimer_8s=0;
			 
				POWER_ON();
				FP_POWER_ON(); //FP_power WT.EDIT 2022.11.28
				TouchKey_Led_Handler();
				Buzzer_KeySound();//WT.EDIT 2022.09.12
     } 
 }

}
/*******************************************************
 * 
 * Function Name: void Start_PowerOn_Handler(void)
 * Function: start power on handler
 * INPUT Ref:NO
 * Return Ref:NO
 * 
********************************************************/
void CheckPassword_Lock_Handler(void)
{
    uint8_t temValue;
    switch(run_t.pwd_fp_label){

       case PWD_ID: //2
	   
			 TouchKey();//TouchKey_Handler();
            run_t.gTimer_8s=0;//clear zero
            switch(run_t.enter_key){

			   case KEY_FAIL://01
					run_t.pwd_fp_label = DISPOSE_KEY_FAIL;
			   break;

			   case KEY_SUCCESS: //2
					run_t.pwd_fp_label = DISPOSE_STEP_COMPARE;
			   break;


			   case KEY_LOCK_60S://03
			   	    run_t.pwd_fp_label = DISPOSE_KEY_LOCK_60S;
			   break;

			   case KEY_NULL: //6
			       run_t.pwd_fp_label =DISPOSE_NULL;
                   
			   break;

			   case KEY_INPUT_NEW_PWD: //4
			   	  run_t.works_led_label = works_ok_blink;
			      run_t.buzzer_sound_label = sound_new_pwd_the_second;
			   	  run_t.pwd_fp_label =DISPOSE_NULL;

			   break;

			   case KEY_INPUT_NEW_PWD_CONFIRM:
                  run_t.pwd_fp_label=DISPOSE_SAVE_DATA;
			   break;


			}
				
           
	   break;

	   case FP_ID: //3
	   	 
		   FP_ReadData_Handler();
	     
	       run_t.pwd_fp_label = DISPOSE_STEP_COMPARE;
	   break;

	   case DISPOSE_STEP_COMPARE://4
	   	 
		  RunCommand_Unlock_Keyboard();
		  switch(run_t.open_lock_lable){

		     case open_lock_success :
			 	 Open_Lock_Success_Fun();
		       if(run_t.password_unlock_model==DISPOSE_STORE_MODEL_EEPROM){
			   	    
					run_t.gTimer_8s =0;
					   if(run_t.motor_runProcess_flag==0)
					         run_t.pwd_fp_label = DISPOSE_STORE_SET_REF;
					   else
					   	   run_t.pwd_fp_label = DISPOSE_COPY_ONESELF;
		       	}
			   else if(run_t.password_unlock_model ==DISPOSE_MOTOR_RUN ){

				   if(run_t.motor_runProcess_flag==0)
				       run_t.pwd_fp_label = DISPOSE_MOTOR_RUN;
				   else
				   	   run_t.pwd_fp_label = DISPOSE_COPY_ONESELF;
				  
			    }
                else if(run_t.password_unlock_model==DISPOSE_NULL)
                    run_t.pwd_fp_label = DISPOSE_NULL;
			  
			  break;
		  	  

			 case open_lock_fail: //2
			 	 Open_Lock_Fail_Fun();
				 if(run_t.password_unlock_model==KEY_LOCK_60S){
				 	   
				 	   run_t.pwd_fp_label = DISPOSE_KEY_LOCK_60S;
				} 
				 else if(run_t.password_unlock_model ==open_lock_null){

                      run_t.pwd_fp_label = DISPOSE_NULL;
				 }
			

			
			 break;
			}
		 
	   break;

	   case DISPOSE_STORE_SET_REF: //6
	   	    
			run_t.works_led_label = works_ok_blink;
			run_t.buzzer_sound_label = sound_new_pwd_the_first;
	   	    run_t.inputNewPassword_Enable = 1;
	        run_t.pwd_fp_label = DISPOSE_NULL;


	   break;

	   case DISPOSE_SAVE_DATA: //7
	       Save_To_EeepromNewPwd();
		  run_t.pwd_fp_label = DISPOSE_NULL;
	   break;

	   case DISPOSE_MOTOR_RUN: //9
	   
         run_t.pwd_fp_label =DISPOSE_NULL;
		 run_t.buzzer_sound_label = sound_excute;
	   	 run_t.Motor_RunCmd_Label = motor_run_start;
		 run_t.motor_runProcess_flag = 1;
	   
	   break;

       case DISPOSE_KEY_FAIL: //10
	   	    run_t.works_led_label = works_error_blink;
			run_t.buzzer_sound_label = sound_fail;

        	run_t.pwd_fp_label = DISPOSE_NULL;
	   break;

	   case DISPOSE_KEY_LOCK_60S://11
             run_t.pwd_fp_label = DISPOSE_NULL;
	   break;

	   case DISPOSE_COPY_ONESELF:
	   	   run_t.buzzer_sound_label = sound_excute;
	   	   run_t.works_led_label = works_ok_led_on;
	       run_t.pwd_fp_label = DISPOSE_NULL;

	   break;

	   case DISPOSE_NULL: //14
         
	   run_t.pwd_fp_label= 0xff;

	   break;

	   default:

	   break;

	}
   
}
/*******************************************************
 * 
 * Function Name:void TouchKey(void)
 * Function: start power on handler
 * INPUT Ref:NO
 * Return Ref:NO
 * 
********************************************************/
void TouchKey(void)
{
   if(I2C_Read_From_Device(SC12B_ADDR,0x08,SC_Data,2)==DONE){
   	    
    
    KeyValue =(uint16_t)(SC_Data[0]<<8) + SC_Data[1];
	HAL_Delay(5);
	RunCheck_Mode(KeyValue); 
    if(KeyValue ==0 ){

	    run_t.NumbersKey_pressedNumbers = 0;
	    run_t.getSpecial_1_key++;
	    run_t.getSpecial_2_key++;
	    run_t.getNumbers_key=0x40;
		 

    }
  }     
			  
	 
}

void TouchKey_Run_Handler(void (*touchkey_huandler)(void))
{

	TouchKey_Handler=touchkey_huandler;

}
void Fingerprint_ReadData_Handler(void(*fp_read_handler)(void))
{

      FP_ReadData_Handler=fp_read_handler;


}



