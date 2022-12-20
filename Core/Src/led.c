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

static void Works_IndicateLed(void);

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

    
	BackLight_Fun();
	Works_IndicateLed();

	if(run_t.panel_lock ==1){
		      Panel_LED_Off();
         	
              ERR_LED_ON();
         	 
         	 run_t.inputDeepSleep_times =0;
         	  run_t.gTimer_input_standby_cnt=0;
         	  run_t.gTimer_8s=0;
             if(run_t.gTimer_input_error_times_60s > 5){
			   run_t.panel_lock =0;
			   run_t.error_times = 0;
               run_t.works_led_lable= works_error_led_off;
	      }
     }




	
	if(run_t.gTimer_ADC >10 ){
   	
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
			// else{
			    run_t.ADC_times = 0;
				BAT_LED_OFF();
				HAL_ADC_Stop(&hadc1);  
                
			}
		
	
   

   if(run_t.gTimer_8s > 8 && (run_t.backlight_run_flag==0 || run_t.backlight_Cmd_lable ==0xff ) && run_t.clearEeprom ==0\
   		&& run_t.inputNewPassword_Enable ==0){
      run_t.backlight_Cmd_lable=backlight_led_off;
	  run_t.Confirm_newPassword=0;
   	  Panel_LED_Off();

   }
  

}
/****************************************************************************
*
*Function Name:void BackLight_Fun(void)
*Function : backlingh of touchky of led
*Retrun Ref:NO
*
****************************************************************************/
static void BackLight_Fun(void)
{
	uint8_t i;
	

switch(run_t.backlight_Cmd_lable){
     
     case backlight_led_on:
		if(run_t.panel_lock==0){
		 BACKLIGHT_ON();
        
		if(run_t.gTimer_8s > 8){

          run_t.backlight_Cmd_lable = backlight_led_off;
          run_t.gTimer_8s=0;
		}
		else{
             run_t.gTimer_input_standby_cnt=0;
		}
		}
		else{
           BACKLIGHT_OFF();
		   PS_LED_ALL_OFF();

           run_t.lowPower_flag=0;
		}
		
		run_t.backlight_run_flag=1;
     break;

     case backlight_led_off:
         
       OK_LED_OFF();
		ERR_LED_OFF();
		PS_LED_ALL_OFF();
	    BACKLIGHT_OFF();

       run_t.gTimer_8s=0;
	  // syspara_t.handler_read_data_flag++;
	   syspara_t.PS_login_times=0;
	   syspara_t.PS_wakeup_flag=0;
	   run_t.Confirm_newPassword = 0;

        run_t.lowPower_flag=0;
	    syspara_t.PS_login_times=0;	//fingerprint input times 
		Panel_LED_Off();
		HAL_ADC_Stop(&hadc1);
	
      
		run_t.passwordsMatch =0 ;
		run_t.powerOn =3;
		run_t.motor_return_homePosition=0;

		//clear new password flag
	
		run_t.inputNewPassword_Enable =0; //WT.EDIT 2022.09.28
		run_t.Numbers_counter =0;
		run_t.clear_inputNumbers_newpassword=0;//WT.EDIT 2022.10.14

		//wake up touch key
	

		for(i=0;i<6;i++){ //WT.EDIT .2022.08.13
		*(pwd2 + i)=0;//pwd2[i]=0;
		*(Readpwd+i)=0;
		*(pwd1+i)=0;//pwd1[i]=0;

		}

		run_t.gTimer_input_standby_cnt=0;
        run_t.backlight_Cmd_lable = backlight_led_confirm;
		run_t.backlight_run_flag=1;
     break;


	case backlight_led_confirm:

	      if(run_t.gTimer_8s >8 ){
		    run_t.backlight_Cmd_lable = standby_led;

		 }
		  else{

		     run_t.gTimer_input_standby_cnt=0;

		  }

		run_t.backlight_run_flag=1;
	break;

    case standby_led:
        
	   if(run_t.gTimer_input_standby_cnt>0){

	   		  run_t.inputDeepSleep_times =2;
	   }
	   else{
          run_t.inputDeepSleep_times =0;

	   }
	   
		run_t.backlight_run_flag=1;
	break;

     default:

     break;

	}

	
 }
/****************************************************************************
	*
	*Function Name:static void Works_IndicateLed(void)
	*Function : works indicate led of state
	*Input Ref: NO
	*Retrun Ref:NO
	*
****************************************************************************/
static void Works_IndicateLed(void)
{
	static uint16_t cnt0,cnt;
    static uint8_t cntrecoder;
	switch(run_t.works_led_lable){

		case works_ok_led_on:
			run_t.gTimer_8s =0;
			ERR_LED_OFF();
			OK_LED_OFF();
		    run_t.input_newPassword_over_number=0;
			run_t.gTimer_8s=0; //WT.EDIT 2022.10.14
			 //erase EEPRO data 
			 if(run_t.clearEeprom==1){
				 run_t.clearEeprom = 0;
				 run_t.gTimer_8s =0;
				 run_t.buzzer_sound_lable=sound_excute;//run_t.buzzer_longsound_flag =1 ;
				  ClearEEPRO_Data();
				  Del_FR();//fingerprint be deteleted
				   run_t.gTimer_8s =0;
			    
				  run_t.inputDeepSleep_times =0;//WT.EDIT 2022.10.26
				  run_t.gTimer_input_standby_cnt=0;

				   run_t. clearEeeprom_count=0;
		            Del_FR();//fingerprint be deteleted
		         run_t.gTimer_led_blink_500ms=0;
		        run_t.clearEeeprom_count=0;
                run_t.works_led_lable=works_ok_blink;
				 
			}
			else{
				if((FP_INPUT_KEY()==0 && syspara_t.PS_wakeup_flag==0) && syspara_t.ps_pre_detector==0)
				    PS_Green_Led_ON();

			}
		break;

		case works_ok_blink: //05
		     run_t.gTimer_8s=0;
		    if((FP_INPUT_KEY()==0 && syspara_t.PS_wakeup_flag==0) && syspara_t.ps_pre_detector==0){
			 if(cnt0==0){
			 	cnt0++;
			 
			 //  PS_Green_Led_OFF();
			   ERR_LED_OFF();
			 }
			
				 run_t.input_newPassword_over_number=0;
				 run_t.gTimer_8s=0; //WT.EDIT 2022.10.14
				 
				 if(run_t.gTimer_led_blink_500ms  < 3 ){

				 	
				    PS_Green_Led_ON();
					OK_LED_ON();

				 }
				 else if(run_t.gTimer_led_blink_500ms  > 2){//500.WT.EDIT 2022.10.31
				 	

					PS_Green_Led_OFF();
					OK_LED_OFF();
				 }

				 if(run_t.gTimer_led_blink_500ms > 4){ //1000.WT.EDIT 2022.10.31
				 	run_t.gTimer_led_blink_500ms=0;
				 	run_t.clearEeeprom_count++;
				 	if(run_t.inputNewPassword_Enable ==1)
				 		run_t.inputNewPwd_OK_led_blank_times++;
				 }
          
			 if(run_t.inputNewPassword_Enable ==1){//WT.EDIT 2022.10.08

			 	if(run_t.inputNewPwd_OK_led_blank_times >9){
			 	run_t.inputNewPwd_OK_led_blank_times=0;
				cnt0 = 0;
			 	run_t.inputNewPassword_Enable =0;
			 	run_t.works_led_lable= works_ok_led_off;//run_t.works_led_lable=works_null;
			
			 	}

			 }
             
       
			
			 if(run_t.clearEeeprom_count >2 &&  run_t.inputNewPassword_Enable ==0){
			 	run_t.clearEeeprom_count=0;
				cnt0 = 0;
                 run_t.works_led_lable= works_ok_led_off;
			 }
			}
		break;

		case works_ok_led_off:
			
             OK_LED_OFF();
			 ERR_LED_OFF();
		     PS_LED_ALL_OFF();
			 BACKLIGHT_OFF();

			  run_t.gTimer_8s=0;
			  run_t.inputNewPassword_Enable =0; //WT.EDIT 2022.09.28
		     run_t.Numbers_counter =0;
			 run_t.powerOn =3;
	
			syspara_t.PS_login_times=0;
			syspara_t.PS_wakeup_flag=0;
			run_t.Confirm_newPassword = 0;
			run_t.inputNewPassword_Enable=0;

			run_t.lowPower_flag=0;
			syspara_t.PS_login_times=0;	//fingerprint input times 
			run_t.works_led_lable= works_null;
			run_t.password_unlock_model=0;

		break;

		case works_error_led_on:
		    OK_LED_OFF();	
            ERR_LED_ON();
		    PS_Red_Led_ON();
			run_t.gTimer_8s=0;
			run_t.inputNewPassword_Enable =0;//WT.EDIT 2022.10.05
			run_t.password_unlock_model=0;
		
			run_t.Confirm_newPassword=0; //WT.EDIT .2022.10.07
			run_t.inputNewPasswordTimes =0;//WT.EDIT .2022.10.07
			HAL_Delay(500);
            run_t.works_led_lable= works_null;

        

		break;

		case works_error_led_off:
		    OK_LED_OFF();	
            ERR_LED_OFF();
		    PS_LED_ALL_OFF();
            run_t.works_led_lable= works_null;
		break;

		case works_error_blink:
			syspara_t.PS_read_template=0;
			if(cnt==0){
				cnt++;
			   PS_Red_Led_ON();

			}
			//  run_t.BackLight =0;//WT.EDIT .2022.10.19
			run_t.inputNewPassword_Enable =0;//WT.EDIT 2022.10.05
			
			run_t.password_unlock_model=0;
			

			run_t.Confirm_newPassword=0; //WT.EDIT .2022.10.07
			run_t.inputNewPasswordTimes =0;//WT.EDIT .2022.10.07

			OK_LED_OFF();


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
		
			if(cntrecoder > 2){
			   cntrecoder =0;

			run_t.saveEEPROM_fail_flag =0;
		
			run_t.input_newPassword_over_number=0;

			ERR_LED_OFF();
			PS_LED_ALL_OFF();
			cnt=0;
		    run_t.works_led_lable= works_null;

			}

		break;

		case factory_led_test:
			run_t.gTimer_8s=0;
			BACKLIGHT_ON();
			OK_LED_ON();
			ERR_LED_ON();
			BAT_LED_ON();

			if(run_t.gTimer_input_error_times_60s > 5){
			run_t.factory_test =0;
			run_t.gTimer_8s=10;
			BACKLIGHT_OFF();
			OK_LED_OFF();
			ERR_LED_OFF();
			BAT_LED_OFF();
            FP_POWER_OFF();
             run_t.works_led_lable= works_null;
			}
			

		break;

		case works_null:
            PS_LED_ALL_OFF();
			OK_LED_OFF();
		    ERR_LED_OFF();
			BACKLIGHT_OFF();
            run_t.works_led_lable= 0xff;
            run_t.backlight_Cmd_lable=backlight_led_off;
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
 	   
    
 	  if(run_t.inputDeepSleep_times > 1){
		run_t.backlight_run_flag=0;

		run_t.gTimer_10s=0;
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

    static unsigned char t0,t1;

	if(htim->Instance==TIM14){

	t0++;
    t1++;


	if(t1>9){ //10ms x10=100ms
        t1=0;
        run_t.motorRunCount++;
	    run_t.gTimer_led_blink_500ms++;
	}
    if(t0>99){ //10*100 =1000ms "1s"
       t0=0;
	  run_t.gTimer_10s_start++;
	  run_t.gTimer_8s++;
	  run_t.gTimer_10s ++;
	  run_t.gTimer_ADC ++;
	  
	  
	   if(run_t.gTimer_10s_start>9){ //10s
	   	 run_t.gTimer_10s_start=0;
	   	 run_t.gTimer_input_standby_cnt++;
		run_t.gTimer_input_error_times_60s++;
	   }
	
    } 
	}

}

