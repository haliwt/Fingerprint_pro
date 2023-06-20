#include "interrupt_manager.h"
#include "run.h"
#include "key.h"
#include "led.h"
#include "delay.h"
#include "tim.h"
#include "buzzer.h"
#include "usart.h"
#include "cmd_link.h"

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
    

	if(GPIO_Pin == SIDE_KEY_INPUT_Pin){


	// __HAL_GPIO_EXTI_CLEAR_FALLING_IT(SIDE_KEY_INPUT_Pin); // 清除下降沿触发的中断标志位 // __HAL_GPIO_EXTI_CLEAR_IT(SIDE_KEY_INPUT_Pin);
	
		if(run_t.lowPower_flag==0){
			
			SystemClock_Config();
			__HAL_RCC_PWR_CLK_ENABLE();
			HAL_ResumeTick();
            MX_GPIO_Init();
			HAL_TIM_Base_Start_IT(&htim14);//
            run_t.inputDeepSleep_times =0; 

			POWER_ON();
			FP_POWER_ON()  ;
			 run_t.gTimer_8s=0;
			run_t.lowPower_flag++;
			 run_t.keyPressed_flag =1;	
			
		}
		else{
		      POWER_ON();
			  FP_POWER_ON()  ;
			   run_t.keyPressed_flag =1;
		    run_t.gTimer_8s=0;
				 #if DEBUG

			 printf("side_key\n");

			 #endif 


		}
	   	

	}
}

void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin)
{



   if(GPIO_Pin == SC12B_INT_INPUT_Pin){
   
    

     
      
      if(run_t.lowPower_flag==0){
			SystemClock_Config();
			__HAL_RCC_PWR_CLK_ENABLE();
			HAL_ResumeTick();
             MX_GPIO_Init();
			HAL_TIM_Base_Start_IT(&htim14);//


            POWER_ON();
			FP_POWER_ON()  ;
			
			run_t.gTimer_8s=0;
			run_t.inputDeepSleep_times =0;
					   
			run_t.lowPower_flag++;
		}
	    else{
		   
	       POWER_ON();
	       FP_POWER_ON()  ;
		   run_t.gTimer_8s=0;//WT.EDIT 2022.10.08
           run_t.keyPressed_flag =1;
		   	 #if DEBUG

			 printf("pwd_key\n");

			 #endif 
		   
	      }

 	}
  
   //fingerprint 
  if(GPIO_Pin==FP_INT_INPUT_Pin){
  	
 
		 __HAL_GPIO_EXTI_CLEAR_RISING_IT(FP_INT_INPUT_Pin); //__HAL_GPIO_EXTI_CLEAR_IT(FP_INT_INPUT_Pin);//WT.EDIT 2022.09.09
    

	 if(run_t.lowPower_flag==0){
		SystemClock_Config();
		__HAL_RCC_PWR_CLK_ENABLE();
		HAL_ResumeTick();
         MX_GPIO_Init();
		HAL_TIM_Base_Start_IT(&htim14);//

		POWER_ON();
		FP_POWER_ON()  ;
		BACKLIGHT_ON();
		run_t.gTimer_8s=0;//WT.EDIT 2022.10.08
      
		run_t.lowPower_flag++;
	   
	    run_t.gTimer_8s=0;
		run_t.inputDeepSleep_times =0;

	      
      }
	  else{
	  	     run_t.gTimer_8s=0;//WT.EDIT 2022.10.08
			 POWER_ON();
			 FP_POWER_ON()  ;
			 BACKLIGHT_ON();
			 #if DEBUG

			 printf("fp_key\n");

			 #endif 
			
			  
		}


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

    static unsigned char t0;

	if(htim->Instance==TIM14){

	   t0++;
      if(t0>99){ //10*100 =1000ms "1s"
		t0=0;
        run_t.gTimer_8s++;
        run_t.gTimer_ADC ++;
        run_t.gTimer_input_error_times_60s++;
		run_t.inputDeepSleep_times++;
		run_t.gTimer_total_backling_off++;
		run_t.gTimer_usart_error ++;
		
	    

	
	}

    if(htim->Instance==TIM17){
		if (__HAL_TIM_GET_FLAG(&htim17, TIM_FLAG_UPDATE) != RESET)//是更新中断
		{	 			   
			USART1_RX_STA|=1<<15;	//标记接收完成
			__HAL_TIM_CLEAR_FLAG(&htim17, TIM_FLAG_UPDATE);  //清除TIM17更新中断标志    
			HAL_TIM_Base_Stop_IT(&htim17);  //关闭TIM17 
		}	    

    }
}

}

