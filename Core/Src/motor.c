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

  static uint8_t adjust_ref,stop_flag;



      switch(run_t.Motor_RunCmd_Label){

        case motor_run_start:
            PS_Green_Led_ON();
            run_t.motor_return_homePosition=1;//motor runing flag 
            syspara_t.handler_read_data_flag++;
           
            run_t.Numbers_counter =0 ;
            run_t.eepromAddress=0;
         
            run_t.password_unlock_model =0;
            run_t.error_times=0;

            run_t.gTimer_8s =0;//WT.EDIT.2022.10.06
            ERR_LED_OFF();
            OK_LED_ON();
			
            Motor_CCW_Run();//open passwordlock 
                     
            run_t.Motor_RunCmd_Label=motor_run_underway;
        break;

        case motor_run_underway: //2
            //MOTOR open run ->stop Position 
            if(run_t.motorRunCount < 3){//100ms *300=3s
                run_t.gTimer_8s =0;//WT.EDIT 2022.10.06
                run_t.motor_return_homePosition=1;//WT.EDIT 2022.08.18
               
            }
            else{
                run_t.motorRunCount=0;
                run_t.Motor_RunCmd_Label=motor_run_half_stop;
            }
        break;

        case motor_run_half_stop: //3
            if(run_t.motorRunCount < 2){ //100ms * 10
              Motor_Stop();
              run_t.motor_return_homePosition=1;//motor runing flag 
            
             }
             else{ 
                 run_t.motorRunCount=0;
                run_t.Motor_RunCmd_Label=morot_reverse;
             }
             
        break;

        case morot_reverse : //5 //home position
			 run_t.gTimer_8s =0;//WT.EDIT.2022.10.06
			 PS_Blue_Led_ON();
			// OK_LED_OFF();
		    if( run_t.motorRunCount < 3){
		            run_t.motor_return_homePosition=1;//motor runing flag 
		            run_t.gTimer_motor_reverse=0;
		            Motor_CW_Run();// Close
		           
		     }
			 else{

				 run_t.Motor_RunCmd_Label=motor_stop;
		         run_t.motorRunCount=0;

			 }

        break;

        case motor_stop: //6
            if(run_t.motorRunCount >5 && adjust_ref <6 ){//15 //100ms x 10 =1s motor stop 1s 
               
                adjust_ref++;
				Motor_Stop();
			    stop_flag =1;
            }
			else if(run_t.motorRunCount >4) {
        		adjust_ref=0;
				Motor_Stop();
			    stop_flag=1;
                
			}
			
  

			 if(stop_flag==1){
			 	stop_flag=0;
                run_t.motor_return_homePosition=0;//motor runing flag 
                run_t.motor_doing_flag=0;
                run_t.Motor_RunCmd_Label= 0xff;
				BACKLIGHT_OFF();
                ERR_LED_OFF();
				PS_Green_Led_OFF();
                run_t.motorRunCount=0;
                syspara_t.PS_read_template=0;
                run_t.backlight_Cmd_lable = backlight_led_off;
      
            }
           
        break;

        default:

        break;    

        }
   


//         Motor_CW_Run();// Close 
//		 HAL_Delay(530);//WT.EDIT 2022.09.19
//		 Motor_Stop();
//		 run_t.motor_return_homePosition=0;//WT.EDIT 2022.08.18
//		 run_t.password_unlock_model=0;
//	     run_t.gTimer_8s=0;
//		 Panel_LED_Off();
}
                

void Motor_Reverse_State(void)
{
     if(run_t.motor_return_homePosition==1){

		 Motor_CW_Run();// Close 
		 //HAL_Delay(530);//WT.EDIT 2022.09.19
		
		 if(run_t.gTimer_motor_reverse > 53){
		 	run_t.gTimer_motor_reverse =0;
		      Motor_Stop();
			 run_t.motor_return_homePosition=0;//WT.EDIT 2022.08.18
			 run_t.password_unlock_model=0;
		     run_t.gTimer_8s=0;
			 Panel_LED_Off();
		 	}
        }

}
