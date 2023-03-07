#include "led.h"
#include "run.h"
#include "gpio.h"
#include "buzzer.h"
#include "at24c02.h"
#include "adc.h"
#include "touchkey.h"
#include "tim.h"
#include "as608.h"




uint16_t adcVale;
uint16_t adcx;
uint8_t ps_sleep=0xff;
uint8_t ps_error_led=0xff;


float temp;  


void Panel_LED_Off(void)
{

      BACKLIGHT_OFF();
      PS_LED_ALL_OFF();//FP_POWER_OFF();
	  OK_LED_OFF();
	  ERR_LED_OFF();
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
    uint8_t i;
	static uint16_t cnt0,cnt,standby_cnt,clear_eeprom_flag;
    static uint8_t cntrecoder;
	switch(run_t.works_led_label){

		case works_ok_led_on:
			
			ERR_LED_OFF();
			OK_LED_OFF();
		    PS_Green_Led_ON();
			
			if(run_t.clearEeprom==1){
				run_t.clearEeprom = 0;
				run_t.gTimer_8s =0;
				ClearEEPRO_Data();
				Del_FR();//fingerprint be deteleted
				run_t.gTimer_8s =0;
				clear_eeprom_flag =1;
				run_t.clearEeeprom_count=0;
				run_t.works_led_label=works_ok_blink;
				 
			}
			else{
				if(run_t.gTimer_8s >8){
			        run_t.works_led_label= works_ok_led_off;
				}
			}
		break;

		case works_ok_blink: //05
		     run_t.gTimer_8s=0;
			 if(cnt0==0){
			 	cnt0++;
			 
			   PS_Green_Led_OFF();
			   ERR_LED_OFF();
			 }
			

		 
			if(run_t.gTimer_led_blink_500ms < 6 ){


				PS_Green_Led_ON();
				OK_LED_ON();

			}
			else if(run_t.gTimer_led_blink_500ms > 5 &&  run_t.gTimer_led_blink_500ms < 11){//500.WT.EDIT 2022.10.31


				PS_Green_Led_OFF();
				OK_LED_OFF();
			}

			if(run_t.gTimer_led_blink_500ms> 10 ){ //1000.WT.EDIT 2022.10.31
				run_t.gTimer_led_blink_500ms=0;
				if(clear_eeprom_flag ==1)
				     run_t.clearEeeprom_count++;
			
			}
          
			 if(run_t.clearEeeprom_count >3 &&  run_t.inputNewPassword_Enable ==0){
			 	run_t.clearEeeprom_count=0;
				cnt0 = 0;
			    clear_eeprom_flag =0;
                 run_t.works_led_label= works_ok_led_off;
			 }
		
		break;

		case works_ok_led_off: //2

		     if(run_t.gTimer_8s > 8){
	             OK_LED_OFF();
				 ERR_LED_OFF();
			     PS_LED_ALL_OFF();
				 BACKLIGHT_OFF();

			     run_t.works_led_label= works_null;
					 
				for(i=0;i<6;i++){ //WT.EDIT .2022.08.13
					*(pwd2 + i)=0;//pwd2[i]=0;
					*(Readpwd+i)=0;
					*(pwd1+i)=0;//pwd1[i]=0;

				}

			}
		break;

		case backlight_led_off:

			 BACKLIGHT_OFF(); 
			 
			 if(run_t.gTimer_8s > 8){
                 run_t.works_led_label= works_null;
			 }
		break;

		case works_error_led_on: //3
			ERR_LED_ON();
		    PS_Red_Led_ON();
		
		    OK_LED_OFF();	
           
		   
			run_t.gTimer_8s=0;
			run_t.inputNewPassword_Enable =0;//WT.EDIT 2022.10.0
		
			run_t.Confirm_newPassword=0; //WT.EDIT .2022.10.07
	        run_t.Numbers_counter =0;
			for(i=0;i<6;i++){ //WT.EDIT .2022.08.13
			*(pwd2 + i)=0;//pwd2[i]=0;
			*(Readpwd+i)=0;
			*(pwd1+i)=0;//pwd1[i]=0;

			}
			
            run_t.works_led_label= works_null;

        

		break;

		case works_error_led_off: //4
		    ERR_LED_ON();
		    OK_LED_OFF();	
            PS_Red_Led_ON();
		   
            run_t.works_led_label= works_null;
			
			for(i=0;i<6;i++){ //WT.EDIT .2022.08.13
			*(pwd2 + i)=0;//pwd2[i]=0;
			*(Readpwd+i)=0;
			*(pwd1+i)=0;//pwd1[i]=0;

			}
		break;

		case works_error_blink: //6
			
			 OK_LED_OFF();	
			if(cnt==0){
				cnt++;
			   PS_Red_Led_ON();
			   ERR_LED_ON();

			}

			run_t.inputNewPassword_Enable =0;//WT.EDIT 2022.10.05
			run_t.Confirm_newPassword=0; //WT.EDIT .2022.10.07
			if(run_t.gTimer_led_blink_500ms < 6 ){

				ERR_LED_OFF();
				PS_Red_Led_OFF();

			}
			else if(run_t.gTimer_led_blink_500ms > 5 &&  run_t.gTimer_led_blink_500ms < 11){
				ERR_LED_ON();
				PS_Red_Led_ON();
			}
			if(run_t.gTimer_led_blink_500ms > 10){
				cntrecoder++;
				run_t.gTimer_led_blink_500ms=0;

			}
		
			if(cntrecoder > 3){
			   cntrecoder =0;
				ERR_LED_OFF();
				PS_LED_ALL_OFF();
				cnt=0;
				
				for(i=0;i<6;i++){ //WT.EDIT .2022.08.13
								*(pwd2 + i)=0;//pwd2[i]=0;
								*(Readpwd+i)=0;
								*(pwd1+i)=0;//pwd1[i]=0;
				
							}
			    run_t.works_led_label= works_null;

			}
			
			
		break;

		case factory_led_test:
		   if(run_t.gTimer_factory < 61 &&  run_t.factory_test ==1){
				run_t.gTimer_8s=0;
				run_t.panel_lock=1;
				BACKLIGHT_ON();
				OK_LED_ON();
				ERR_LED_ON();
				BAT_LED_ON();
			}

			if(run_t.gTimer_factory > 60){
				run_t.factory_test =0;
				run_t.gTimer_8s=10;
			    run_t.panel_lock=0;
				BACKLIGHT_OFF();
				OK_LED_OFF();
				ERR_LED_OFF();
				BAT_LED_OFF();
	            FP_POWER_OFF();
	            run_t.works_led_label= works_null;
				
			}
			

		break;

		case works_null: //7

		    
            PS_LED_ALL_OFF();
			OK_LED_OFF();
		    ERR_LED_OFF();
			BACKLIGHT_OFF();
        
             if(run_t.panel_lock ==1){
                if(run_t.gTimer_60s > 60){
				   run_t.gTimer_60s=0;
                   run_t.error_times=0;
				   run_t.panel_lock=0;


				}
			 }
			 else{
	            run_t.works_led_label = standby_led;
				standby_cnt =0;
				run_t.gTimer_8s =0;
				
			 }
			
		break;

		 case standby_led :
            
		    
			 if(run_t.gTimer_8s > 8 && standby_cnt ==0){
			 	 run_t.gTimer_8s =0;
				 standby_cnt ++;
			}
		  	else if(run_t.gTimer_8s > 8 && run_t.inputNewPassword_Enable ==0){
	            run_t.gTimer_8s =0;
	            Standby_Model_Handler();

			}

			
		  
         break;


		 default:
		 break;

    
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
	  BACKLIGHT_ON(); 
}
/****************************************************************************
*
*Function Name:void Standby_Model_Handler()
*Function : Standby input condition function
*Input Ref: NO
*Retrun Ref:NO
*
****************************************************************************/
void Standby_Model_Handler()
{
 	   
        run_t.lowPower_flag=0;
		ps_sleep=PS_Sleep();
		Panel_LED_Off();
		POWER_OFF();
		FP_POWER_OFF();

		__HAL_RCC_PWR_CLK_ENABLE();
		HAL_TIM_Base_Stop_IT(&htim14);//
		HAL_SuspendTick();
		SysTick->CTRL = 0x00;//关闭定时器
		SysTick->VAL = 0x00;//清空val,清空定时器

		LowPower_GPIO_Set();
		HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON,PWR_STOPENTRY_WFI);//WFI ->wait for interrupt
		SystemClock_Config();//Low power of low frequency 8MHz


	
}


