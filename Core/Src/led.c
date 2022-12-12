#include "led.h"
#include "run.h"
#include "gpio.h"
#include "buzzer.h"
#include "at24c02.h"
#include "adc.h"
#include "touchkey.h"
#include "tim.h"
#include "as608.h"


static void BackLight_Fun(void);
static void Buzzer_RunSound(void);
//static void Fail_Buzzer_Sound(void);
static void Panle_InputTimesError_LED_Off(void);


uint16_t adcVale;
uint16_t adcx;
float temp;  
uint8_t ps_sleep=0xff;

void Panel_LED_Off(void)
{

          BACKLIGHT_2_OFF();
		  OK_LED_OFF();
		  ERR_LED_OFF();
		  BAT_LED_OFF();

}
/****************************************************************
	*
	*Function Name:void DisplayLed_Handler(void)
	*Function : display back light and buzzer sound 
	*Input Ref:NO
	*Return Ref:NO
	*
****************************************************************/
static void Panle_InputTimesError_LED_Off(void)
{
	 BACKLIGHT_2_OFF();
	 OK_LED_OFF();
	// ERR_LED_OFF();
	 BAT_LED_OFF();
}
/****************************************************************
*
*Function Name:void DisplayLed_Handler(void)
*Function : display back light and buzzer sound 
*Input Ref:
*Return Ref:
*
****************************************************************/
void DisplayLed_Handler(void)
{

        Buzzer_RunSound();
         
        Buzzer_InputNewPassword_two_short();
		BackLight_Fun();
		

		  //erase EEPRO data 
		  if(run_t.clearEeprom==1){
			  run_t.clearEeprom = 0;
			  run_t.gTimer_8s =0;
			  ClearEEPRO_Data();
			  Del_FR();//fingerprint be deteleted
			   run_t.gTimer_8s =0;
		       run_t.led_blank = 1;
			   run_t.clearEeeprom_done = 1;
			  run_t.inputDeepSleep_times =0;//WT.EDIT 2022.10.26
			  run_t.buzzer_longsound_flag =1 ;
			   run_t.buzzer_fail_sound_flag =0;
			   run_t. clearEeeprom_count=0;
                Del_FR();//fingerprint be deteleted
			 
		  }

 
	   if(run_t.panel_lock ==1){
			
			
			 Panle_InputTimesError_LED_Off();

			 run_t.open_lock_fail=0;//run_t.lock_fail=0;
			 run_t.BackLight=0;

		
         
		     ERR_LED_ON(); //WT.EDIT 202209.28

         

         if(run_t.gTimer_input_error_times_60s > 5){
			   run_t.panel_lock =0;
			   run_t.error_times = 0;
		       ERR_LED_OFF(); //WT.EDIT 2022.09.20
 
		   }
         
 
	   }

	   
	   if(run_t.gTimer_ADC >10 && run_t.input_lowpower_stopmodel_flag==0){
	   	
              run_t.gTimer_ADC=0;
			
			     POWER_ON();
				adcx=Get_Adc_Average(10);
				//adcx =Get_Adc();
				temp=(float)adcx*(3.3/4096); //3.111
                // temp=(float)adcx*(2.7/4096);
		         temp = temp *1000; //31.11V
		        adcVale =(uint16_t)(temp);
				
				if(adcVale < 1500 ){ // low 3.3V is alarm
				    run_t.ADC_times++; //WT.EDIT 2022.09.09
				   if(run_t.ADC_times > 3 ){
				   	run_t.ADC_times = 4;
				      BAT_LED_ON();
				   }

				}
				else{
				    run_t.ADC_times = 0;
					BAT_LED_OFF();
					HAL_ADC_Stop(&hadc1);  
                    
				}
			
		
	   }
      

}


/****************************************************************************
*
*Function Name:void BackLight_Fun(void)
*Function : run is main 
*Input Ref: NO
*Retrun Ref:NO
*
****************************************************************************/
static void BackLight_Fun(void)
{
    uint8_t i;
	static uint8_t cntrecoder;
	static uint16_t cnt,cnt0;
	//back light turn on or turn off function

      if(run_t.login_in_success ==1){//WT.DEIT 2022.10.31
		  	if(run_t.gTimer_1s >2){
				 
			    run_t.stop_gTimer_8s=1;
                  
			}
	   }
		    

	
	   if(run_t.BackLight ==0){ //WT.EDIT 2022.10.07
	     
	         BACKLIGHT_2_OFF();

	   }
	   else if(run_t.BackLight ==1){
	   	
	   	    
	           BACKLIGHT_2_ON();
			  
	  }
	
	 //Stop_Mode
	 //turn off touch key of LED and function LED function
	  if(((run_t.gTimer_8s >8 && run_t.factory_test !=1 && run_t.panel_lock ==0)|| run_t.stop_gTimer_8s==1)&& run_t.eeprom_Reset_flag !=1){

            if(run_t.inputNewPassword_Enable ==0 && run_t.led_blank==0){
		  	if(run_t.open_lock_fail==0 && run_t.input_newPassword_over_number==0){
	  	 
		   run_t.login_in_success =0;//WT.EDIT 2022.10.31
		   run_t.gTimer_1s=0;
		  run_t.stop_gTimer_8s =0;
		  run_t.BackLight =0;
		  run_t.open_lock_fail=0;//run_t.lock_fail=0;
		  run_t.gTimer_8s=0;
		  run_t.gTimer_200ms=0;//WT.EDIT 2022.10.19
          syspara_t.PS_login_times=0;	//fingerprint input times 
		   Panel_LED_Off();
		  HAL_ADC_Stop(&hadc1);
		
		  
		 run_t.led_blank =0;
         run_t.passwordsMatch =0 ;
	     run_t.powerOn =3;
         
		 
	
	  
		 run_t.motor_return_homePosition=0;

		 //clear new password flag
		 if(syspara_t.PS_wakeup_flag==0)
               run_t.Confirm_newPassword =0; //WT.EDIT 2022.09.28
		 run_t.inputNewPassword_Enable =0; //WT.EDIT 2022.09.28
         run_t.Numbers_counter =0;
		 run_t.clear_inputNumbers_newpassword=0;//WT.EDIT 2022.10.14
		 
         //wake up touch key
         run_t.touchkey_first ++; //WT.EDIT 2022.10.19 ->touch key delay times 
         run_t.touchkey_first_turn_on_led=0;//WT.EDIT.2022.10.28
         run_t.readI2C_data =0; ////WT.EDIT.2022.10.28

        for(i=0;i<6;i++){ //WT.EDIT .2022.08.13
				*(pwd2 + i)=0;//pwd2[i]=0;
                *(Readpwd+i)=0;
				*(pwd1+i)=0;//pwd1[i]=0;

		 }
        //PS_Sleep(void);
		POWER_OFF();
		//FP_POWER_OFF(); //WT.EDIT 2022.11.26 fingerprint
	     if(run_t.inputDeepSleep_times > 2){  //wait 20s  
				   run_t.inputDeepSleep_times =0;
				   run_t.normal_works_state =0;
			       run_t.touchkey_first =0; //WT.EDIT 2022.09.26
	          		/*close tick timer low power Mode */
				    run_t.gTimer_10s=0;
				    run_t.lowPower_flag=0;
                    ps_sleep=PS_Sleep();
                    FP_POWER_OFF();
             
                  __HAL_RCC_PWR_CLK_ENABLE();
                 HAL_TIM_Base_Stop_IT(&htim14);//
                  HAL_SuspendTick();
					SysTick->CTRL = 0x00;//关闭定时器
	                SysTick->VAL = 0x00;//清空val,清空定时器

				
					
				   LowPower_GPIO_Set();
					 syspara_t.PS_wakeup_flag=0;
			       HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON,PWR_STOPENTRY_WFI);//WFI ->wait for interrupt
                   run_t.input_lowpower_stopmodel_flag=1;
                  SystemClock_Config();//Low power of low frequency 8MHz
			       
                    
                    
				   
			  	}
		
	
	  	   }
	  	}
	  }
	  /*____________________error led blank _________________________*/
	 //LED error bank function .
	  if(run_t.open_lock_fail_led==1 || run_t.input_newPassword_over_number==1){
		   cnt ++ ;
		//  run_t.BackLight =0;//WT.EDIT .2022.10.19
		  run_t.inputNewPassword_Enable =0;//WT.EDIT 2022.10.05
	      run_t.led_blank	=0;
		  run_t.password_unlock_model=0;
		  run_t.open_lock_success=0;
     
		     run_t.Confirm_newPassword=0; //WT.EDIT .2022.10.07
		  run_t.inputNewPasswordTimes =0;//WT.EDIT .2022.10.07
		
		  OK_LED_OFF();
	
	  
		  if(cnt < 501 ){
	
			  ERR_LED_OFF();
			  
		  }
		  else if(cnt > 500 && cnt < 1001){
			  ERR_LED_ON();
		  }
		  if(cnt>1000){
		  	cnt = 0;
			cntrecoder++;
			
		  }
		  if(cntrecoder > 2){
		  	cntrecoder =0;
		 
		      run_t.saveEEPROM_fail_flag =0;
			  run_t.open_lock_fail_led=0;//run_t.lock_fail=0;
			  run_t.input_newPassword_over_number=0;

			  ERR_LED_OFF();
              run_t.stop_gTimer_8s=1;
		 
		  }
	
	  }
	 
	
	 //OK_LED blank function
	  if((run_t.inputNewPassword_Enable ==1 || run_t.led_blank	==1) && run_t.BackLight !=2){	
	    
		 cnt0 ++ ;
		  ERR_LED_OFF();
		 run_t.open_lock_fail_led=0;//run_t.lock_fail=0;
		 run_t.input_newPassword_over_number=0;
	     run_t.readI2C_data =1;
	     run_t.gTimer_8s=0; //WT.EDIT 2022.10.14
	
		  if(run_t.led_blank ==1 &&   run_t.clearEeeprom_done == 1){
		  	run_t.clearEeeprom_done = 0;

			Buzzer_LongSound(); //WT.EDIT 2022.10.05
		  }
	
		  if(cnt0 < 501 ){
	
			  
               OK_LED_OFF();
			  
		  }
		  else if(cnt0>499 && cnt0 < 1001){//500.WT.EDIT 2022.10.31
			  OK_LED_ON();
		  }
	
		  if(cnt0>999){ //1000.WT.EDIT 2022.10.31
		  	cnt0 = 0;
           

		    run_t.clearEeeprom_count++;
		   
		    if(run_t.inputNewPassword_Enable ==1)
		          run_t.inputNewPwd_OK_led_blank_times ++ ;
		  	}
		  
		    if(run_t.inputNewPassword_Enable ==1 && run_t.eeprom_Reset_flag ==0){//WT.EDIT 2022.10.08
		    
				if(run_t.inputNewPwd_OK_led_blank_times >9){
					 run_t.inputNewPwd_OK_led_blank_times=0;
					 run_t.BackLight =0;
					 run_t.inputNewPassword_Enable =0;
				   
				     OK_LED_OFF();
					 run_t.stop_gTimer_8s =1;
              
					
					 
				 }
                  
			}
		    
            //-------------------------------------------//
		      if(run_t.led_blank	==1 && syspara_t.PS_exit_login_flag==0){ //EDIT.WT.2022.09.28
                 if(run_t.clearEeeprom_count >2){
					 run_t.clearEeeprom_count=0;
					 run_t.led_blank=0;
				     OK_LED_OFF();
					 run_t.stop_gTimer_8s=1;
					 if(run_t.eeprom_Reset_flag ==1)
					     run_t.eeprom_Reset_flag =0;//WT.EDIT 2022.10.26
					
				 }
				 	
			}

				
		}
	//Factory test all LED be check process
	  if(run_t.factory_test ==1){
		
	      run_t.gTimer_8s=0;
		  BACKLIGHT_2_ON();
		  OK_LED_ON();
		  ERR_LED_ON();
		  BAT_LED_ON();
	
		if(run_t.gTimer_input_error_times_60s > 5){
			run_t.factory_test =0;
			run_t.gTimer_8s=10;
			  BACKLIGHT_2_OFF();
			  OK_LED_OFF();
			  ERR_LED_OFF();
			  BAT_LED_OFF();
	
	
		}
	
	  }

}
/****************************************************************************
*
*Function Name:void  void Buzzer_RunSound(void)
*Function : run is main 
*Input Ref: NO
*Retrun Ref:NO
*
****************************************************************************/
void Buzzer_RunSound(void)
{

	 if(run_t.buzzer_longsound_flag ==1 && run_t.buzzer_fail_sound_flag ==0){
		   run_t.buzzer_longsound_flag =0;
		   run_t.buzzer_key_sound_flag=0;
		   Buzzer_LongSound();
		  
	      
	 }
     else{


	 if(run_t.buzzer_fail_sound_flag ==1){
	 	  run_t.buzzer_fail_sound_flag=0; //WT.EDIT 2022.10.06
		   Fail_Buzzer_Sound();
	       run_t.buzzer_key_sound_flag=0;//WT.EDIT 2022.10.19
	       syspara_t.ps_error_times_key++;
		}
		else if(run_t.buzzer_key_sound_flag==1){
			run_t.buzzer_key_sound_flag=0;//WT.EDIT 2022.10.06
		    BUZZER_KeySound();
		}

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
void  Buzzer_InputNewPassword_two_short(void)
{

     if(run_t.buzzer_fail_sound_flag==0){//WT.EDIT 2022.10.06
	 	
      if(run_t.buzzer_two_short ==1){
	  	  
          run_t.buzzer_two_short =0;
		   run_t.inputDeepSleep_times =0; //WT.EDIT 2022.10.26
		   run_t.gTimer_8s=0;
          Buzzer_High_Sound();
	      run_t.buzzer_key_sound_flag=0;
		
		 }


      if(run_t.buzzer_two_short ==2){
          run_t.buzzer_two_short =0;
		        run_t.inputDeepSleep_times =0;//WT.EDIT 2022.10.26
                run_t.gTimer_8s=0;
                BUZZER_KeySound();//Buzzer_ShortSound(); //WT.EDIT 2022.09.13
				HAL_Delay(50);
				BUZZER_KeySound();
				run_t.buzzer_key_sound_flag=0;
		
			

        }
	  
     }

	 if(run_t.buzzer_highsound_flag==1){
	     run_t.buzzer_highsound_flag=0;
          Buzzer_High_Sound();
          HAL_Delay(50);
	      Buzzer_High_Sound_2();
	 


	 }

}
/************************************************************
 	*
 	* Function Name :void TouchKey_Led(void)
 	* 
 	* 
 	* 
*************************************************************/
void TouchKey_Led_Handler(void)
{
	  BACKLIGHT_2_ON(); 
}
/****************************************************************************
*
*Function Name:void HAL_TIM_PeriodElapsedHalfCpltCallback(TIM_HandleTypeDef *htim)
*Function : half -> 16bit, TIM2 timing time is 10ms 
*Input Ref: NO
*Retrun Ref:NO
*
****************************************************************************/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{

    static unsigned char t0;

	if(htim->Instance==TIM14){

	if(run_t.input_lowpower_stopmodel_flag==0){
  
    t0++;
    
	run_t.gTimer_200ms ++;
    if(t0>99){ //10*100 =1000ms "1s"
       t0=0;
	   
	  run_t.gTimer_10s_start++;
	   run_t.gTimer_1s ++;
	   
	   run_t.gTimer_8s++;
	   run_t.gTimer_10s ++;
	   run_t.gTimer_ADC ++;
	  
	   if(run_t.gTimer_10s_start>9){ //10s
		 run_t.gTimer_10s_start=0;
		 run_t.gTimer_input_error_times_60s++;
	     run_t.inputDeepSleep_times++;
		
	    

	   }
	
    } 
	}

	}
}


