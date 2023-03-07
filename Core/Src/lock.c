#include "lock.h"
#include "run.h"
#include "at24c02.h"
#include "as608.h"
#include "led.h"
#include "kmp.h"
#include "buzzer.h"
#include "single_mode.h"
#include "motor.h"

uint8_t origin_pwd[6]={1,2,3,4,0,0};

static void SavePassword_To_EEPROM(void);


/****************************************************************************
*
*Function Name:static void ReadPassword_EEPROM_SaveData(void)
*Function : run is main 
*Input Ref: NO
*Retrun Ref:NO
*
****************************************************************************/
void Read_Administrator_Password(void)
{
     
	  static unsigned char value,readFlag[1];
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
	
			  }

      
	   if(run_t.eepromAddress <3){
	   	 
		    run_t.gTimer_8s =0;//
		    EEPROM_Read_Byte(ReadAddress,readFlag,0x01);
		    HAL_Delay(5);
			run_t.gTimer_8s =0;//
		   if(readFlag[0] ==1){// has a been saved pwassword 

                    
					EEPROM_Read_Byte((ReadAddress+0x01),Readpwd,6);
					HAL_Delay(5);
					run_t.gTimer_8s =0;//

                    if(run_t.Numbers_counter > 6){
 
                        value = BF_Search(virtualPwd,Readpwd);
					}
					else
					    value = CompareValue(Readpwd,pwd1);
					
					
					if(value==1)//if(strcmp(pwd1,pwd2)==0)
					{
						readFlag[0]=0;
						run_t.gTimer_8s =0;//
						run_t.open_lock_lable = open_lock_success;


						  for(i=0;i<6;i++){
	                        pwd1[i]=0;
	                        pwd2[i]=0;
	                        Readpwd[i]=0;

                      		}
						
                        
						//return ;

					}
					else{
						  
						run_t.Numbers_counter =0 ;
			
		                  if(run_t.eepromAddress==2){
		                        run_t.open_lock_lable = open_lock_fail;
								run_t.gTimer_8s =0;//
								 for(i=0;i<6;i++){
			                        pwd1[i]=0;
			                        pwd2[i]=0;
			                        Readpwd[i]=0;

		                         }
								// return ;
		                        
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
						if(syspara_t.ps_readEeprom_data >0){//WT.EDIT 2022.12.12			   
                            run_t.open_lock_lable = open_lock_fail;//// run_t.open_lock_success=1;	
                            run_t.gTimer_8s =0;//
                        }
                        else{
                            run_t.open_lock_lable = open_lock_success;//run_t.open_lock_fail=1;	
                            run_t.gTimer_8s =0;//
                        }
						for(i=0;i<6;i++){
                        pwd1[i]=0;
                        pwd2[i]=0;
                        Readpwd[i]=0;

                      }
						syspara_t.PS_login_times=0;
						//return ;

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
						// return ;
						
					}
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
     
	  static unsigned char value,readFlag[1];
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

				        syspara_t.ps_readEeprom_data = AT24CXX_ReadOneByte(EEPROM_AS608Addr);
						if(syspara_t.ps_readEeprom_data >0){//WT.EDIT 2022.12.12			   
                            run_t.open_lock_lable = open_lock_fail;//// run_t.open_lock_success=1;	
                            run_t.gTimer_8s =0;//
                        }
                        else{
                            run_t.open_lock_lable = open_lock_success;//run_t.open_lock_fail=1;	
                            run_t.gTimer_8s =0;//
                        }
					
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
/*******************************************************
 * 
 * Function Name:void Save_To_EeepromNewPwd(void)
 * Function: start power on handler
 * INPUT Ref:NO
 * Return Ref:NO
 * 
********************************************************/
void Save_To_EeepromNewPwd(void)
{

	
	if(run_t.inputNewPassword_Enable ==1 && run_t.inputNewPasswordTimes !=1 ){//WT.EDIT .2022.09.28.if(run_t.adminiId==1){
		
		   SavePassword_To_EEPROM();
			
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
static void SavePassword_To_EEPROM(void)
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
		
			
		      run_t.clear_inputNumbers_newpassword=0;//WT.EDIT 2022.10.14
		      run_t.inputNewPassword_Enable =0; //WT.EDIT 2022.09.28
				
			   return ;			
				
		   break;

		  }
        
         run_t.gTimer_8s=0;

		EEPROM_Read_Byte(run_t.userId,&run_t.readEepromData,1);
		HAL_Delay(5);
		if(run_t.readEepromData !=1){
	
             value =CompareValue(pwd1, pwd2);
			
			 if(value ==1){//login success
		       EEPROM_Write_Byte(run_t.userId ,&initvalue,1);
				 HAL_Delay(5);
				 EEPROM_Write_Byte((run_t.userId + 0x01),pwd1,6);
				 HAL_Delay(5);

				run_t.inputNewPasswordTimes =0;
			
				run_t.Numbers_counter =0;
				 run_t.motor_return_homePosition=0;
		
			
				run_t.Confirm_newPassword =0;//WT.EIDT 2022.09.12
				run_t.buzzer_sound_label = sound_excute;//run_t.buzzer_longsound_flag =1;//WT.EDIT 2022.10.28
			
				run_t.clear_inputNumbers_newpassword=0;//WT.EDIT 2022.10.14
		        run_t.inputNewPassword_Enable =0; //WT.EDIT 2022.10.14
		        OK_LED_ON(); //WT.EDIT 2022.10.28
				ERR_LED_OFF();
                  run_t.gTimer_8s=4;
				run_t.buzzer_sound_label = sound_excute ;
			    run_t.works_led_label =  works_ok_blink;
		
						
				return ;
					
			

			 }
			 else{  //log in is error 
			 	        
				run_t.inputNewPasswordTimes =0;
				run_t.Confirm_newPassword =0;  //be save eeprom data flag bit

		
				run_t.motor_return_homePosition=0;
				run_t.Numbers_counter =0;
			    run_t.clear_inputNumbers_newpassword=0;//WT.EDIT 2022.10.14

				run_t.inputNewPassword_Enable =0; //WT.EDIT 2022.09.28
			
				OK_LED_OFF(); //WT.EDIT 2022.10.28
				ERR_LED_ON();
				run_t.gTimer_8s=5;//WT.EDIT 2022.11.01
				run_t.buzzer_sound_label = sound_fail;
				run_t.works_led_label =  works_error_blink;

				
				return ;
				
				}
              
			 }
		}
	}
 }

/*******************************************************************
	*
	*Function Name: void Open_Lock_Success_Fun(void)
	*Function : open drawer is success
	*Input Ref:No
	*Return Ref:No
	*
*******************************************************************/
void Open_Lock_Success_Fun(void)
{
	uint8_t i; 

	ERR_LED_OFF();
	OK_LED_ON();
	run_t.gTimer_8s =0;
	switch(run_t.Confirm_newPassword){
      case 1:////prepare new password 
	

        run_t.inputNewPassword_Enable =1; // // be related to "Ref must be"
		run_t.motor_return_homePosition= 0;
		run_t.Numbers_counter =0 ;
		run_t.eepromAddress=0;

		syspara_t.PS_login_times=0;
		run_t.error_times=0;
		run_t.inputNewPwd_OK_led_blank_times=0;

		
		syspara_t.ps_serch_getimage=0xff;
		syspara_t.ps_serch_genchar =0xff;
		syspara_t.ps_serach_result=0xff;
		
	

        run_t.password_unlock_model=DISPOSE_STORE_MODEL_EEPROM;// permit to save new password

	  break;

	  case 0:
		if(run_t.motor_return_homePosition==1){//WT.EDIT 2022.10.28,motor runing to home

			ERR_LED_OFF();
			OK_LED_ON();
			run_t.gTimer_8s =0;

			run_t.Numbers_counter =0 ;
            run_t.error_times=0;
			
			for(i=0;i<6;i++){
			pwd1[i]=0;
			Readpwd[i]=0;
			pwd2[i]=0;

			}
			syspara_t.ps_serch_getimage=0xff;
			syspara_t.ps_serch_genchar =0xff;
			syspara_t.ps_serach_result=0xff;
			
			run_t.password_unlock_model = DISPOSE_NULL;
		}
		else{

			syspara_t.ps_serch_getimage=0xff;
			syspara_t.ps_serch_genchar =0xff;
			syspara_t.ps_serach_result=0xff;
            run_t.error_times=0;

			run_t.Numbers_counter =0 ; //WT.EDIT 2022.10.28

		    for(i=0;i<6;i++){
			pwd1[i]=0;
			Readpwd[i]=0;
			pwd2[i]=0;

		    }
			run_t.password_unlock_model = DISPOSE_MOTOR_RUN;


		}

     break;

	}


}
/*******************************************************************
	*
	*Function Name: void Open_Lock_Fail_Fun(void)
	*Function : open drawer is fail
	*Input Ref:No
	*Return Ref:No
	*
*******************************************************************/
void Open_Lock_Fail_Fun(void)
{
	uint8_t i; 

	OK_LED_OFF();
	ERR_LED_ON();
    run_t.gTimer_8s=0;//WT.EDIT 2022.09.28
	run_t.Confirm_newPassword =0; //release administrator shet input new password flag.

	
	run_t.Numbers_counter = 0;
	run_t.eepromAddress=0;

	syspara_t.PS_login_times=0xff; //WT.EDIT 2022.12.17

	run_t.error_times ++ ; //input times 5 ,
	if(run_t.error_times > 4){
	
		run_t.panel_lock=1;
		run_t.gTimer_60s=0;
		run_t.works_led_label =works_error_blink;
        run_t.password_unlock_model = KEY_LOCK_60S;
		return ;
	}
    else{
		run_t.inputNewPassword_Enable =0;
		run_t.buzzer_sound_label =sound_fail;//run_t.buzzer_fail_sound_flag=1;

		run_t.clear_inputNumbers_newpassword=0; //WT.EDIT 2022.10.14

		run_t.Numbers_counter =0; //WT.EDIT 2022.10.14

		for(i=0;i<6;i++){
			pwd1[i]=0;
			Readpwd[i]=0;
			pwd2[i]=0;

		}
		run_t.works_led_label = works_error_blink;//WORKS_ERROR_BLINK;
		syspara_t.ps_serch_getimage=0xff;
		syspara_t.ps_serch_genchar =0xff;
		syspara_t.ps_serach_result=0xff;
		run_t.password_unlock_model = open_lock_null;
    }

}
