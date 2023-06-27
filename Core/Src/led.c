#include "led.h"
#include "run.h"
#include "gpio.h"
#include "buzzer.h"
#include "at24c02.h"
#include "adc.h"
#include "touchkey.h"
#include "tim.h"
#include "as608.h"
#include "fingerprint.h"
#include "single_mode.h"

static void BackLight_Fun(void);

static void Works_Fingerprint_IndicateLed(void);
static void PWR_Enter_Stop(void);



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
	Works_Fingerprint_IndicateLed();

	if(run_t.panel_lock ==1){
		      Panel_LED_Off();
         	  BACKLIGHT_OFF();
              ERR_LED_ON();
         	 
         	 run_t.inputDeepSleep_times =0;
         	  run_t.gTimer_input_standby_cnt=0;
         	  run_t.gTimer_8s=0;
             if(run_t.gTimer_input_error_times_60s > 60){
			   run_t.panel_lock =0;
			   run_t.error_times = 0;
               run_t.works_led_lable= works_error_led_off;
	      }
     }

	if(run_t.factory_test ==1){
			run_t.gTimer_8s=0;
			BACKLIGHT_ON();
			OK_LED_ON();
			ERR_LED_ON();
			BAT_LED_ON();

			if(run_t.gTimer_factory_times_60s > 60){
			run_t.factory_test =0;
			run_t.gTimer_8s=10;
			BACKLIGHT_OFF();
			OK_LED_OFF();
			ERR_LED_OFF();
			BAT_LED_OFF();
            FP_POWER_OFF();
             run_t.works_led_lable= works_null;
			}
		

	}




	
	if(run_t.gTimer_ADC >10 ){
   	    uint16_t adcx,adcVale;
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
     
     case backlight_led_on: //9
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

		}
		
		run_t.backlight_run_flag=1;
     break;

     case backlight_led_off: //10

	   if(run_t.gTimer_8s > 8){
         
        OK_LED_OFF();
		ERR_LED_OFF();
		PS_LED_ALL_OFF();
	    BACKLIGHT_OFF();
        run_t.touchkey_save_data_flag = 0;
       run_t.gTimer_8s=0;
	  // syspara_t.handler_read_data_flag++;
	   syspara_t.PS_login_times=0;
	 
	   run_t.Confirm_newPassword = 0;
   
      
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
		run_t.touchkey_first ++; //WT.EDIT 2022.10.19 ->touch key delay times 

		for(i=0;i<6;i++){ //WT.EDIT .2022.08.13
		*(pwd2 + i)=0;//pwd2[i]=0;
		*(pwd1+i)=0;//pwd1[i]=0;

		}

		run_t.gTimer_input_standby_cnt=0;
        run_t.backlight_Cmd_lable = backlight_led_confirm;
		run_t.backlight_run_flag=1;
	   	}
     break;


	case backlight_led_confirm:

	      if(run_t.gTimer_8s >8 ){
		  	
		    run_t.backlight_Cmd_lable = standby_led;

		 }
		  else{
             run_t.gTimer_10s_start=0;
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
static void Works_Fingerprint_IndicateLed(void)
{
	uint8_t i;
    static uint16_t cnt0,cnt;
    static uint8_t cntrecoder;
	if(run_t.factory_test ==0){
	switch(run_t.works_led_lable){

		case works_ok_led_on:
		
			ERR_LED_OFF();
			OK_LED_OFF();
			PS_Blue_Led_ON();
            BACKLIGHT_OFF();
		    run_t.input_newPassword_over_number=0;
			
            if(run_t.gTimer_8s > 8)
			      run_t.works_led_lable= works_ok_led_off;
			
		break;

		case works_led_ok_blink: //05
		     run_t.gTimer_8s=0;
		   
			 if(cnt0==0){
			 	cnt0++;
			    BACKLIGHT_OFF();
			   PS_Green_Led_OFF();
			   ERR_LED_OFF();
			  }
			
				 run_t.input_newPassword_over_number=0;
				 run_t.gTimer_8s=0; //WT.EDIT 2022.10.14
				 
				 if(run_t.gTimer_led_blink_500ms < 6 ){

				 	
				    PS_Green_Led_ON();
					OK_LED_ON();

				 }
				 else if(run_t.gTimer_led_blink_500ms > 5 &&  run_t.gTimer_led_blink_500ms < 11){//500.WT.EDIT 2022.10.31
				 	

					PS_Green_Led_OFF();
					OK_LED_OFF();
                     
				}
                else { //1000.WT.EDIT 2022.10.31
				 	
				 	run_t.gTimer_led_blink_500ms=0;
				 	if(run_t.inputNewPassword_Enable ==1)
				 	       run_t.inputNewPwd_OK_led_blank_times++;
					if(run_t.clearEeprom == 1)
                          run_t.clearEeeprom_count++;
					
				 }
          
			 if(run_t.inputNewPassword_Enable ==1){//WT.EDIT 2022.10.08

			 	if(run_t.inputNewPwd_OK_led_blank_times >10){
			 	run_t.inputNewPwd_OK_led_blank_times=0;
				cnt0 = 0;
			 	run_t.inputNewPassword_Enable =0;
			 	run_t.works_led_lable= works_ok_led_off;//run_t.works_led_lable=works_null;
			
			 	}

			 }
             if(run_t.clearEeeprom_count >2 &&  run_t.inputNewPassword_Enable ==0){
			 	run_t.clearEeeprom_count=0;
				cnt0 = 0;
                run_t.gTimer_8s =0;
				run_t.clearEeprom=0;
                run_t.works_led_lable= works_ok_led_off;
			 }
			
		break;

		case works_ok_led_off:
			 
	             OK_LED_OFF();
				 ERR_LED_OFF();
			     PS_LED_ALL_OFF();
				 BACKLIGHT_OFF();
			     run_t.works_led_lable= works_null;
				 run_t.password_unlock_model=0;
				  run_t.backlight_Cmd_lable=backlight_led_off;
				  run_t.Confirm_newPassword =0;
					for(i=0;i<6;i++){
						  pwd2[i]=0;
						  pwd1[i]=0;
					}
             

		break;

		case works_error_led_on:
			if(run_t.gTimer_8s > 8){
			    OK_LED_OFF();	
	            ERR_LED_ON();
			    PS_Red_Led_ON();
                BACKLIGHT_OFF();
				run_t.gTimer_8s=0;
				run_t.inputNewPassword_Enable =0;//WT.EDIT 2022.10.05
				run_t.password_unlock_model=0;
			
				run_t.Confirm_newPassword=0; //WT.EDIT .2022.10.07
				run_t.inputNewPasswordTimes =0;//WT.EDIT .2022.10.07
				
	            run_t.works_led_lable= works_null;
	            run_t.backlight_Cmd_lable=backlight_led_off;
			}

		break;

		case works_error_led_off:
            if(run_t.gTimer_8s>8){
                OK_LED_OFF();	
                ERR_LED_OFF();
                PS_LED_ALL_OFF();
                BACKLIGHT_OFF();
                run_t.works_led_lable= works_null;
				run_t.backlight_Cmd_lable=backlight_led_off;
            }
		break;

		case works_led_error_blink: //6
          
			syspara_t.PS_read_template=0;
			if(cnt==0){
				cnt++;
			    PS_Red_Led_ON();
                BACKLIGHT_OFF();
                OK_LED_OFF();

			}
			//  run_t.BackLight =0;//WT.EDIT .2022.10.19
			run_t.inputNewPassword_Enable =0;//WT.EDIT 2022.10.05
			run_t.Confirm_newPassword=0; //WT.EDIT .2022.10.07
			run_t.inputNewPasswordTimes =0;//WT.EDIT .2022.10.07

			
           if(run_t.gTimer_led_blink_500ms < 6 ){

			ERR_LED_OFF();
			PS_Red_Led_OFF();

			}
			else if(run_t.gTimer_led_blink_500ms > 5 &&  run_t.gTimer_led_blink_500ms < 11){
			ERR_LED_ON();
			 PS_Red_Led_ON();
			}
			else if(run_t.gTimer_led_blink_500ms > 10){
                cntrecoder++;
                run_t.gTimer_led_blink_500ms=0;

			}
		    
			if(cntrecoder > 2){
			   cntrecoder =0;

                run_t.saveEEPROM_fail_flag =0;
            
                run_t.input_newPassword_over_number=0;

                ERR_LED_OFF();
                PS_LED_ALL_OFF();
                BACKLIGHT_OFF();
                cnt=0;
                run_t.works_led_lable= works_null;
                run_t.backlight_Cmd_lable=backlight_led_off;
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
		run_t.touchkey_first =0; //WT.EDIT 2022.09.26
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
*Function Name:void BackLight_Fun(void)
*Function : run is main 
*Input Ref: NO
*Retrun Ref:NO
*
****************************************************************************/
void BackLight_Control_Handler(void)
{
    uint8_t i;
	static uint8_t err_counter,new_pwd_counter,confirm_ok_counter;
	static uint16_t err_cnt,ok_cnt,new_counter;
	//back light turn on or turn off function

	 

	  switch(run_t.backlight_label){
   
          case BACKLIGHT_ON:
             BACKLIGHT_ON();

		    	 new_counter = 0;
                 new_pwd_counter=0;
			     err_cnt = 0;
			    err_counter=0;
				
				ok_cnt	= 0;
				confirm_ok_counter=0; 
	
			if(run_t.gTimer_8s > 8){
                run_t.gTimer_8s=4;
				run_t.inputDeepSleep_times=0;
			    run_t.keyPressed_flag =0;
				run_t.backlight_label =BACKLIGHT_AT_ONCE_OFF;
				run_t.Confirm_newPassword=0;
			}
          break;

		  case BACKLIGHT_OFF: //0X00
		  
	        // BACKLIGHT_OFF();
			  new_counter = 0;
              new_pwd_counter=0;
			  err_cnt = 0;
			  err_counter=0;
			  ok_cnt	= 0;
			  confirm_ok_counter=0; 
			run_t.gTimer_total_backling_off=0;
		    if(run_t.gTimer_8s > 8){
	             run_t.keyPressed_flag =0;
				 run_t.inputDeepSleep_times=0;
				 run_t.backlight_label =BACKLIGHT_AT_ONCE_OFF;
			}

		
          break;

		 case BACKLIGHT_AT_ONCE_OFF://2

          if(run_t.keyPressed_flag ==1){
				BACKLIGHT_ON();
              
				run_t.gTimer_8s=0;
                run_t.backlight_label = BACKLIGHT_ON;
		  }
		  else{
			 if(run_t.gTimer_8s > 8){
			 	 new_counter = 0;
                 new_pwd_counter=0;
			     err_cnt = 0;
			    err_counter=0;
				
				ok_cnt	= 0;
				confirm_ok_counter=0; 
				Panel_LED_Off();
				run_t.gTimer_total_backling_off=0;
            	run_t.backlight_label =BACKLIGHT_INPUT_STOP_MODEL;
			 }
		   }
		break;

		case BACKLIGHT_OK_BLINK://3
         ok_cnt ++ ;
		 BACKLIGHT_ON();
		 ERR_LED_OFF();
		 if(ok_cnt  < 61 ){
	
			 OK_LED_OFF();
			  
		  }
		  else if(ok_cnt >59 && ok_cnt  < 122){//500.WT.EDIT 2022.10.31
			  OK_LED_ON();
		  }
	
		  if(ok_cnt >120){ //1000.WT.EDIT 2022.10.31
		  	ok_cnt  = 0;
			confirm_ok_counter++;
           
		   }
		  
		  if(confirm_ok_counter > 2){
			  ok_cnt  = 0;
			  confirm_ok_counter=0; 

			  new_counter = 0;
              new_pwd_counter=0;

			  new_counter = 0;
              new_pwd_counter=0;

			  run_t.gTimer_8s=10;

			run_t.backlight_label =BACKLIGHT_AT_ONCE_OFF;


		  }

		 if(run_t.motor_doing_flag==1) OK_LED_ON();
		 break;

		case BACKLIGHT_ERROR_BLINK://4
		   err_cnt ++ ;
		    BACKLIGHT_ON();
		
		    OK_LED_OFF();
	
	  
		  if(err_cnt < 61 ){
	
			  ERR_LED_OFF();
			  
		  }
		  else if(err_cnt > 59 && err_cnt < 122){
			  ERR_LED_ON();
		  }
		  if(err_cnt>120){
		  	err_cnt = 0;
			err_counter++;
			
		  }
		  
		  if(err_counter > 2){
		  	err_cnt = 0;
			err_counter=0;
		    run_t.backlight_label =BACKLIGHT_OFF;
		 
		  }
	
		break;

		case BACKLIGHT_NEW_PASSWORD_LED: //5//new input Pwd
		   new_counter ++ ;
		   BACKLIGHT_ON();
		   ERR_LED_OFF();
		   if(new_counter < 101 ){
	
			  OK_LED_OFF();
			  
		  }
		  else if(new_counter>99 && new_counter < 202){//500.WT.EDIT 2022.10.31
			  OK_LED_ON();
		  }
	
		  if(new_counter>200){ //1000.WT.EDIT 2022.10.31
		  	new_counter = 0;
            new_pwd_counter++;
		  }
		  
		    
		    
		if(new_pwd_counter>9){

		    new_counter = 0;
            new_pwd_counter=0;
					
			run_t.inputNewPassword_Enable =0;
			run_t.backlight_label =BACKLIGHT_AT_ONCE_OFF;
					 
		}
                  
	   break;

		case BACKLIGHT_ERROR_OVER_INPUT_TIMES://6
			    run_t.keyPressed_flag =0;
		        BACKLIGHT_OFF();
				ERR_LED_OFF();
		        if(run_t.panel_lock==0){
                  run_t.backlight_label =BACKLIGHT_AT_ONCE_OFF;
		        }
		break;


		case BACKLIGHT_FACTORY_LED://7
		  BACKLIGHT_ON();
		  OK_LED_ON();
		  ERR_LED_ON();
		  BAT_LED_ON();
	
		if(run_t.gTimer_input_error_times_60s > 59){
			run_t.factory_test =0;
			run_t.gTimer_8s=10;
			  BACKLIGHT_OFF();
			  OK_LED_OFF();
			  ERR_LED_OFF();
			  BAT_LED_OFF();
	          run_t.backlight_label =BACKLIGHT_AT_ONCE_OFF;
		}
	
	    break;

		case BACKLIGHT_INPUT_STOP_MODEL: //8
		

            run_t.confirm_button_flag =0 ;
			run_t.powerOn =3;

			run_t.keyPressed_flag=0;
			run_t.password_unlock=0;
		    

			//clear new password flag
			run_t.inputNewPassword_Enable =0; //WT.EDIT 2022.09.28
			run_t.input_digital_key_number_counter =0;
			run_t.clear_inputNumbers_newpassword=0;
			
			//wake up touch key
		

	
			
			for(i=0;i<6;i++){ //WT.EDIT .2022.08.13
					*(pwd2 + i)=0;//pwd2[i]=0;
					*(Readpwd+i)=0;
					*(pwd1+i)=0;//pwd1[i]=0;

			}
			  POWER_OFF();
			  Panel_LED_Off();
			  HAL_ADC_Stop(&hadc1);
		     

              if(run_t.inputDeepSleep_times > 19){  //wait 30s  
			   run_t.inputDeepSleep_times =0;
               
               PWR_Enter_Stop();
			   
		  }
		  break;
		}
		//ClearEEPROM_Data_Fun();
	    ADC_Detected_LowVoltage();
	    if(run_t.gTimer_total_backling_off > 180){
			run_t.gTimer_total_backling_off=0;
		  	Panel_LED_Off();

	     }
      
}


/****************************************************************
*
*Function Name:void DisplayLed_Handler(void)
*Function : display back light and buzzer sound 
*Input Ref:
*Return Ref:
*
****************************************************************/
void ClearEEPROM_Data_Fun(void)
{
      uint8_t i;
	if(run_t.clearEeprom==1){
			run_t.clearEeprom = 0;
			run_t.gTimer_8s =0;
            
			ClearEEPRO_Data();
			run_t.gTimer_8s =0;
			
			run_t.clearEeeprom_done = 1;
			run_t.inputDeepSleep_times =0;//WT.EDIT 2022.10.26
			
			run_t.buzzer_sound_tag = confirm_sound;
		
			run_t. clearEeeprom_count=0;
			//back light led control
			
			run_t.backlight_label = BACKLIGHT_OK_BLINK; //WT.EDIT 2023.03.27
			run_t.confirm_button_flag=confirm_button_donot_pressed;
			run_t.input_digital_key_number_counter=0;
			run_t.Confirm_newPassword = 0;
	        run_t.eepromAddress=0;
			 for(i=0;i<6;i++){
		  	   pwd1[i]=0;
			   Readpwd[i]=0;
		
		  	}
			 
		  }
      

}

/******************************************************************************
	*
	*Function Name:void Panel_Lock_Handler(void)
	*Function :
	*
	*
	*
*******************************************************************************/
void Panel_Lock_Handler(void)
{
	ERR_LED_ON(); 
	 if(run_t.gTimer_input_error_times_60s > 59){
			   run_t.panel_lock =0;
			   run_t.error_times = 0;
		       ERR_LED_OFF(); 
		       run_t.confirm_button_flag=confirm_button_donot_pressed;
 
	}
         
}


static void PWR_Enter_Stop(void)
{

	  #ifdef DEBUG_FLAG

			       GPIO_Standby_Init();

			   #endif 
          		/*close tick timer low power Mode */
		        #if DEBUG

				printf("inter stop\n");

				#endif 
			    run_t.lowPower_flag=0;
              //  __HAL_IWDG_START(__HANDLE__)
				HAL_SuspendTick();
				SysTick->CTRL = 0x00;//关闭定时器
                SysTick->VAL = 0x00;//清空val,清空定时器

				//   	__HAL_RCC_PWR_CLK_ENABLE();         //Ê¹ÄÜPWRÊ±ÖÓ
				
				/* input low power mode "STOP"*/
				//HAL_PWR_EnableWakeUpPin(SC12B_KEY_Pin);
				//Sys_Enter_Standby();
		        HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON,PWR_STOPENTRY_WFI);//WFI ->wait for interrupt

              //  SystemClock_Config();//Low power of low frequency 8MHz


}

