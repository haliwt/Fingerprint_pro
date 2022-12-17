#include "run.h"
#include "at24c02.h"
#include "led.h"
#include "buzzer.h"
#include "motor.h"
#include "kmp.h"
#include "single_mode.h"

#include "as608.h"




RUN_T run_t;


uint8_t readFlag[1]={0};
uint8_t pwd1[6];
uint8_t pwd2[6];
uint8_t origin_pwd[6]={1,2,3,4,0,0};
uint8_t virtualPwd[20];
uint8_t Readpwd[6];
uint8_t eevalue ;


void (*RunChed_KeyMode)(uint16_t keydat);


static void Read_Administrator_Password(void);




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
*Function Name:static void ReadPassword_EEPROM_SaveData(void)
*Function : run is main 
*Input Ref: NO
*Retrun Ref:NO
*
****************************************************************************/
void SavePassword_To_EEPROM(void)
{
   static unsigned char value,eeNumbers;
   static uint8_t initvalue =0x01;
   if(run_t.inputNewPasswordTimes ==2){//3 WT.EDIT 2022.10.14 
	for(eeNumbers =0; eeNumbers< 11;eeNumbers++){// password is ten numbers
        run_t.gTimer_8s=0;
	  switch(eeNumbers){	  
		   case 0:
		
			  run_t.userId= ADMINI;
				
		   break;

		   case 1 :
		        run_t.userId = USER_1;
			break;
		  
		   case 2: 
				run_t.userId = USER_2; 
			break;
		   
		   case 3 :
		     run_t.userId = USER_3; 
				
			break;
		   
		   case 4: 
	           run_t.userId = USER_4; 
			break;

		   case 5 :
		       run_t.userId = USER_5;  
			break;
		   
		   case 6: 
		        run_t.userId = USER_6; 
			
		    break;
		   case 7 :
		
		        run_t.userId = USER_7; 
			
		    break;
		   
		   case 8: 
		
			  run_t.userId = USER_8; 
			break;
		   
		   case 9 :
	
			run_t.userId = USER_9; 
				
			break;


		   case 10:
		   	  
				run_t.Confirm_newPassword =0; //to save new password of flag 
				run_t.password_unlock_model=0;
				
				run_t.inputNewPasswordTimes =0;
			
				run_t.Numbers_counter =0;
				run_t.passwordsMatch =0 ;
				run_t.saveEEPROM_fail_flag =1; //WT.EDIT 2022.10.06	
		      run_t.clear_inputNumbers_newpassword=0;//WT.EDIT 2022.10.14
		      run_t.inputNewPassword_Enable =0; //WT.EDIT 2022.09.28
				
			   return ;			
				
		   break;

		  }
        
        

		EEPROM_Read_Byte(run_t.userId,&run_t.readEepromData,1);
		HAL_Delay(5);
		if(run_t.readEepromData !=1){
	
             value =CompareValue(pwd1, pwd2);
			
			 if(value ==1){//login success
		       EEPROM_Write_Byte(run_t.userId ,&initvalue,1);
				 HAL_Delay(5);
				 EEPROM_Write_Byte((run_t.userId + 0x01),pwd1,6);
				 HAL_Delay(5);

	            run_t.inputDeepSleep_times =0; //WT.EDIT 2022.09.20
				run_t.inputNewPasswordTimes =0;
			
				run_t.Numbers_counter =0;
				 run_t.motor_return_homePosition=0;
		
			
				run_t.Confirm_newPassword =0;//WT.EIDT 2022.09.12
				run_t.buzzer_sound_lable = sound_excute;//run_t.buzzer_longsound_flag =1;//WT.EDIT 2022.10.28
			
				run_t.clear_inputNumbers_newpassword=0;//WT.EDIT 2022.10.14
		        run_t.inputNewPassword_Enable =0; //WT.EDIT 2022.10.14
		        OK_LED_ON(); //WT.EDIT 2022.10.28
				ERR_LED_OFF();
                  run_t.gTimer_8s=7;
				run_t.login_in_success=1; //WT.EDIT 2022.10.31
			
		
						
				return ;
					
			

			 }
			 else{
			 	        
				run_t.inputNewPasswordTimes =0;
				run_t.Confirm_newPassword =0;  //be save eeprom data flag bit

		
				run_t.motor_return_homePosition=0;
				run_t.Numbers_counter =0;
			
				run_t.buzzer_sound_lable=sound_fail;//run_t.buzzer_fail_sound_flag=1; //WT.EDIT 2022.10.06	
			
				run_t.saveEEPROM_fail_flag =1; //WT.EDIT 2022.10.06	
				run_t.inputDeepSleep_times =0; //WT.EDIT 2022.09.20
				
				run_t.clear_inputNumbers_newpassword=0;//WT.EDIT 2022.10.14

				run_t.inputNewPassword_Enable =0; //WT.EDIT 2022.09.28
			
				OK_LED_OFF(); //WT.EDIT 2022.10.28
				ERR_LED_ON();
				run_t.gTimer_8s=5;//WT.EDIT 2022.11.01
				return ;
				
				}
              
			 }
		}
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
   unsigned char temp, i;
  static unsigned char k0=0xff,k1=0xff,k2=0xff,key,spec;
   switch(dat){
	case SPECIAL_1 ://0x40: //CIN1->'*'
		
       if(k0 != run_t.getSpecial_1_key){
         k0 = run_t.getSpecial_1_key;
      
		 run_t.getSpecial_2_key++;//n1++;
		 run_t.getNumbers_key++;//n2++;
		 spec=1;
	 
		  run_t.buzzer_sound_lable = sound_key;//run_t.buzzer_key_sound_flag =1;
		  
		
		  run_t.gTimer_8s=0;  //LED turn on holde times
		  run_t.backlight_Cmd_lable=backlight_led_on;
		   POWER_ON();

		  if(run_t.inputNewPassword_Enable ==1){//WT.EDIT 2022.10.13

		      run_t.clear_inputNumbers_newpassword ++;
			  run_t.gTimer_8s=0;
			  if(run_t.clear_inputNumbers_newpassword ==2){ //the second times cancel input new password action.

				run_t.clear_inputNumbers_newpassword=0;
				run_t.inputNewPassword_Enable=0;
				run_t.Confirm_newPassword=0;
				run_t.inputNewPasswordTimes =0;

				run_t.buzzer_sound_lable = sound_key;//run_t.buzzer_key_sound_flag =1;

				run_t.input_newPassword_over_number = 0;
				OK_LED_OFF();
				ERR_LED_OFF();

			  }
			  run_t.input_newPassword_over_number = 0;//WT.EDIT 2022.10.07
			 
		 
		 if(run_t.clear_inputNumbers_newpassword ==1){//WT.EDIT 2022.10.14//if(run_t.inputNewPasswordTimes ==2){ //the first administrator password
			   run_t.gTimer_8s=0;
		
				for(i=0;i<6;i++){
					  pwd2[i]=0;
					  pwd1[i]=0;
				}
			   run_t.inputNewPasswordTimes =0; //WT.EDIT 2022.10.14
	
			  run_t.Numbers_counter =0 ;
			   run_t.gTimer_8s=0;
			 run_t.password_unlock_model=STORE_MODEL_EEPROM; //run_t.open_lock_success=STORE_MODEL;//3;
		     run_t.Confirm_newPassword=1;
			 run_t.inputNewPwd_OK_led_blank_times=0;
			   
		 }
       	}
		 else{
	        for(i=0;i<6;i++){
		  	   pwd1[i]=0;
			   Readpwd[i]=0;
		
		  	}
		 } //WT.EDIT 2022.10.07
	     ERR_LED_OFF();
		 run_t.gTimer_8s=0;
		 spec=1;
		   
		
		   run_t.Numbers_counter =0 ;
		   run_t.passwordsMatch = 0;
		   run_t.inputDeepSleep_times =0;
		  
		    
	    }
		
       break;

	

	 case SPECIAL_2://0x200: //CIN10 '#' ->confirm 
         if(k1 != run_t.getSpecial_2_key){
	         k1 = run_t.getSpecial_2_key;

           run_t.getSpecial_1_key++;//n1++
		     run_t.getNumbers_key++;//n2++;
		     spec=1;
			 
	
		   
         if(run_t.Confirm_newPassword ==0){
				run_t.buzzer_sound_lable=sound_key;//run_t.buzzer_key_sound_flag =1; 
				
            }
			else if(run_t.inputNewPasswordTimes ==0){
				run_t.buzzer_sound_lable = sound_new_pwd_the_first;//run_t. = 1;
			}
			run_t.gTimer_8s=0;
			 run_t.backlight_Cmd_lable=backlight_led_on;
			POWER_ON();

			if(run_t.Numbers_counter ==0){

				run_t.passwordsMatch = 0;
				run_t.gTimer_8s=0;
			}
		    else if(run_t.Numbers_counter < 4 && run_t.Numbers_counter >0){//error
                OK_LED_OFF();
                ERR_LED_ON();
                run_t.Numbers_counter=0;
                run_t.passwordsMatch = 0;
                run_t.error_times ++ ;
             
		
				run_t.buzzer_sound_lable =sound_fail;//run_t.buzzer_fail_sound_flag=1;
				
                if(run_t.error_times > 4 ){ //OVER 5 error  times auto lock touchkey 60 s
	                run_t.gTimer_10s_start=0;//WT.EDIT 2022.09.20
	                run_t.gTimer_input_error_times_60s =0;
	                run_t.panel_lock=1;
					run_t.gTimer_8s=0;

                }

		   }
		   else{


				if(run_t.inputNewPassword_Enable ==1){ //prepare input newpassword .WT.EDI 2022.10.13//if( run_t.Confirm_newPassword ==1){

			            run_t.inputNewPasswordTimes ++ ;  //recoder times
			          
						if(run_t.inputNewPasswordTimes ==1){
						 //Confirm Key "#"
						    
							run_t.buzzer_sound_lable = sound_new_pwd_the_second;//run_t. = 2;

						}
					
						
						run_t.passwordsMatch = 1; //run next step process
						run_t.Numbers_counter=0;
						run_t.inputDeepSleep_times =0;
						run_t.gTimer_8s=0;
						
						
			    }
				else if(run_t.motor_return_homePosition==0){ // return home position
						run_t.buzzer_sound_lable =sound_key;//run_t.buzzer_key_sound_flag =1; 
						run_t.passwordsMatch = 1;
						run_t.inputNewPasswordTimes=0; //08.13
					
						run_t.inputDeepSleep_times =0;
				}
				else if(run_t.motor_return_homePosition==1){ //repeat itself motor doing run

				        run_t.buzzer_sound_lable =sound_key;//run_t.buzzer_key_sound_flag =1; 
					   run_t.passwordsMatch=1;
				   
					   run_t.inputDeepSleep_times =0;
					   run_t.eepromAddress=0;

				 }
				 run_t.gTimer_8s=0;
			}

		}
	 
      break;

	 
	case KEY_0:
		key=1;
		spec=0;
		run_t.getNumbers_key++;
		run_t.inputDeepSleep_times =0;
		run_t.gTimer_input_standby_cnt=0;
		run_t.gTimer_8s=0;
		run_t.backlight_Cmd_lable = backlight_led_on;
            
   break;

    case KEY_1 :

		key=1;
		spec=0;
		run_t.getNumbers_key++;
		run_t.inputDeepSleep_times =0;
		run_t.gTimer_input_standby_cnt=0;
		run_t.gTimer_8s=0;
	   run_t.backlight_Cmd_lable = backlight_led_on;
   	 
	break;
			
    case KEY_2:

		key=1;
		spec=0;
		run_t.getNumbers_key++;
		run_t.inputDeepSleep_times =0;
		run_t.gTimer_input_standby_cnt=0;
		run_t.gTimer_8s=0;
		run_t.backlight_Cmd_lable = backlight_led_on;
  
	 
	break;
			
	case  KEY_3:
		key=1;
		spec=0;
		run_t.getNumbers_key++;
		run_t.inputDeepSleep_times =0;
		run_t.gTimer_input_standby_cnt=0;
		run_t.gTimer_8s=0;
	   run_t.backlight_Cmd_lable = backlight_led_on;         
 
	
    break;
			
	case KEY_4:
		key=1;
		spec=0;
		run_t.getNumbers_key++;
		run_t.inputDeepSleep_times =0;
		run_t.gTimer_input_standby_cnt=0;
		run_t.gTimer_8s=0;
		run_t.backlight_Cmd_lable = backlight_led_on;
   break;
			
	case KEY_5:
		key=1;
		spec=0;
		run_t.getNumbers_key++;
		run_t.inputDeepSleep_times =0;
		run_t.gTimer_input_standby_cnt=0;
		run_t.gTimer_8s=0;
		run_t.backlight_Cmd_lable = backlight_led_on;
           
   break;
			
	case KEY_6:
		key=1;
		spec=0;
		run_t.getNumbers_key++;
		run_t.inputDeepSleep_times =0;
		run_t.gTimer_input_standby_cnt=0;
		run_t.gTimer_8s=0;
		run_t.backlight_Cmd_lable = backlight_led_on;
          
   break;
	case KEY_7:
		
		key=1;
		spec=0;
		run_t.getNumbers_key++;
		run_t.inputDeepSleep_times =0;
		run_t.gTimer_input_standby_cnt=0;
		run_t.gTimer_8s=0;
		run_t.backlight_Cmd_lable = backlight_led_on;
            
   break;
			
	case KEY_8:
		key=1;
		spec=0;
		run_t.getNumbers_key++;
		run_t.inputDeepSleep_times =0;
		run_t.gTimer_input_standby_cnt=0;
		run_t.gTimer_8s=0;
        run_t.backlight_Cmd_lable = backlight_led_on;
   break;
			
	case KEY_9:
		key=1;
		spec=0;
		run_t.getNumbers_key++;
		run_t.inputDeepSleep_times =0;
		run_t.gTimer_input_standby_cnt=0;
		run_t.gTimer_8s=0;
		run_t.backlight_Cmd_lable = backlight_led_on;
   break;
		  

	}  

	
	  if(k2 != run_t.getNumbers_key && key==1 && spec ==0 && run_t.getNumbers_key !=0x40 &&run_t.NumbersKey_pressedNumbers==0){
				
				k2=run_t.getNumbers_key;
		      key = 0;
			   spec =1;
			   run_t.getSpecial_1_key++;//n1++

			   run_t.getSpecial_2_key++;//n1++;
		    
				run_t.NumbersKey_pressedNumbers=1;
				run_t.Numbers_counter ++ ;
				run_t.buzzer_sound_lable =sound_key;//run_t.buzzer_key_sound_flag =1;
			
				 run_t.gTimer_8s=0;
		
				 run_t.passwordsMatch =0;
				 run_t.backlight_Cmd_lable = backlight_led_on;
				 POWER_ON();

				 if(run_t.Confirm_newPassword ==1 && run_t.Numbers_counter >6){//WT.EDIT 2022.10.08

				    run_t.input_newPassword_over_number = 1;//run_t.lock_fail=1;
			        run_t.gTimer_8s=0;
				    run_t.buzzer_sound_lable = sound_fail;//run_t.buzzer_fail_sound_flag =1;
	 	   
	             }
			     else{
					temp = InputNumber_ToSpecialNumbers((TouchKey_Numbers) dat); //input Numbers
					if(run_t.Numbers_counter > 20) run_t.Numbers_counter =20;
					virtualPwd[run_t.Numbers_counter-1]=temp;
				
				    
					 
				     if(run_t.Numbers_counter < 7){//run_t.inputNewPasswordTimes

					  if(run_t.inputNewPasswordTimes ==0 && run_t.inputNewPassword_Enable ==1){//WT.EDIT 2022.10.14
					  	pwd2[run_t.Numbers_counter-1]=temp; //the first input new password .
					  }
	                  else  pwd1[run_t.Numbers_counter-1] =temp;
				     
	               
	             }
				
			   }
			run_t.gTimer_8s=0;
		    
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
	
    
	 if(run_t.Confirm_newPassword == 1){
	 	run_t.gTimer_8s=0;//WT.EDIT 2022.09.28
	 	Read_Administrator_Password();//WT.EDIT 2022.010.07
	 }
	 else
		ReadPassword_EEPROM_SaveData();
	 
     
}

/****************************************************************************
	*
	*Function Name:void Lock_Open_Order(void)
	*Function : excute open and don't open lock 
	*Input Ref: NO
	*Retrun Ref:NO
	*
****************************************************************************/
void Lock_Open_Order(void)
{
	
    uint8_t i; 
 if(run_t.detection_input_flag==1){
		run_t.detection_input_flag=0;
    switch(run_t.open_lock_lable){

    	 case open_lock_success:
            ERR_LED_OFF();
		    OK_LED_ON();

    	  if(run_t.Confirm_newPassword ==1){ //prepare new password 
			
			
				run_t.inputNewPassword_Enable =1; //Input Administrator password is OK
				run_t.motor_return_homePosition= 0;
				run_t.Numbers_counter =0 ;
				run_t.eepromAddress=0;
				run_t.passwordsMatch = 0;
			    syspara_t.PS_login_times=0;
				
				
				run_t.inputDeepSleep_times =0;
				run_t.gTimer_input_standby_cnt=0;
				run_t.error_times=0;
			
				run_t.gTimer_8s =0;
				
                run_t.buzzer_sound_lable =sound_high;
                run_t.inputNewPwd_OK_led_blank_times=0;
			    run_t.works_led_lable = works_ok_blink;
			    run_t.password_unlock_model=STORE_MODEL_EEPROM;// permit to save new password
			  syspara_t.ps_serch_getimage=0xff;
              syspara_t.ps_serch_genchar =0xff;
              syspara_t.ps_serach_result=0xff;
			
		}
		else{ //runing open lock 
		    if(run_t.motor_return_homePosition==1){//WT.EDIT 2022.10.28
				run_t.buzzer_sound_lable =sound_excute;//Buzzer_LongSound(); //WT.EDIT 2022.10.06
				ERR_LED_OFF();
				OK_LED_ON();
				

				run_t.Numbers_counter =0 ;
				run_t.passwordsMatch = 0;
				run_t.inputDeepSleep_times =0;
				for(i=0;i<6;i++){
					pwd1[i]=0;
					Readpwd[i]=0;
					pwd2[i]=0;

				}
				syspara_t.ps_serch_getimage=0xff;
				syspara_t.ps_serch_genchar =0xff;
				syspara_t.ps_serach_result=0xff;
				syspara_t.handler_read_data_flag++;

			}
            else{
		   	    
                syspara_t.ps_serch_getimage=0xff;
                syspara_t.ps_serch_genchar =0xff;
                syspara_t.ps_serach_result=0xff;
                
                run_t.motor_doing_flag=1;
                run_t.Motor_RunCmd_Label=motor_run_start;
                run_t.motorRunCount=0;
				 
			    	run_t.Numbers_counter =0 ; //WT.EDIT 2022.10.28
				   run_t.passwordsMatch = 0;
				   run_t.inputDeepSleep_times =0;
				   run_t.buzzer_sound_lable = sound_excute;
					   
			       for(i=0;i<6;i++){
				  	   pwd1[i]=0;
					   Readpwd[i]=0;
					   pwd2[i]=0;
				
				  	}
			   	// syspara_t.handler_read_data_flag++;

               }

 
	     }


       run_t.open_lock_lable=open_lock_null;
   break;

   case open_lock_fail:
			OK_LED_OFF();
			ERR_LED_ON();

            syspara_t.handler_read_data_flag++;
			run_t.Numbers_counter = 0;
			run_t.eepromAddress=0;
			run_t.passwordsMatch = 0;
			syspara_t.PS_login_times=0xff; //WT.EDIT 2022.12.17
		
			run_t.error_times ++ ; //input times 5 ,
			if(run_t.error_times > 4){
				run_t.gTimer_10s_start=0;
				run_t.gTimer_input_error_times_60s =0;
				run_t.panel_lock=1;
				run_t.gTimer_8s=0;//WT.EDIT 2022.09.28
            run_t.works_led_lable = works_error_led_on;

			}
			run_t.Confirm_newPassword =0;
			run_t.inputNewPassword_Enable =0;
			run_t.buzzer_sound_lable =sound_fail;//run_t.buzzer_fail_sound_flag=1;

			run_t.clear_inputNumbers_newpassword=0; //WT.EDIT 2022.10.14

			run_t.Numbers_counter =0; //WT.EDIT 2022.10.14
			
			for(i=0;i<6;i++){
			pwd1[i]=0;
			Readpwd[i]=0;
			pwd2[i]=0;

			}
			run_t.works_led_lable =  works_error_blink;
             syspara_t.ps_serch_getimage=0xff;
                syspara_t.ps_serch_genchar =0xff;
                syspara_t.ps_serach_result=0xff;
      run_t.open_lock_lable=open_lock_null;
	break;

	case open_lock_null:
		run_t.open_lock_lable=0xff;

	break;

	}

   }
}

/****************************************************************************
*
*Function Name:static void ReadPassword_EEPROM_SaveData(void)
*Function : run is main 
*Input Ref: NO
*Retrun Ref:NO
*
****************************************************************************/
static void Read_Administrator_Password(void)
{
     
	  static unsigned char value;
	  static uint16_t   ReadAddress; 
      uint8_t i;

	 for(run_t.eepromAddress =0; run_t.eepromAddress <3;run_t.eepromAddress++){ //2022.10.07 be changed ten password 
	  
	    switch(run_t.eepromAddress){
	
				 case 0:
					  ReadAddress = ADMINI;
				 break;
				 
				 case 1:
					 ReadAddress = USER_1;
				  
			   break;
	
			   case 2:
					 ReadAddress = USER_2;
			   break;

			   default:

			   break;
	
			  
	
		   }

      
	   if(run_t.eepromAddress <3){
	   	 
		    run_t.gTimer_8s =0;//
		    EEPROM_Read_Byte(ReadAddress,readFlag,0x01);
		    HAL_Delay(5);
		   if(readFlag[0] ==1){// has a been saved pwassword 

                    
					EEPROM_Read_Byte((ReadAddress+0x01),Readpwd,6);
					HAL_Delay(5);
					

                    if(run_t.Numbers_counter > 6){
 
                        value = BF_Search(virtualPwd,Readpwd);
					}
					else
					    value = CompareValue(Readpwd,pwd1);
					
					
					if(value==1)//if(strcmp(pwd1,pwd2)==0)
					{
						readFlag[0]=0;
						run_t.open_lock_lable = open_lock_success;//run_t.open_lock_success=1;
						  run_t.gTimer_8s =0;//
						  for(i=0;i<6;i++){
	                        pwd1[i]=0;
	                        pwd2[i]=0;
	                        Readpwd[i]=0;

                      		}
                        
						return ;

					}
					else{
						  
                     	//run_t.inputNewPassword_Enable =1; //Input Administrator password is OK
						run_t.Numbers_counter =0 ;
						run_t.passwordsMatch = 0;
                  if(run_t.eepromAddress==2){
                         run_t.open_lock_lable = open_lock_fail;//run_t.open_lock_fail = 1;
						
						 run_t.gTimer_8s =0;//
						 for(i=0;i<6;i++){
	                        pwd1[i]=0;
	                        pwd2[i]=0;
	                        Readpwd[i]=0;

                         }
						 return ;
                        
                   }
						
					}

			}
            else{
		   
			if(run_t.eepromAddress==2){ //don't has a empty space,default password is  "1,2,3,4" ,don't be write new  password

			        ReadAddress = ADMINI;
                    EEPROM_Read_Byte(ReadAddress,readFlag,1);
                    HAL_Delay(5);
                   if(readFlag[0] ==0){

				    
                     if(run_t.Numbers_counter > 4){//6
 
                            value=0;
							    
                         //value = BF_Search(virtualPwd,origin_pwd);
					 }
                    else
					 value =CompareValue(origin_pwd, pwd1);

				   if(value==1){

				   		syspara_t.ps_readEeprom_data = AT24CXX_ReadOneByte(EEPROM_AS608Addr);
						if(syspara_t.ps_readEeprom_data==0){//WT.EDIT 2022.12.12			   
                            run_t.open_lock_lable = open_lock_success;// run_t.open_lock_success=1;	
                            run_t.gTimer_8s =0;//
                        }
                        else{
                            run_t.open_lock_lable = open_lock_fail;//run_t.open_lock_fail=1;	
                            run_t.gTimer_8s =0;//
                        }
						for(i=0;i<6;i++){
                        pwd1[i]=0;
                        pwd2[i]=0;
                        Readpwd[i]=0;

                      }
						 syspara_t.PS_login_times=0;
						return ;

					}
					else{

					     run_t.open_lock_lable = open_lock_fail;//run_t.open_lock_fail = 1;
						 run_t.gTimer_8s =0;//
						   for(i=0;i<6;i++){
                        pwd1[i]=0;
                        pwd2[i]=0;
                        Readpwd[i]=0;

                      }
						 syspara_t.PS_login_times=0;
						 return ;
						
					}
				 }
                 else{
                          run_t.open_lock_lable = open_lock_fail;//run_t.open_lock_fail = 1;
						 run_t.gTimer_8s =0;//
						   for(i=0;i<6;i++){
                        pwd1[i]=0;
                        pwd2[i]=0;
                        Readpwd[i]=0;

                      }
						 return ;
                 
                 }
             }
			 //n_t.eepromAddress++ ;	
				 
			}
           }

		 
	   	}
}


/****************************************************************************
*
*Function Name:static void ReadPassword_EEPROM_SaveData(void)
*Function : run is main 
*Input Ref: NO
*Retrun Ref:NO
*
****************************************************************************/
void ReadPassword_EEPROM_SaveData(void)
{
     
	  static unsigned char value;
	  static uint32_t    ReadAddress; 

	 for(run_t.eepromAddress =0; run_t.eepromAddress <11;run_t.eepromAddress++){ //2022.10.07 be changed ten password 
	  

		switch(run_t.eepromAddress){
	
				 case 0:
					  ReadAddress = ADMINI;
				 break;
				 case 1:
					 ReadAddress = USER_1;
				  
			   break;
	
				 case 2:
					 ReadAddress = USER_2;
			   break;
	
			   case 3:
					 ReadAddress = USER_3;
			   break;
	
			   case 4:
					 ReadAddress = USER_4;
			   break;
	
			   case 5:
					ReadAddress = USER_5;
				break;
	
			   case 6:
					 ReadAddress = USER_6;
				break;
				
				case 7:
					ReadAddress = USER_7;
				  break;
	
				 case 8:
				 
				   ReadAddress = USER_8;
				 break;
	
				 case 9:
				 
					  ReadAddress = USER_9;
			   break;
	
				 case 10:
				   run_t.open_lock_lable = open_lock_fail;
				   return ;
				break;
	
		   }

      
	   if(run_t.eepromAddress <10){
	   	   if(run_t.Confirm_newPassword == 1){ //set save new password flag bit by administrator open lock
                ReadAddress = ADMINI;
           }
		   
		    EEPROM_Read_Byte(ReadAddress,readFlag,1);
		  //  HAL_Delay(5);
		   if(readFlag[0] ==1){// has a been saved pwassword 

					EEPROM_Read_Byte(ReadAddress + 0X01,Readpwd,6);
				//	HAL_Delay(5);
					

                    if(run_t.Numbers_counter > 6){
 
                        value = BF_Search(virtualPwd,Readpwd);
					}
					else
					    value = CompareValue(Readpwd,pwd1);
					
					
					if(value==1)//if(strcmp(pwd1,pwd2)==0)
					{
						readFlag[0]=0;
						
						run_t.open_lock_lable = open_lock_success;
						return ;

					}
					else{
						if(run_t.Confirm_newPassword ==1){
                     		readFlag[0]=0;
						   run_t.open_lock_lable = open_lock_fail;
							return ;
						}
						//n_t.eepromAddress++ ;	
					}

			}
			else{ //don't has a empty space,default password is  "1,2,3,4" ,don't be write new  password

			     if(ReadAddress == ADMINI){

				    
                     if(run_t.Numbers_counter > 4){
 
                            value=0;
							    
                         //value = BF_Search(virtualPwd,origin_pwd);
					 }
                    else
					 value =CompareValue(origin_pwd, pwd1);

				   if(value==1){
									   
						 run_t.open_lock_lable = open_lock_success;
					
						return ;

					}
					else{

					    run_t.open_lock_lable = open_lock_fail;
						 return ;
						
					}
				 }
				 //n_t.eepromAddress++ ;	
				 
			}

		 
	   	}
	  
	 }
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



