#include "buzzer.h"
#include "gpio.h"
#include "run.h"
#include "delay.h"
#include "tim.h"
#include "as608.h"
#define  STM32L010F4P6_IC

#define CPU_FREQUENCY_MHZ 24 // STM32时钟主频

static void Buzzer_High_Sound(void);

static void Buzzer_ErrorSound(void);
//static void Buzzer_Fail_Sound(void);
static void Buzzer_High_Sound_2(void);
/*****************************************************************
  *
  *Function Name: void BUZZER_KeySound(void)
  *Function: Key be pressed carry out sound
  *Input Ref:NO
  *Return Ref:NO
  *
*****************************************************************/
void Buzzer_KeySound(void)
{
  //unsigned int m=300;//80

  #ifdef STM32L010F4P6_IC
  unsigned int m=200;//80
	while(m--){

   HAL_GPIO_TogglePin(BEEP_GPIO_Port,BEEP_GPIO_Pin);//BEEP=!BEEP;
   delay_us(460);//4000//350//__delay_us(300);;//__delay_us(800);//delayUS(300);
 

  }
 #else

    HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_3); //
    HAL_Delay(100);
	HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_3);



 #endif
}
/*****************************************************************
  *
  *Function Name: void BUZZER_KeySound(void)
  *Function: Key be pressed carry out sound
  *Input Ref:NO
  *Return Ref:NO
  *
*****************************************************************/
void Buzzer_LongSound(void)
{
    #ifdef STM32L010F4P6_IC

	unsigned int m=600;//400
	while(m--){

	   HAL_GPIO_TogglePin(BEEP_GPIO_Port,BEEP_GPIO_Pin );//BEEP=!BEEP;
	   delay_us(460);//(600)__delay_us(300);//delayUS(600);
     //delay_us(300);
     
    }
	#else 
	  HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_3); //
	  HAL_Delay(200);
	   HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_3);


	#endif 
}
/*****************************************************************
  *
  *Function Name: void Buzzer_ShortSound(void)
  *Function: short sound
  *Input Ref:NO
  *Return Ref:NO
  *
*****************************************************************/
void Buzzer_ShortSound(void)
{

   #ifdef STM32L010F4P6_IC
   
	unsigned int m=200;
	

	 while(m--){

	   HAL_GPIO_TogglePin(BEEP_GPIO_Port,BEEP_GPIO_Pin );//BEEP=!BEEP;
	  delay_us(460);//__delay_us(300);//delayUS(600);



	 }
   #else
	 HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_3); //
	  HAL_Delay(50);
	   HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_3);
   #endif 

	 

}
/*****************************************************************
  *
  *Function Name: void Buzzer_ErrorSound(void)
  *Function: error sound
  *Input Ref:NO
  *Return Ref:NO
  *
*****************************************************************/
static void Buzzer_ErrorSound(void)
{

   #ifdef STM32L010F4P6_IC


	unsigned int m=200;//80
  
	while(m--){

  HAL_GPIO_TogglePin(BEEP_GPIO_Port,BEEP_GPIO_Pin );//BEEP=!BEEP;
  delay_us(460);//4000//350//__delay_us(300);;//__delay_us(800);//delayUS(300);
 

  }
 #else 

      HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_3); //
	  HAL_Delay(100);
	  HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_3);


 #endif 


}

/*****************************************************************
  *
  *Function Name: void Buzzer_ErrorSound(void)
  *Function: high frequency 
  *Input Ref:NO
  *Return Ref:NO
  *
*****************************************************************/
static void Buzzer_High_Sound(void)
{
     

	 unsigned int m=250;//300
	
  
      while(m--){
          HAL_GPIO_TogglePin(BEEP_GPIO_Port,BEEP_GPIO_Pin );//BEEP=!BEEP;
          delay_us(480);//480
         
       }

}
/****************************************************************************
*
*Function Name:void  Buzzer_InputNewPassword_two_short(void)
*Function : run is main 
*Input Ref: NO
*Retrun Ref:NO
*
****************************************************************************/
static void Buzzer_High_Sound_2(void)
{

    #ifdef STM32L010F4P6_IC
	 unsigned int m=100;

     while(m--){
         HAL_GPIO_TogglePin(BEEP_GPIO_Port,BEEP_GPIO_Pin );//BEEP=!BEEP;
         delay_us(480);//__delay_us(500);//delayUS(99);
      }
	 #else 
	    
	    HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_3); //
		HAL_Delay(100);
		HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_3);


	 #endif 
}

/****************************************************************************
*
*Function Name:void  Buzzer_InputNewPassword_two_short(void)
*Function : run is main 
*Input Ref: NO
*Retrun Ref:NO
*
****************************************************************************/
void Buzzer_Fail_Sound(void)
{

	Buzzer_ErrorSound();//Buzzer_ShortSound();//Buzzer_ReSound();//fail sound has two sound //WT.EDIT 2022.09.13
    BUZZER_OFF();
	HAL_Delay(50);
	Buzzer_ErrorSound();//Buzzer_ShortSound();//Buzzer_ReSound();//fail sound has two sound 
	BUZZER_OFF();
    HAL_Delay(50);
	Buzzer_ErrorSound();
	BUZZER_OFF();
	//HAL_Delay(50);

}
/****************************************************************************
  *
  *Function Name:void  Buzzer_InputNewPassword_two_short(void)
  *Function : run is main 
  *Input Ref: NO
  *Retrun Ref:NO
  *
****************************************************************************/
void Buzzer_Sound_Handler(void)
{
    
   switch(run_t.buzzer_sound_label){

      case sound_key:
          Buzzer_KeySound();
        
          run_t.buzzer_sound_label=sound_over;
      break;

      case sound_fail: //2
          Buzzer_Fail_Sound();
		
          run_t.buzzer_sound_label=sound_over;
      break;

      case sound_new_pwd_the_first:
           Buzzer_High_Sound();
	
            run_t.buzzer_sound_label=sound_over;
      break;

      case sound_new_pwd_the_second:
        Buzzer_KeySound();//Buzzer_ShortSound(); //WT.EDIT 2022.09.13
        HAL_Delay(50);
        Buzzer_KeySound();
	
        run_t.buzzer_sound_label=sound_over;
      break;

      case sound_high:
         Buzzer_High_Sound();
          HAL_Delay(50);
        Buzzer_High_Sound_2();
	
         run_t.buzzer_sound_label=sound_over;

      break;

      case sound_excute:
        Buzzer_LongSound();
		
        run_t.buzzer_sound_label=sound_over;

      break;

      case sound_over: //7

        BUZZER_OFF();
        run_t.buzzer_sound_label=0xff;
        
      break;

      default:
          
      break;


    }


}

