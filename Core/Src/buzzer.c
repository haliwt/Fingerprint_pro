#include "buzzer.h"
#include "gpio.h"
#include "run.h"
#include "delay.h"
#include "tim.h"
#include "as608.h"
#define  STM32L010F4P6_IC

#define CPU_FREQUENCY_MHZ    64 // STM32时钟主频


static void Buzzer_Two_KeySound(void);


/*****************************************************************
  *
  *Function Name: void Buzzer_KeySound(void)
  *Function: Key be pressed carry out sound
  *Input Ref:NO
  *Return Ref:NO
  *
*****************************************************************/
void Buzzer_KeySound(void)
{

    // HAL_TIM_Base_Start(&htim3);
     HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_3);
     HAL_Delay(40);
  
}

/*****************************************************************
  *
  *Function Name: void Buzzer_KeySound(void)
  *Function: Key be pressed carry out sound
  *Input Ref:NO
  *Return Ref:NO
  *
*****************************************************************/
static void Buzzer_Two_KeySound(void)
{
    HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_3);
    HAL_Delay(60);


}

/*****************************************************************
  *
  *Function Name: void Buzzer_KeySound(void)
  *Function: Key be pressed carry out sound
  *Input Ref:NO
  *Return Ref:NO
  *
*****************************************************************/
void Buzzer_LongSound(void)
{

    HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_3);
    HAL_Delay(500);
  
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

    HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_3);
    HAL_Delay(20);


}
/*****************************************************************
  *
  *Function Name: void Buzzer_ErrorSound(void)
  *Function: error sound
  *Input Ref:NO
  *Return Ref:NO
  *
*****************************************************************/
void Buzzer_ErrorSound(void)
{
    HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_3);
    HAL_Delay(50);

}

/*****************************************************************
  *
  *Function Name: void Buzzer_ErrorSound(void)
  *Function: high frequency 
  *Input Ref:NO
  *Return Ref:NO
  *
*****************************************************************/
void Buzzer_High_Sound(void)
{

    HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_3);
    HAL_Delay(30);

   
}

void Buzzer_High_Sound_2(void)
{

    HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_3);
    HAL_Delay(40);
  
}



/****************************************************************************
*
*Function Name:void Buzzer_Sound(void)
*Function : run is main 
*Input Ref: NO
*Retrun Ref:NO
*
****************************************************************************/
void Buzzer_Fail_Sound(void)
{
	   Buzzer_ErrorSound();//Buzzer_ShortSound();//Buzzer_ReSound();//fail sound has two sound //WT.EDIT 2022.09.13
	   HAL_TIM_PWM_Stop(&htim3,TIM_CHANNEL_3);
       HAL_Delay(50);
		
		Buzzer_ErrorSound();//Buzzer_ShortSound();//Buzzer_ReSound();//fail sound has two sound 
	 HAL_TIM_PWM_Stop(&htim3,TIM_CHANNEL_3);
         HAL_Delay(50);
		
		Buzzer_ErrorSound();
	 HAL_TIM_PWM_Stop(&htim3,TIM_CHANNEL_3);
       HAL_Delay(50);



}

void Buzzer_Sound_Stop(void)
{
   HAL_TIM_PWM_Stop(&htim3,TIM_CHANNEL_3);


}
/****************************************************************************
*
*Function Name:void Buzzer_Sound_Handler(void)
*Function : run is main 
*Input Ref: NO
*Retrun Ref:NO
*
****************************************************************************/
void Buzzer_Sound_Handler(void)
{

     static uint8_t sound_continuce;
    switch(run_t.buzzer_sound_tag){

	   case key_sound:
	   	    if(sound_continuce==0){
                sound_continuce++;
                Buzzer_KeySound();
              
                
            }
	        run_t.buzzer_sound_tag = buzzer_sound_null;

	  break;

	  case two_short_one_sound: //run_t.buzzer_two_short ==1
	 	   
            Buzzer_High_Sound();
	 	   	
	        run_t.buzzer_sound_tag = buzzer_sound_null;
       break;

	   case two_short_two_sound://run_t.buzzer_two_short ==2
        
	          Buzzer_Two_KeySound();//Buzzer_ShortSound(); //WT.EDIT 2022.09.13
	          Buzzer_Sound_Stop();
			  HAL_Delay(100);
			  Buzzer_Two_KeySound();
              Buzzer_Sound_Stop();
          	
		 run_t.buzzer_sound_tag = buzzer_sound_null;
			

	  break;
	  

	  case high_sound://run_t.buzzer_highsound_flag==1,one times new password confirm

         Buzzer_High_Sound();
         Buzzer_Sound_Stop();
	     HAL_Delay(100);
		 Buzzer_High_Sound_2();
         Buzzer_Sound_Stop();
			
		 run_t.buzzer_sound_tag = buzzer_sound_null;
	 break;


     case  clear_eeprom_sound: //run_t.clearEeeprom_done == 1

	 
		Buzzer_LongSound(); //WT.EDIT 2022.10.05
			
	
  		run_t.buzzer_sound_tag = buzzer_sound_null;
     break;

	 case confirm_sound://run_t.buzzer_longsound_flag ==1
	 
  		Buzzer_LongSound();
	 	
		  run_t.buzzer_sound_tag = buzzer_sound_null;
     break;

	 case fail_sound://run_t.fail_sound_flag ==1)
      
	       // Fail_Buzzer_Sound();
		   Buzzer_Fail_Sound();
     
		run_t.buzzer_sound_tag = buzzer_sound_null;
	 break;

	 case buzzer_sound_null:
        sound_continuce=0;
        Buzzer_Sound_Stop();
	 	   

	 break;


	}



}


