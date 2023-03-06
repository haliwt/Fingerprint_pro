#ifndef __MOTOR_H_
#define __MOTOR_H_
#include "main.h"


typedef enum _motor_run_state{
	motor_run_start=0x01,
	motor_run_underway,
	motor_run_half_stop,
    motor_interval_stop_times,
    morot_reverse,
    motor_stop
}motor_state;


#define MOTOR_CW_GPIO_PIN    		GPIO_PIN_6
#define MOTOR_CW_GPIO				GPIOA

#define MOTOR_CW_RUN()         HAL_GPIO_WritePin(MOTOR_CW_GPIO,MOTOR_CW_GPIO_PIN,GPIO_PIN_SET)//  (PORTAbits.RA4 =1)
#define MOTOR_CW_OFF()		   HAL_GPIO_WritePin(MOTOR_CW_GPIO,MOTOR_CW_GPIO_PIN,GPIO_PIN_RESET)// (PORTAbits.RA4 = 0)



#define MOTOR_CCW_GPIO_PIN        GPIO_PIN_5
#define MOTOR_CCW_GPIO            GPIOA

#define MOTOR_CCW_RUN()          HAL_GPIO_WritePin(MOTOR_CCW_GPIO,MOTOR_CCW_GPIO_PIN,GPIO_PIN_SET)//(PORTAbits.RA5 = 1)
#define MOTOR_CCW_OFF()          HAL_GPIO_WritePin(MOTOR_CCW_GPIO,MOTOR_CCW_GPIO_PIN,GPIO_PIN_RESET)//(PORTAbits.RA5 = 0)





void Motor_CCW_Run(void);
void Motor_CW_Run(void);
void Motor_Stop(void);

void RunMotor_Definite_Handler(void); 


void Motor_Reverse_State(void);

void Motor_StartRun(void);




#endif 

