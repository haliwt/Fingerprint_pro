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
#include "motor.h"


key_types key;
uint8_t buzzertimes;

void  (*ReadDigital_Key_Numbers_Handler)(void);
void (*Input_NewPwd_Digtial_Handler)(uint16_t dat);


__IO uint16_t  KeyValue;
__IO uint8_t read_digital_key;


uint8_t virtualPwd[20];

uint8_t read_key_value;
uint8_t key_up =1 ;


static void ReadDigital_Inputkey_Fun(void);
static void Input_NewPwd_Digital_Fun(uint16_t dat);


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
				if(++key.on_time>20 && ++key.on_time < 800) //1000  0.5us -> short time key
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
     
	 if(keyvalue == 0x01){ //short times key that set up new passwords 
               
		run_t.Confirm_newPassword = 1; //set up new passwords of flag:1->enable ,0->disable
		run_t.inputDeepSleep_times =0;
		run_t.inputNewPassword_Enable =0;
		run_t.input_digital_key_number_counter =0 ;
		run_t.gTimer_8s=0;

		run_t.buzzer_sound_tag = key_sound;
		run_t.backlight_label =BACKLIGHT_ON; 

   
        run_t.confirm_button_flag=confirm_button_donot_pressed;

		run_t.panel_lock =0;
		run_t.thefirst_side_key=0;
		run_t.keyPressed_flag =1;
	
        /********************************************/     
		
		
		POWER_ON();//WT.EDIT .2022.10.06
		BACKLIGHT_ON();   
		OK_LED_OFF();//WT.EDIT .2022.10.31
		ERR_LED_OFF();
		
	 
       }
	 
      if(keyvalue== 0x81){ //long times key that earse EEPROM password
        run_t.keyPressed_flag =1;
        run_t.clearEeprom = 1;
		run_t.clearEeeprom_done = 0;
		run_t.inputDeepSleep_times =0;
        run_t.gTimer_8s=0;
		run_t.confirm_button_flag=confirm_button_clearn_eeprom;
		
       }

}

/****************************************************************************
*
*Function Name:void RunCheck_Mode(unsigned int dat)
*Function : run is main 
*Input Ref: NO
*Retrun Ref:NO
*
****************************************************************************/
void RunCheck_Mode(uint16_t dat)
{
   uint8_t  i;
  
   static uint8_t k0=0xff,k1=0xff,k2=0xff,key;
 

    switch(dat){

	  

	case SPECIAL_1 ://0x40: //CIN1->'*' clear data KEY 
		
      	if(k0 != run_t.getSpecial_1_key){
			k0 = run_t.getSpecial_1_key;

			run_t.getSpecial_2_key++;//n1++;
			run_t.getNumbers_key++;//n2++;
			run_t.keyPressed_flag =1; // has a input key be pressing 
			key=0;
            //backlight
            POWER_ON();
            OK_LED_OFF();
			ERR_LED_OFF();
			run_t.backlight_label =BACKLIGHT_ON;
			run_t.buzzer_sound_tag = key_sound;
            run_t.gTimer_8s=0;  //LED turn on holde times

			POWER_ON();
            switch(run_t.inputNewPassword_Enable){

			  case 1:
				run_t.clear_inputNumbers_newpassword ++;
				run_t.gTimer_8s=0;
			    switch(run_t.clear_inputNumbers_newpassword){

				   case 1:
					    //the first clear input digital key of data by input new password enable
						run_t.gTimer_8s=0;

						for(i=0;i<6;i++){
							pwd2[i]=0;
							pwd1[i]=0;
						}
						run_t.inputNewPasswordTimes =0; 
						run_t.input_digital_key_number_counter =0;
                    
					   //other ref
						run_t.gTimer_8s=0;
						run_t.confirm_button_flag=confirm_button_donot_pressed;//confirm_button_save_new_password;

				
				break;

				   case 2:  //continue input "two" times -> exit input new password 
					    //the second times cancel input new password action.

						run_t.clear_inputNumbers_newpassword=0;
						run_t.inputNewPassword_Enable=0; //cancel input input new password disable
						run_t.Confirm_newPassword=0;  //Clear set up input new password KEY
						run_t.inputNewPasswordTimes =0;
						run_t.keyPressed_flag =0;
					
						//input key flag
						run_t.input_digital_key_number_counter =0;

						run_t.password_unlock=UNLOCK_NULL;
						run_t.confirm_button_flag=confirm_button_donot_pressed;

						run_t.backlight_label = BACKLIGHT_OFF;

						run_t.inputNewPassword_Enable=0;

						for(i=0;i<6;i++){
							pwd2[i]=0;
							pwd1[i]=0;
						
						}

					break;

					
			   }
       
		break;

		case 0:
	      for(i=0;i<6;i++){
		  	   pwd1[i]=0;
			   Readpwd[i]=0;
		
		  }
		
         break;
        }
			
	    ERR_LED_OFF();
		run_t.gTimer_8s=0;
		run_t.input_digital_key_number_counter =0 ;
		run_t.confirm_button_flag = 0;
		run_t.inputDeepSleep_times =0;
	
		    
	   }
	  
       break;

	

	 case SPECIAL_2://0x200: //CIN10 '#' ->confirm KEY
         if(k1 != run_t.getSpecial_2_key){
	         k1 = run_t.getSpecial_2_key;

             run_t.getSpecial_1_key++;//n1++
		     run_t.getNumbers_key++;//n2++;
		     run_t.keyPressed_flag =1;//has a input key be pressing 
		     key=0;
			
			run_t.gTimer_8s=0;
			
			POWER_ON();
			OK_LED_OFF();

			run_t.backlight_label =BACKLIGHT_ON;
			

		    if(run_t.input_digital_key_number_counter ==0){
			
				run_t.confirm_button_flag = confirm_button_donot_pressed;
				run_t.buzzer_sound_tag = key_sound;
				
				run_t.gTimer_8s=0;
			}
		    else if(run_t.input_digital_key_number_counter < 4 ){//error input key times be msut greater than or equal to  4  

                run_t.input_digital_key_number_counter=0;
				run_t.Confirm_newPassword =0;
				run_t.inputNewPassword_Enable =0;
			
                run_t.confirm_button_flag = confirm_button_donot_pressed;
               
           
               // run_t.fail_sound_flag=1;
			     run_t.buzzer_sound_tag = fail_sound;
			     run_t.backlight_label =BACKLIGHT_ERROR_BLINK;
			     run_t.error_times ++ ;
				
                if(run_t.error_times > 4 ){ //OVER 5 error  times auto lock touchkey 60 s
	            
	                run_t.confirm_button_flag= confirm_button_error_times; //run_t.panel_lock=1;
				
				  }

		   }
		   else{ //input key equal to 4 or bigger than  4
				if(run_t.inputNewPassword_Enable ==1){ //prepare input newpassword .WT.EDI 2022.10.13//if( run_t.Confirm_newPassword ==1){

		            run_t.inputNewPasswordTimes ++ ;  //recoder times
		            run_t.gTimer_8s=0;
			        switch(run_t.inputNewPasswordTimes){

					case 1:
					
					//Confirm Key "#"
					run_t.input_digital_key_number_counter =0 ;
					//run_t.buzzer_two_short = 2;
					run_t.buzzer_sound_tag = two_short_two_sound;
				
                    run_t.input_digital_key_number_counter=0;
					run_t.confirm_button_flag=confirm_button_donot_pressed;
					run_t.inputDeepSleep_times =0;
					run_t.gTimer_8s=0;
					
					break;

					case 2:
					//Confirm Key "#"
				
                   // run_t.input_digital_key_number_counter=0;
			
					run_t.confirm_button_flag=confirm_button_save_new_password;
				    //run_t.new_pwd_save_data_tag = NEW_PWD_SAVE_DATA_TO_EEPROM;
					run_t.inputDeepSleep_times =0;
					run_t.gTimer_8s=0;


					break;
			        }
					
						
			    }
				else if(run_t.motor_doing_flag==motor_null){ // return home position
					    //sound by run process adjust
						run_t.inputNewPasswordTimes=0; 
					   // run_t.input_digital_key_number_counter=0;//if is virtual more than 7 number
						run_t.inputDeepSleep_times =0;
				        run_t.confirm_button_flag = confirm_button_pressed;
					
				}
				else if(run_t.motor_doing_flag !=motor_null){ //motor runing ->repeat itself motor doing run
						//run_t.input_digital_key_number_counter=0;//if is virtual more than 7 number
						run_t.inputNewPasswordTimes=0;
				       run_t.oneself_copy_behavior=1;
					   run_t.inputDeepSleep_times =0;
					  
					   run_t.confirm_button_flag = confirm_button_pressed; //run next step process
					   
				 }
				 run_t.gTimer_8s=0;
			}
		   
		}
	 
      break;

	 
	case KEY_0:
		
		key=1;
	    run_t.getNumbers_key++;
		run_t.keyPressed_flag =1;
		
             
	break;

    case KEY_1 :

		key=1;
		run_t.getNumbers_key++;
		run_t.keyPressed_flag =1;
		
	break;
			
    case KEY_2:
		key=1;
	    run_t.getNumbers_key++;
	    run_t.keyPressed_flag =1;
		
	 
	break;
			
	case  KEY_3:
		key=1;
	    run_t.getNumbers_key++;
		run_t.keyPressed_flag =1;
		
	
    break;
			
	case KEY_4:
		key=1;
	    run_t.getNumbers_key++;
		run_t.keyPressed_flag =1;
	
	break;
			
	case KEY_5:
		key=1;
	    run_t.getNumbers_key++;
		run_t.keyPressed_flag =1;
		
			
    break;
			
	case KEY_6:
		
		key=1;
	    run_t.getNumbers_key++;
		run_t.keyPressed_flag =1;
		
    break;
	
	case KEY_7:
		key=1;
	    run_t.getNumbers_key++;
		run_t.keyPressed_flag =1;
		
	break;
			
	case KEY_8:
		key=1;
		run_t.getNumbers_key++;
		run_t.keyPressed_flag =1;
	
	break;

	case KEY_9:
		key=1;
	    run_t.getNumbers_key++;
		run_t.keyPressed_flag =1;
		
	break;
		  

	}  

	if(k2 != run_t.getNumbers_key && key==1 ){
				
		k2=run_t.getNumbers_key;
		key = 0;
		run_t.getSpecial_1_key++;//n1++

		run_t.getSpecial_2_key++;//n1++;

		POWER_ON();
		run_t.backlight_label =BACKLIGHT_ON;
	
		run_t.input_digital_key_number_counter ++ ;
		run_t.buzzer_sound_tag = key_sound;

		run_t.gTimer_8s=0;

		run_t.confirm_button_flag =confirm_button_donot_pressed;
		POWER_ON();
        switch(run_t.Confirm_newPassword){

			case 1:
				Input_NewPwd_Digtial_Handler((TouchKey_Numbers)dat);
			break;

	    case 0:

		     if(run_t.inputNewPassword_Enable ==1){

				Input_NewPwd_Digtial_Handler((TouchKey_Numbers)dat);

			}
			else{
				read_digital_key = InputNumber_ToSpecialNumbers((TouchKey_Numbers)dat); //input Numbers
				//virtual password is 20bit
				 //virtual input key numbers 
				if(run_t.input_digital_key_number_counter < 21){
				//if(run_t.input_digital_key_number_counter > 20)run_t.input_digital_key_number_counter =20;
				
				    virtualPwd[run_t.input_digital_key_number_counter-1]=read_digital_key;
			        

				ReadDigital_Key_Numbers_Handler();

		   		}
		   }
		   break;
        }
		run_t.gTimer_8s=0;
		
	}
}

/********************************************************************
	*
	*Function Name: static void Input_NewPwd_Digital_Fun(void)
	*Function : control input new password of digital number 
	*			don't over seve numbers
	*
	*
	*
********************************************************************/
static void Input_NewPwd_Digital_Fun(uint16_t dat)
{
	if(run_t.input_digital_key_number_counter >6){//WT.EDIT 2022.10.08
		run_t.gTimer_8s=0;
		run_t.confirm_button_flag=confirm_button_over_numbers;
		
		

	}
	else{
		read_digital_key = InputNumber_ToSpecialNumbers((TouchKey_Numbers)dat); 
		ReadDigital_Key_Numbers_Handler();


	}

}
/********************************************************************
	*
	*Function Name: static void ReadDigital_Inputkey_Fun(void)
	*Function : read touch key input digital numbers
	*
	*
	*
********************************************************************/
static void ReadDigital_Inputkey_Fun(void)
{
    uint8_t read_numbers;
   
	
	if(run_t.input_digital_key_number_counter < 7){//run_t.inputNewPasswordTimes

		if(run_t.inputNewPasswordTimes ==0 && run_t.inputNewPassword_Enable ==1){//WT.EDIT 2022.10.14
            read_numbers = OverNumbers_Password_Handler();
            if(read_numbers==1){
	            run_t.confirm_button_flag= confirm_button_over_numbers; //over times ten group numbers password
	            run_t.input_digital_key_number_counter =0;
            }
            else{
                run_t.new_password_the_first_numbers = run_t.input_digital_key_number_counter;
                pwd2[run_t.input_digital_key_number_counter-1]=read_digital_key; //the first input new password .
            }
          }
		else 
            pwd1[run_t.input_digital_key_number_counter-1] =read_digital_key;

	}

}

/*******************************************************
 * 
 * Function Name:void TouchKey_Handler(void)
 * Function: start power on handler
 * INPUT Ref:NO
 * Return Ref:NO
 * 
********************************************************/
void TouchKey_Handler(void)
{
  
	 
	 if(I2C_Read_From_Device(SC12B_ADDR,0x08,SC_Data,2)==DONE){

	        read_key_value=Read_SC12B_KEY();
	       KeyValue =(uint16_t)(SC_Data[0]<<8) + SC_Data[1];
		   HAL_Delay(10);
	       while(key_up==1 &&  KeyValue !=0){
		   	    key_up=0;
		   		RunCheck_Mode(KeyValue); 

	       }
		   
	       if( KeyValue ==0){
		   	    key_up=1;
	            run_t.getSpecial_1_key++;
	            run_t.getSpecial_2_key++;
				run_t.getNumbers_key++;
	             

           }
	     
			  
	 
	 
	}
}




/*******************************************************************
* Funtion of pointer 
*******************************************************************/
void ReadInput_KeyNumber_Handler(void(*read_digital_key_handler)(void))
{
	ReadDigital_Key_Numbers_Handler = read_digital_key_handler;

}

void Input_NewPwd_Digital_Handler(void(*input_new_digital_handler)(uint16_t data))
{

	Input_NewPwd_Digtial_Handler = input_new_digital_handler;


}


