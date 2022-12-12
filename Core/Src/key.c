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
    * Input Ref: GPIO_Pin：interrupt of GPIO of pin number
    * Return Ref: 无
    * 说    明: 无
    *
*******************************************************************************/
void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin)
{

    if(GPIO_Pin == KEY_INPUT_Pin){
		 POWER_ON();
   
	   __HAL_GPIO_EXTI_CLEAR_IT(KEY_INPUT_Pin);
	
		if(run_t.lowPower_flag==0){
			 do{
	            run_t.ADC_times=0;
		  
			 	run_t.lowPower_flag++;
				if(run_t.normal_works_state ==0){//WT.EDIT 2022.10.08
				 run_t.normal_works_state++;
			     run_t.input_lowpower_stopmodel_flag=0;
			  	 SystemClock_Config();
				 HAL_ResumeTick();
				 HAL_TIM_Base_Start_IT(&htim14);//
				//  MX_USART1_UART_Init();
				  
				 run_t.inputDeepSleep_times =0;
				
				 	
				}
				run_t.readI2C_data =1;//WT.EDIT 2022.09.26 jump the "if(run_t.touchkey_first_turn_on_led==1 && run_t.panel_lock ==0)"
				run_t.touchkey_first_turn_on_led =1;//WT.EDIT 2022.09.26
			    run_t.gTimer_8s=0;//WT.EDIT 2022.09.26
			   
			 	}while(run_t.lowPower_flag==0); 
		}
	   	

	}
}







void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin)
{
   static  uint8_t touchkey=0xff;
    


   if(GPIO_Pin == SC12B_INT_INPUT_Pin){
   
      run_t.input_lowpower_stopmodel_flag=0;
        POWER_ON();
       __HAL_GPIO_EXTI_CLEAR_IT(SC12B_INT_INPUT_Pin);//WT.EDIT 2022.09.09
      if(run_t.lowPower_flag==0){
      do{
	 	    run_t.lowPower_flag++;

	        run_t.ADC_times=0;
		 
			run_t.input_lowpower_stopmodel_flag=0;
		  	 SystemClock_Config();
			  HAL_ResumeTick();
			  HAL_TIM_Base_Start_IT(&htim14);//
			//  MX_USART1_UART_Init();
           
    		run_t.readI2C_data =1;
		  	 run_t.inputDeepSleep_times =0;
		  
		
		 if(touchkey != run_t.touchkey_first && run_t.Confirm_newPassword ==0){//2022.10.19
		 	  touchkey = run_t.touchkey_first;
	         // run_t.touchkey_first_turn_on_led =1;
		      run_t.readI2C_data =0;//WT.EDIT 2022.09.26
			  run_t.gTimer_200ms=0;
			  run_t.gTimer_8s=0;//WT.EDIT 2022.10.08
		 }
      	}while(run_t.lowPower_flag ==0);
      	
    }
}

  
   //fingerprint 
   if(GPIO_Pin==FP_INT_INPUT_Pin){
         syspara_t.PS_wakeup_flag=1;
		  POWER_ON();
		  FP_POWER_ON()  ;
       __HAL_GPIO_EXTI_CLEAR_IT(FP_INT_INPUT_Pin);//WT.EDIT 2022.09.09
      if(run_t.lowPower_flag==0){

	       do{

            run_t.ADC_times=0;
			run_t.lowPower_flag++;
	      	run_t.input_lowpower_stopmodel_flag=0;
		    SystemClock_Config();
	        HAL_ResumeTick();
			HAL_TIM_Base_Start_IT(&htim14);//
			//MX_USART1_UART_Init();
           //  HAL_UART_Receive_IT(&huart1,UART1_RX_DataBuf,sizeof(UART1_RX_DataBuf));
		   POWER_ON();
		  FP_POWER_ON()  ;
		  	run_t.inputDeepSleep_times =0;
	       	}while(run_t.lowPower_flag==0);
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
		POWER_ON();
		TouchKey_Led_Handler();//BACKLIGHT_2_ON();
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
			if(key.read == key.buffer) // adjust key be down 
			{
				if(++key.on_time> 100 && ++key.on_time < 500) //1000  0.5us -> short time key
				{
					key.value = key.buffer^_KEY_ALL_OFF; // key.value = 0x1E ^ 0x1f = 0x01, com = 0x0E ^ 0x1f = 0x11
					key.on_time = 0;
                    key.state   = second;
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
                        BUZZER_KeySound();//Buzzer_ShortSound(); //WT.EDIT 2022.10.05
                        BUZZER_OFF(); 
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
                        BUZZER_OFF(); 
                        run_t.gTimer_8s=0;//WT.EDIT 2022.10.26
                        run_t.eeprom_Reset_flag =1; //WT.EDIT 2022.10.26
                        run_t.inputDeepSleep_times =0; //WT.EDIT 2022.10.26
						run_t.BackLight =1; //WT.EDIT 2022.10.26
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
		run_t.inputDeepSleep_times =0;
		run_t.inputNewPassword_Enable =0;
		run_t.gTimer_8s=0;
		run_t.buzzer_key_sound_flag =1;//WT.EDIT 2022.10.06
		run_t.BackLight =1; //WT.EDIT .2022.10.06

       	run_t.inputNewPasswordTimes =0;
		//run_t.password_unlock=0;//accomplish by save task//WT.EIDT 2022.09.12
        run_t.password_unlock_model=0;
		run_t.open_lock_fail=0;//run_t.lock_fail =0;
		run_t.open_lock_success=0; 
		run_t.Numbers_counter =0;
		run_t.motor_return_homePosition=0;
		syspara_t.ps_thefirst_input_fp =0;//clear fingerprint flag
	
        BACKLIGHT_2_ON();        
		
		POWER_ON();//WT.EDIT .2022.10.06
		OK_LED_OFF();//WT.EDIT .2022.10.31
	        
       }
	 
      if(keyvalue== 0x81){

        run_t.clearEeprom = 1;
		run_t.inputDeepSleep_times =0;
        run_t.gTimer_8s=0;
		
       }




}

