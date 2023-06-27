#include "motor.h"
#include "single_mode.h"
#include "run.h"
#include "led.h"
#include "at24c02.h"
#include "kmp.h"
#include "buzzer.h"
#include "touchkey.h"
#include "motor.h"
#include "tim.h"
#include "as608.h"
#include "fingerprint.h"

static void MotorStart_CW_Step(void);//Up 
static void MotorStart_CCW_Step(void);
uint8_t ps_color=0xff;

/**********************************************************
 *
 * Function Name: void Motor_CCW_Run(void) 
 * Function : Open lock 
 * Input Ref: NO
 * Return Ref:NO
 *
 *********************************************************/
void Motor_CCW_Run(void) 
{

    MotorStart_CCW_Step();
    MOTOR_CCW_RUN();
    MOTOR_CW_OFF();	
	
}
/**********************************************************
 *
 * Function Name: void Motor_CW_Run(void)
 * Function : shut off  lock 
 * Input Ref: NO
 * Return Ref:NO
 *
 *********************************************************/
void Motor_CW_Run(void)
{
	MotorStart_CW_Step();
	 MOTOR_CW_RUN();	
	 MOTOR_CCW_OFF();
	
}



void Motor_Stop(void)
{
    MOTOR_CW_OFF(); //brake
    MOTOR_CCW_OFF(); //brake
}
/**
 * @brief 
 * 
 */
 static void MotorStart_CW_Step(void)//Up 
{

       MOTOR_CW_RUN();	
       HAL_Delay(10);//__delay_ms(50);
	  
   
}

 static void MotorStart_CCW_Step(void)
{

   

       MOTOR_CCW_RUN();
	   HAL_Delay(10);
	
        
   }

/****************************************************************************
*
*Function Name:void RunMotor_Definite_Handler(void) 
*Function : run is main 
*Input Ref: NO
*Retrun Ref:NO
*
****************************************************************************/
void RunMotor_Definite_Handler(void) //definite motor
{
	  static uint8_t led=0;

		switch(run_t.motor_doing_flag){

		case motor_foreward:
			run_t.gTimer_8s =0;//WT.EDIT.2022.10.06
			run_t.motorRunCount++;

			if(run_t.motor_return_homePosition==0 ){
				run_t.motor_return_homePosition= 1; 
				Buzzer_LongSound(); //WT.EDIT 2022.10.06
				Motor_CCW_Run();//open lock 
				
		
			
		
				ERR_LED_OFF();
				OK_LED_ON();


				run_t.eepromAddress=0;


				run_t.error_times=0;

				run_t.gTimer_8s =0;//WT.EDIT.2022.10.06

				run_t.returnHomePosition_Count=0;
				run_t.password_unlock=0;
			}

			if(run_t.motorRunCount>180 && run_t.motorRunCount <210){
				Motor_Stop();
				if(led==0){
				led++;
				
			
				run_t.backlight_label =BACKLIGHT_ON;
				OK_LED_OFF();

			}
			run_t.inputDeepSleep_times =0;
			run_t.gTimer_8s =0;//WT.EDIT.2022.10.06
			}

			if(run_t.motorRunCount >199){
                run_t.motorRunCount=0;
				run_t.returnHomePosition_Count=0;
				run_t.motor_doing_flag=motor_revesal;
				
				led=0;
			}
          
		  break;

		  case motor_revesal:
             run_t.returnHomePosition_Count++;
			 run_t.motorRunCount++;
			run_t.gTimer_8s =0;
		     run_t.inputDeepSleep_times =0;
			if(run_t.motorRunCount >50){
			    Motor_CW_Run();// Close
			    
			}
           
			
			if(run_t.returnHomePosition_Count > 231){
			    Motor_Stop();
				run_t.motorRunCount=0;
				run_t.motor_return_homePosition=0;//WT.EDIT 2022.08.18
			
				 run_t.motor_doing_flag=motor_null;
				run_t.oneself_copy_behavior=0;
                
      
			
				//led control gpio
				run_t.gTimer_8s =10;//WT.EDIT 2022.10.06
				run_t.backlight_label =BACKLIGHT_AT_ONCE_OFF;
			}

		   
		  break;

		  case motor_null:
		     run_t.motorRunCount=0;
			run_t.returnHomePosition_Count=0;

		  break;
		  
		  default:
          break;
	   }
 
}
                
        