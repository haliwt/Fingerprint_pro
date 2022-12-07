#include "motor.h"

static void MotorStart_CW_Step(void);//Up 
static void MotorStart_CCW_Step(void);


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
	
       MOTOR_CCW_OFF();
        
       MOTOR_CW_RUN();	
	   HAL_Delay(25);//__delay_ms(25);
	   MOTOR_CW_OFF();
	   HAL_Delay(25);//__delay_ms(25);
       MOTOR_CW_RUN();	
       HAL_Delay(25);//__delay_ms(50);
	  
   
}

 static void MotorStart_CCW_Step(void)
{
   
        
      	MOTOR_CW_OFF();	
	
	   MOTOR_CCW_RUN();
	   HAL_Delay(25);//__delay_ms(50);
	    MOTOR_CCW_OFF();
	   HAL_Delay(25);//__delay_ms(50);
       MOTOR_CCW_RUN();
	   HAL_Delay(25);
        
   }

