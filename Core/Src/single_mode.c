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
 uint16_t KeyValue;
uint8_t fp_cnt;

static void Save_To_EeepromNewPwd(void);
static void UnLock_Aand_SaveData_Handler(void);

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
			
			run_t.gTimer_10s_start=0;
			run_t.gTimer_input_error_times_60s =0;
		
			run_t.buzzer_sound_lable = sound_key;//run_t.buzzer_key_sound_flag =1;
		   run_t.factory_test = 1;
			run_t.works_led_lable=factory_led_test;
              
			  POWER_ON();
			  FP_POWER_ON(); //FP_power
              
      
  }
 else{
 if(run_t.powerOn ==0){

				run_t.powerOn++;
				run_t.passwordsMatch =0;
				run_t.password_unlock_model =POWER_ON_MODEL; // 4: power on is motor 1/4 angle
				run_t.motor_return_homePosition=0; //
				run_t.gTimer_8s=0;
			 
				POWER_ON();
				FP_POWER_ON(); //FP_power WT.EDIT 2022.11.28
				TouchKey_Led_Handler();
				Buzzer_KeySound();//WT.EDIT 2022.09.12
            syspara_t.PS_wakeup_flag=0;
	           
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
    

	if(run_t.panel_lock==0 && run_t.factory_test==0){
	//keyboard password lock
    if(run_t.passwordsMatch==0){ //&& run_t.readI2C_data ==1 && run_t.factory_test ==0){
	  	TouchKey_Handler();

     }//fingerprint lock input 
	 
	 
	 if((FP_INPUT_KEY()==1  || syspara_t.ps_pre_detector==1 ) && run_t.inputNewPassword_Enable ==0 && run_t.passwordsMatch ==0 ){
	 	fp_cnt++;

	  
		FP_ReadData_Handler();

		run_t.detection_input_flag=1;
     
	 }//keyboard password input 
     else if(run_t.passwordsMatch ==1 && run_t.inputNewPassword_Enable==0){
		  
		  run_t.passwordsMatch=0;
		  syspara_t.PS_wakeup_flag=0;

          RunCommand_Unlock_Keyboard();
		  
         run_t.detection_input_flag=1;
		 
    }

    Lock_Open_Order();
    UnLock_Aand_SaveData_Handler();
  }
}

/**************************************************************************
	*
	*Funtcion Name:static void UnLock_Aand_SaveData_Handler(void)
	*Function : to special action process 
	*Input Ref:NO
	*Return Ref:NO
	*
**************************************************************************/
static void UnLock_Aand_SaveData_Handler(void)
{
   switch(run_t.password_unlock_model){

    case STORE_MODEL_EEPROM:
        run_t.inputDeepSleep_times=0;
		run_t.passwordsMatch=0  ;
	   run_t.gTimer_8s=0;
	     if(syspara_t.PS_wakeup_flag==0)
		     Save_To_EeepromNewPwd();
	     else
		     Fingerprint_NewClinet_Login_Fun();
		run_t.motor_return_homePosition=0;
	
        
    break;

	case POWER_ON_MODEL: //Power On motor run 1/4 angle
	  
         Motor_CW_Run();// Close 
		 HAL_Delay(530);//WT.EDIT 2022.09.19
		 Motor_Stop();
		 run_t.motor_return_homePosition=0;//WT.EDIT 2022.08.18
		 run_t.password_unlock_model=0;
	     run_t.gTimer_8s=0;
		 Panel_LED_Off();
	

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
	if(run_t.inputNewPassword_Enable ==1 && run_t.inputNewPasswordTimes !=1 ){//WT.EDIT .2022.09.28.if(run_t.adminiId==1){
		
		   run_t.gTimer_8s=0;

		   SavePassword_To_EEPROM();
			
	 }
	run_t.motor_return_homePosition=0;
	run_t.inputDeepSleep_times =0;
	run_t.gTimer_8s=0;
	//run_t.BackLight =1;//WT.EDIT 2022.11.01
	
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
         //if(I2C_Simple_Read_From_Device(SC12B_ADDR,SC_Data,2) ==DONE){
			

		 KeyValue =(uint16_t)(SC_Data[0]<<8) + SC_Data[1];
				RunCheck_Mode(KeyValue); 
	            if(KeyValue ==0){

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



