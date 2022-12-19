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
//	MotorStart_CW_Step();
//	MOTOR_CW_RUN();	
//	MOTOR_CCW_OFF();
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
//	MotorStart_CCW_Step();
//    MOTOR_CCW_RUN();
//    MOTOR_CW_OFF();

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
//	
//       MOTOR_CCW_OFF();
//        
//       MOTOR_CW_RUN();	
//	   HAL_Delay(25);//__delay_ms(25);
//	   MOTOR_CW_OFF();
//	   HAL_Delay(25);//__delay_ms(25);
       MOTOR_CW_RUN();	
       HAL_Delay(10);//__delay_ms(50);
	  
   
}

 static void MotorStart_CCW_Step(void)
{

   
//	   MOTOR_CW_OFF();	
//	
//	   MOTOR_CCW_RUN();
//	   HAL_Delay(25);//__delay_ms(50);
//	    MOTOR_CCW_OFF();
//	   HAL_Delay(25);//__delay_ms(50);
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

  if(run_t.motor_doing_flag==1){//open lock doing

        run_t.gTimer_8s =0;//WT.EDIT.2022.10.06

      switch(run_t.Motor_RunCmd_Label){

        case motor_run_start:
            PS_Green_Led_ON();
            run_t.motor_return_homePosition=1;//motor runing flag 
            syspara_t.handler_read_data_flag++;
           
            run_t.Numbers_counter =0 ;
            run_t.eepromAddress=0;
            run_t.passwordsMatch = 0;
            run_t.password_unlock_model =0;
            run_t.error_times=0;

            run_t.gTimer_8s =0;//WT.EDIT.2022.10.06
            ERR_LED_OFF();
            OK_LED_ON();
			
           // Buzzer_LongSound(); //WT.EDIT 2022.10.06
            Motor_CCW_Run();//open passwordlock 
                     
            run_t.Motor_RunCmd_Label=motor_run_underway;
        break;

        case motor_run_underway:
            //MOTOR open run ->stop Position 
            if(run_t.motorRunCount < 25){//100ms *300=3s
                run_t.gTimer_8s =0;//WT.EDIT 2022.10.06
                run_t.motor_return_homePosition=1;//WT.EDIT 2022.08.18
               
            }
            else{
                run_t.motorRunCount=0;
                run_t.Motor_RunCmd_Label=motor_run_half_stop;
            }
        break;

        case motor_run_half_stop:
            if(run_t.motorRunCount < 11){ //100ms * 10
              Motor_Stop();
               run_t.motor_return_homePosition=1;//motor runing flag 
            
                }
             else{ 
                 run_t.motorRunCount=0;
                run_t.Motor_RunCmd_Label=morot_reverse;
             }
             
        break;

        case morot_reverse :
            run_t.motor_return_homePosition=1;//motor runing flag 
            Motor_CW_Run();// Close
            run_t.Motor_RunCmd_Label=motor_stop;
            run_t.motorRunCount=0;

        break;

        case motor_stop:
            if(run_t.motorRunCount >20){//15 //100ms x 10 =1s motor stop 1s 
                Motor_Stop();
                run_t.motor_return_homePosition=0;//motor runing flag 
                run_t.motor_doing_flag=0;
                run_t.Motor_RunCmd_Label= 0xff;
                ERR_LED_OFF();
                OK_LED_OFF();
				PS_Green_Led_OFF();
                run_t.motorRunCount=0;
                syspara_t.PS_read_template=0;
                //ps_color=PS_ControlBLN(0x01,0x01,0x01,0);
               ps_color= PS_Breath_LED()  ;
            }
           
        break;

        default:

        break;    

        }
       }
}
                
        