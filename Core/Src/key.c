#include "key.h"
#include "main.h"
#include "gpio.h"
#include "run.h"
#include "led.h"
#include "buzzer.h"
#include "as608.h"
#include "tim.h"
#include "touchkey.h"
#include "single_mode.h"
#include "usart.h"
#include "cmd_link.h"
#include "adc.h"
#include "at24c02.h"
#include "delay.h"


key_types key;
uint8_t buzzertimes;

/*******************************************************************************
    *
    * Function Name: void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
    * Function : Interrupt function call back 
    * Input Ref: GPIO_Pin£ºinterrupt of GPIO of pin number
    * Return Ref: ÎÞ
    * Ëµ    Ã÷: ÎÞ
    *
*******************************************************************************/
void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin)
{
    

	if(GPIO_Pin == KEY_INPUT_Pin){
		 POWER_ON();
        FP_POWER_ON()  ;
         BACKLIGHT_ON();
	   __HAL_GPIO_EXTI_CLEAR_IT(KEY_INPUT_Pin);
	
		if(run_t.lowPower_flag==0){
			SystemClock_Config();
			HAL_ResumeTick();
			HAL_TIM_Base_Start_IT(&htim14);//


			
			POWER_ON();
			FP_POWER_ON()  ;
			BACKLIGHT_ON();
			run_t.lowPower_flag++;
		
			run_t.backlight_Cmd_lable =0xff;	 	
			
			 run_t.gTimer_8s=0;
			 run_t.inputDeepSleep_times =0; 
			 	
		}
	   	

	}
}

void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin)
{

	static uint8_t rx_times=0xff;

   if(GPIO_Pin == SC12B_INT_INPUT_Pin){
   
     
       POWER_ON();
       FP_POWER_ON()  ;
	   BACKLIGHT_ON();
	  

       __HAL_GPIO_EXTI_CLEAR_IT(SC12B_INT_INPUT_Pin);//WT.EDIT 2022.09.09
      
      if(run_t.lowPower_flag==0){
			SystemClock_Config();
			HAL_ResumeTick();
			HAL_TIM_Base_Start_IT(&htim14);//

			POWER_ON();
			FP_POWER_ON()  ;
			run_t.lowPower_flag++;
			
			run_t.buzzer_sound_lable=sound_key;//Buzzer_KeySound();
			
			run_t.inputDeepSleep_times =0;
			run_t.backlight_Cmd_lable =0xff;
		    run_t.gTimer_8s=0;
		}

 	}
  
   //fingerprint 
  if(GPIO_Pin==FP_INT_INPUT_Pin){
         syspara_t.PS_wakeup_flag=1;
		  POWER_ON();
		  FP_POWER_ON()  ;
		  BACKLIGHT_ON();
		  if(rx_times !=syspara_t.fp_rx_times){
		  	    rx_times =syspara_t.fp_rx_times;
		  		syspara_t.handler_read_data_flag++;
		  }
		  __HAL_GPIO_EXTI_CLEAR_IT(FP_INT_INPUT_Pin);//WT.EDIT 2022.09.09
     if(run_t.lowPower_flag==0){

		
		SystemClock_Config();
		HAL_ResumeTick();
		HAL_TIM_Base_Start_IT(&htim14);//

		POWER_ON();
		FP_POWER_ON()  ;
		BACKLIGHT_ON();
		//if(rx_times !=syspara_t.fp_rx_times){
			//rx_times =syspara_t.fp_rx_times;
			Buzzer_KeySound();

		///}
		
	    run_t.gTimer_8s=0;//WT.EDIT 2022.10.08
      
		run_t.lowPower_flag++;
	    run_t.backlight_Cmd_lable =0xff;
	    run_t.gTimer_8s=0;
		run_t.inputDeepSleep_times =0;
	      
      }
	}

}

  
   
/*******************************************************************************
    *
    * Function Name: unsigned char  Scan_Key(void)
    * Function : state machine of key 
    * Input Ref: NO
    * Return Ref: key of value 
    *
*******************************************************************************/
uint8_t Scan_Key(void)
{
  unsigned char   reval = 0;
  key.read = _KEY_ALL_OFF; //0x1F 
   if(HAL_GPIO_ReadPin(KEY_INPUT_GPIO_Port,KEY_INPUT_Pin) ==0 )
	{
		key.read &= ~0x01; // 0x1f & 0xfe =  0x1E
		//POWER_ON();
		//TouchKey_Led_Handler();//BACKLIGHT_2_ON();
	}
	
	
	switch(key.state )
	{
		case start:
		{
			if(key.read != _KEY_ALL_OFF)
			{
				key.buffer   = key.read; //??:key.buffer = 0x1E  POWER KEY 
				key.state    = first;
				key.on_time  = 0;
				key.off_time = 0;
           
                
			}
			break;
		}
		case first: //shot key times 
		{
			if(key.read == key.buffer) // adjust key be down  short key
			{
				if(++key.on_time> 100 && ++key.on_time < 800) //1000  0.5us -> short time key
				{
					key.value = key.buffer^_KEY_ALL_OFF; // key.value = 0x1E ^ 0x1f = 0x01, com = 0x0E ^ 0x1f = 0x11
					key.on_time = 0;
                    key.state   = second;
					//Buzzer_KeySound();
                 }
			}
			else
			{
				key.state   = start;
			}
			break;
		}
		case second: //long key times 
		{
			if(key.read == key.buffer) //again adjust key if be pressed down 
			{
				if(++key.on_time>1000)// 2000 = 7s long key be down
				{
					
					//key.value = key.value|0x80; //key.value = 0x01 | 0x80  =0x81  
					key.on_time = 0;
					key.state   = finish;
					do{
                        buzzertimes++;
						run_t.gTimer_8s=0;//WT.EDIT 2022.10.26
						run_t.inputDeepSleep_times =0; //WT.EDIT 2022.10.26
                        Buzzer_KeySound();
                        BUZZER_OFF(); 
                        ERR_LED_OFF();
                        PS_Blue_Led_ON();
                        HAL_Delay(400);
                       if(HAL_GPIO_ReadPin(KEY_INPUT_GPIO_Port,KEY_INPUT_Pin) ==1){
                         buzzertimes=0;
                         return 0;
                       }

                    }while(buzzertimes < 11);
                    if(buzzertimes > 9){
                        buzzertimes=0;
                        Buzzer_ShortSound();
                        BUZZER_OFF(); 
                        HAL_Delay(200);
                        Buzzer_ShortSound();
                        BUZZER_OFF(); //BUZZER_OFF(); 
                        run_t.gTimer_8s=0;//WT.EDIT 2022.10.26
                        run_t.eeprom_Reset_flag =1; //WT.EDIT 2022.10.26
                        run_t.inputDeepSleep_times =0; //WT.EDIT 2022.10.26
                        ERR_LED_OFF();
                        while(HAL_GPIO_ReadPin(KEY_INPUT_GPIO_Port,KEY_INPUT_Pin) ==0);
                        key.value = key.value|0x80;
                    }

                   
			  }
			}
			else if(key.read == _KEY_ALL_OFF)  // loose hand 
			{
				if(++key.off_time>5) //30 don't holding key dithering
				{
					key.value = key.buffer^_KEY_ALL_OFF; // key.value = 0x1E ^ 0x1f = 0x01
					
					key.state   = finish; // loose hand,shot time be pressed 
				}
			}
		   
			break;
		}
		case finish:
		{
			
			reval = key.value; // is short time  TIMER_KEY = 0x01  2. long times TIMER_KEY = 0X81
			key.state   = end;
         
			break;
		}
		case end:
		{
			if(key.read == _KEY_ALL_OFF)
			{
				if(++key.off_time>5)//50 //100
				{
					key.state   = start;
                  
				}
			}
			break;
		}
		default:
		{
			key.state   = start;
         
			break;
		}
	}
	return  reval;


}


/*******************************************************
 * 
 * Function Name: void  SideKey_Fun(uint8_t keyvalue)
 * Function: 
 * INPUT Ref:NO
 * Return Ref:NO
 * 
********************************************************/
void  SideKey_Fun(uint8_t keyvalue)
{
     
	 if(keyvalue == 0x01){
               
		run_t.Confirm_newPassword = 1;
		syspara_t.PS_wakeup_flag=0;
		run_t.inputDeepSleep_times =0;
		run_t.gTimer_input_standby_cnt=0;
		run_t.inputNewPassword_Enable =0;
		run_t.gTimer_8s=0;
		run_t.buzzer_sound_lable= sound_key;//run_t.buzzer_key_sound_flag =1;//WT.EDIT 2022.10.06
		

       	run_t.inputNewPasswordTimes =0;
        run_t.password_unlock_model=0; 
		run_t.Numbers_counter =0;
		run_t.motor_return_homePosition=0;
	    
	
        BACKLIGHT_ON();        
		FP_POWER_ON();
		POWER_ON();//WT.EDIT .2022.10.06
		OK_LED_OFF();//WT.EDIT .2022.10.31
		ERR_LED_OFF();
		PS_Blue_Led_ON();
	
	        
       }
	 
      if(keyvalue== 0x81){
	  	
		syspara_t.PS_wakeup_flag=0;
        run_t.works_led_lable=works_ok_led_on;
        run_t.clearEeprom = 1;
		run_t.inputDeepSleep_times =0;
        run_t.gTimer_8s=0;

		
       }




}

