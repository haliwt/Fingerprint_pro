#include "run.h"
#include "at24c02.h"
#include "led.h"
#include "buzzer.h"
#include "motor.h"
#include "kmp.h"
#include "single_mode.h"
#include "lock.h"
#include "as608.h"




RUN_T run_t;

void (*Pwd_fun)(void);
void (*fp_fun)(void);


uint8_t touchkey_id[10];
uint8_t readFlag[1]={0};
uint8_t pwd1[6];
uint8_t pwd2[6];
//uint8_t origin_pwd[6]={1,2,3,4,0,0};
uint8_t virtualPwd[20];
uint8_t Readpwd[6];
uint8_t eevalue ;


void (*RunChed_KeyMode)(uint16_t keydat);




uint8_t touchkey_key_id[10];



/****************************************************************************
*
*Function Name:void Password_Modify(void)
*Function : run is main 
*Input Ref: NO
*Retrun Ref:NO
*
****************************************************************************/
unsigned char CompareValue(uint8_t *pt1,uint8_t *pt2)
{
	unsigned char i ;
   for(i=0;i<6;i++){
		if(*(pt1+i) != *(pt2+i)){
			return 0;
		}
		
	}
	return 1;
   
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
   uint8_t temp, i,read_numbers;
   static uint8_t numbers_key=0xff,special1= 0xff,special2=0xff,key_pressed;
   switch(dat){
	case SPECIAL_1 ://0x40: //CIN1->'*' ->cancel_key
		
       if(special1 != run_t.getSpecial_1_key){
           special1 = run_t.getSpecial_1_key;
      
		 run_t.getSpecial_2_key++;//n1++;
		 run_t.getNumbers_key++;//n2++;
		
	 
		  run_t.buzzer_sound_label = sound_key;//run_t.buzzer_key_sound_flag =1;
		  run_t.gTimer_8s=0;  //LED turn on holde times
		  
		  switch(run_t.cancel_key_label){

                case 0:
					if(run_t.inputNewPassword_Enable ==1){//WT.EDIT 2022.10.13

						run_t.clear_inputNumbers_newpassword ++;
						run_t.gTimer_8s=0;
						if(run_t.clear_inputNumbers_newpassword ==2){ //the second times cancel input new password action.

						run_t.clear_inputNumbers_newpassword=0;
						run_t.inputNewPassword_Enable=0;
						run_t.Confirm_newPassword=0;
						run_t.inputNewPasswordTimes =0;
						run_t.Numbers_counter =0 ;

						for(i=0;i<6;i++){
						pwd1[i]=0;
						Readpwd[i]=0;
					}

					OK_LED_OFF();
					ERR_LED_OFF();
					run_t.enter_key = KEY_NULL;
					return ;

					}
					if(run_t.clear_inputNumbers_newpassword ==1){//the first administrator password


						for(i=0;i<6;i++){
						pwd2[i]=0;
						pwd1[i]=0;
					}
					run_t.inputNewPasswordTimes =0; //WT.EDIT 2022.10.14
					run_t.Numbers_counter =0 ;

					//run_t.password_unlock_model=STORE_MODEL_EEPROM; //run_t.open_lock_success=STORE_MODEL;//3;
					run_t.Confirm_newPassword=1; //
					run_t.inputNewPwd_OK_led_blank_times=0;
					run_t.enter_key = KEY_NULL;
					return ;

					}

					}
					else
						run_t.cancel_key_label = 1;
					

		        break;
					
				case 1:
					for(i=0;i<6;i++){
						pwd2[i]=0;
						pwd1[i]=0;
					}
					run_t.cancel_key_label=0;
					run_t.enter_key = KEY_NULL;
					return ;
				break;

				default:
					
				break;


		  }
       	}
	   break;
		case SPECIAL_2://0x200: //CIN10 '#' ->confirm 


        if(special2 != run_t.getSpecial_2_key ){
	         special2 = run_t.getSpecial_2_key;

           run_t.getSpecial_1_key++;//n1++
		    run_t.getNumbers_key++;//n2++;
		    
			 
	       run_t.gTimer_8s=0;

		   switch(run_t.confirm_key_label){

               case 0 :
                if(run_t.Numbers_counter ==0){ //only has entery_key 

					run_t.buzzer_sound_label = sound_key;  
					run_t.pwd_fp_label = 
                    run_t.gTimer_8s=0;
				    run_t.enter_key = KEY_NULL;
					return ;
				 }

				
				 if(run_t.Numbers_counter < 4 && run_t.Numbers_counter >0){//error
				 
						run_t.Numbers_counter=0;
						run_t.error_times ++ ;
						if(run_t.error_times > 4 ){ //OVER 5 error  times auto lock touchkey 60 s
						run_t.gTimer_10s_start=0;//WT.EDIT 2022.09.20
						run_t.gTimer_input_error_times_60s =0;
						run_t.panel_lock=1;
						run_t.gTimer_60s=0;
						run_t.gTimer_8s=0;
						run_t.enter_key = KEY_LOCK_60S;
						run_t.buzzer_sound_label =sound_fail;
		                  
						return ;
					}
				 
				}


			   case 1:
			   	
				if(run_t.inputNewPassword_Enable ==1){ //prepare input newpassword .WT.EDI 2022.10.13//if( run_t.Confirm_newPassword ==1){

					run_t.inputNewPasswordTimes ++ ;  //recoder times

					if(run_t.inputNewPasswordTimes ==1){//Confirm Key "#"
				
						run_t.buzzer_sound_label = sound_new_pwd_the_second;//run_t. = 2;

					}

					run_t.enter_key =KEY_SUCCESS;
                    run_t.buzzer_sound_label =sound_key;
					run_t.Numbers_counter=0;
					run_t.gTimer_8s=0;


				}
				else if(run_t.motor_return_homePosition==0){ // return home position
					//run_t.buzzer_key_sound_flag =1; 

					run_t.enter_key =KEY_SUCCESS;
					run_t.buzzer_sound_label =sound_key;
					run_t.inputNewPasswordTimes=0; 
				    run_t.gTimer_8s=0;

				}
				else if(run_t.motor_return_homePosition==1){ //repeat itself motor doing run

					run_t.buzzer_sound_label =sound_key;//run_t.buzzer_key_sound_flag =1; 

					run_t.enter_key =KEY_SUCCESS;
				    run_t.buzzer_sound_label =sound_key;
					run_t.eepromAddress=0;

				}
				
			   break;

			}
	 
      break;

	 
	case KEY_0:
		run_t.getNumbers_key++;
	    touchkey_key_id[0]=0;
		run_t.gTimer_8s=0;
		run_t.getSpecial_1_key++;
		run_t.getSpecial_2_key++;
		key_pressed = 1;
	break;

    case KEY_1 :
		run_t.getNumbers_key++;
	    touchkey_key_id[0]=1;
		run_t.gTimer_8s=0;
		run_t.getSpecial_1_key++;
		run_t.getSpecial_2_key++;
	    key_pressed = 1;
   	break;
			
    case KEY_2:
		run_t.getNumbers_key++;
	    touchkey_key_id[2]=2;
        run_t.gTimer_8s=0;
		run_t.getSpecial_1_key++;
		run_t.getSpecial_2_key++;
  		key_pressed = 1;
	 
	break;
			
	case  KEY_3:
		run_t.getNumbers_key++;
	    touchkey_key_id[3]=3;
		run_t.gTimer_8s=0;
		run_t.getSpecial_1_key++;
		run_t.getSpecial_2_key++;
		key_pressed = 1;
	 break;
			
	case KEY_4:
		run_t.getNumbers_key++;
	    touchkey_key_id[4]=4;
		run_t.gTimer_8s=0;
		run_t.getSpecial_1_key++;
		run_t.getSpecial_2_key++;
		key_pressed = 1;
		
   break;
			
	case KEY_5:
		run_t.getNumbers_key++;
	    touchkey_key_id[5]=5;
		run_t.gTimer_8s=0;
		run_t.getSpecial_1_key++;
		run_t.getSpecial_2_key++;
		key_pressed = 1;
	break;
			
	case KEY_6:
		run_t.getNumbers_key++;
		touchkey_key_id[6]=6;
		run_t.gTimer_8s=0;
		run_t.getSpecial_1_key++;
		run_t.getSpecial_2_key++;
		key_pressed = 1;
          
   break;
   
   case KEY_7:
   		run_t.getNumbers_key++;
	     touchkey_key_id[7]=7;
        run_t.gTimer_8s=0;
		run_t.getSpecial_1_key++;
		run_t.getSpecial_2_key++;
		key_pressed = 1;
            
   break;
			
	case KEY_8:
		run_t.getNumbers_key++;
	    touchkey_key_id[8]=8;
		run_t.gTimer_8s=0;
		run_t.getSpecial_1_key++;
		run_t.getSpecial_2_key++;
		key_pressed = 1;

   break;
			
	case KEY_9:
		run_t.getNumbers_key++;
	    touchkey_key_id[9]=9;
	    run_t.gTimer_8s=0;
		run_t.getSpecial_1_key++;
		run_t.getSpecial_2_key++;
		key_pressed = 1;
		
   break;
		  
   
	}  
	default :
	break;

   	}
	 if(numbers_key != run_t.getNumbers_key && key_pressed ==1 && run_t.getNumbers_key !=0x40 && run_t.NumbersKey_pressedNumbers==0){
		numbers_key =run_t.getNumbers_key;
           key_pressed =0;
			run_t.NumbersKey_pressedNumbers=1;
			run_t.Numbers_counter ++ ;

			run_t.gTimer_8s=0;

	

			switch(run_t.Confirm_newPassword){

			  case INPUT_NEW_PWD:
			  	   if(run_t.Numbers_counter >6){ //error
				     run_t.Numbers_counter =0;
			         run_t.gTimer_8s=0;
				     run_t.enter_key = DISPOSE_KEY_FAIL; //LED and sound is display error 
				     run_t.buzzer_sound_label = sound_fail ;
					 return ;
				   }
				   
			 // break;

			  case  INPUT_NORMAL_NUMBERS:
			  	
				temp = InputNumber_ToSpecialNumbers((TouchKey_Numbers) dat); //input Numbers
			
				//input correct numbers
				if(run_t.Numbers_counter < 7){//run_t.inputNewPasswordTimes
					//alarm statement if input numbers overflow 10 numbers,input new password 
					if(run_t.inputNewPasswordTimes ==0 && run_t.inputNewPassword_Enable ==1){//WT.EDIT 2022.10.14
						read_numbers = OverNumbers_Password_Handler(); //
						if(read_numbers==1){ //error 
							
				            run_t.Numbers_counter=0;
							run_t.gTimer_8s=0;
							run_t.enter_key = DISPOSE_KEY_FAIL; //LED and sound is display error 
							run_t.buzzer_sound_label = sound_fail ;
					        return ;
					
						}
						else{
						   pwd2[run_t.Numbers_counter-1]=temp; //the first input new password
						   run_t.buzzer_sound_label = sound_key ;
						}
					}
					else{
						pwd1[run_t.Numbers_counter-1] =temp;
						run_t.buzzer_sound_label = sound_key ;
					} 

				   }

				if(run_t.Numbers_counter > 20){
					run_t.Numbers_counter =20;
				 }
				 virtualPwd[run_t.Numbers_counter-1]=temp;
				run_t.buzzer_sound_label = sound_key ;
			  break;
				

			}

			}

}
/****************************************************************************
*
*Function Name:void RunCheck_Mode(unsigned int dat)
*Function : Read from EEPROM data. 
*Input Ref: NO
*Retrun Ref:NO
*
****************************************************************************/
void RunCommand_Unlock_Keyboard(void)
{
	
    
	 if(run_t.Confirm_newPassword == 1){ //set new password flag .
	 	run_t.gTimer_8s=0;//WT.EDIT 2022.09.28
	 	Read_Administrator_Password();//WT.EDIT 2022.010.07
	 }
	 else
		ReadPassword_EEPROM_SaveData(); //Read has been saved data .
	 
     
}





/****************************************************************************
*
*Function Name:unsigned char  InputNumber_ToSpecialNumbers(TouchKey_Numbers number)
*Function : run is main 
*Input Ref: NO
*Retrun Ref:NO
*
****************************************************************************/
unsigned char  InputNumber_ToSpecialNumbers(TouchKey_Numbers number)
{
     unsigned char temp ;
	 switch(number){

      case KEY_1: 
            
	 	    temp = 1;

	  break; 

	  case KEY_2:

	       temp =2;

	  break;

	  case KEY_3:
	  	   temp =3;

	  break;

	  case KEY_4:
	  	   temp =4;
	  break;

	  case KEY_5:
	  	   temp = 5;

	  break;

	  case 	KEY_6:

	       temp =6;

	  break;

	  case KEY_7 :

	       temp =7;
	  break;

	  case KEY_8 :

	       temp =8;
	  break;

	  case KEY_9 :

	       temp =9;
	  break;

	  case KEY_0 :

	       temp =0;
	  break;


      default :
	  	 

	  break;
	}

	return temp;

}

void RunCheck_KeyMode_Handler(void(*keymode_handler)(uint16_t keydat))
{
      RunChed_KeyMode=keymode_handler; 

}
/****************************************************************************
*
*Function Name:void OverNumbers_Password_Handler(void)
*Function : run is main 
*Input Ref: NO
*Retrun Ref:NO
*
****************************************************************************/
uint8_t OverNumbers_Password_Handler(void)
{
     uint32_t    ReadAddress; 
     uint8_t   read_flag;

	ReadAddress = USER_9;
	EEPROM_Read_Byte(ReadAddress,readFlag,0x01);


	if(readFlag[0] ==1){ //over ten numbers password
		
		read_flag = 1;
	}
	else{
	   
	   read_flag = 0;
	}

	return read_flag;


}

