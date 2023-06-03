#include "fingerprint.h"
#include "as608.h"
#include "run.h"
#include "at24c02.h"
#include "led.h"
#include "buzzer.h"
#include "single_mode.h"
#include "usart.h"
#include "cmd_link.h"
#include "buzzer.h"

uint8_t ps_genChar=0xff,ps_regmodel=0xff,ps_storechar=0xff;
uint8_t ps_getImage=0xff;


void Fingerprint_Init(void)
{

     syspara_t.ps_serch_getimage=0xff;
	 syspara_t.ps_serch_genchar=0xff;
	 syspara_t.ps_serach_result=0xff;
     syspara_t.ps_judeg_read_templete_flag=0XFF;
  
     syspara_t.PS_login_times=0;
     syspara_t.PS_clear_ps_success=0xff;
	Fingerprint_ReadData_Handler(RunCommand_Unlock_Fingerprint);

}

/**********************************************************************
	*
	*Function Name:void Fingerprint_NewClinet_Login_Fun(void)
	*Function : 删除指纹
	*Iinput Ref: NO
	*Return Ref:NO
	*
***********************************************************************/
void Fingerprint_NewClinet_Login_Fun(void)
{
  
   run_t.gTimer_8s=0;
    

    switch(syspara_t.PS_login_times){

		case 0: //input 1 times syspara_t.PS_read_template=0;
		run_t.gTimer_8s=0;
		   if(FP_INPUT_KEY()==1){
		   	run_t.inputNewPwd_OK_led_blank_times=0;
			ps_getImage=PS_GetImage();
			if(ps_getImage==0){

			ps_genChar =PS_GenChar(CharBuffer1);//生成特征

			if(ps_genChar==0 ){
				Buzzer_KeySound();
				HAL_Delay(300);
				syspara_t.PS_login_times=1;
		
			}
			else{
				syspara_t.PS_login_times=0;	
					
			}

			}
			else{
				syspara_t.PS_login_times=0;	
				
			}
        }
    	
        break; 

        case 1: //input 2 times
		run_t.gTimer_8s=0;
              if(FP_INPUT_KEY()==1){
			  	run_t.inputNewPwd_OK_led_blank_times=0;
				syspara_t.PS_read_template=0;
				ps_getImage=PS_GetImage();

				if(ps_getImage==0){

				ps_genChar =PS_GenChar(CharBuffer1);//生成特征

				if(ps_genChar==0 ){

				Buzzer_KeySound();
				HAL_Delay(300);
				syspara_t.PS_login_times=2;
			
				}
				else{


				syspara_t.PS_login_times=1;	
			
				}

				}
				else{

				syspara_t.PS_login_times=1;	
				
				}
		    }
		    
        break; 
                     
                     
         case 2: //input 3 times
		 run_t.gTimer_8s=0;
            if(FP_INPUT_KEY()==1){
			run_t.inputNewPwd_OK_led_blank_times=0;
			syspara_t.PS_read_template=0;
			ps_getImage=PS_GetImage();

			if(ps_getImage==0){

			ps_genChar =PS_GenChar(CharBuffer1);//生成特征

			if(ps_genChar==0 ){

			Buzzer_KeySound();
			HAL_Delay(600);
			syspara_t.PS_login_times=3;
			
			run_t.inputNewPwd_OK_led_blank_times=0;
			}
			else{

			
			syspara_t.PS_login_times=2;	
		
			}

			}
			else{

			
			syspara_t.PS_login_times=2;	
		
			}
          }
        break;  

        case 3: //input 4  times 
		run_t.gTimer_8s=0;
            if(FP_INPUT_KEY()==1){
			    run_t.inputNewPwd_OK_led_blank_times=0;
	        	syspara_t.PS_read_template=0;
	            ps_regmodel=PS_RegModel();
	        //  if(ps_regmodel==0){
					syspara_t.ps_readEeprom_data = AT24CXX_ReadOneByte(EEPROM_AS608Addr);
					ps_storechar=PS_StoreChar(CharBuffer1,(syspara_t.ps_readEeprom_data+1));//????     
					//if(ps_storechar==0){ //login new fingerprint success
						if(syspara_t.ps_readEeprom_data < 41){
							OK_LED_ON() ;
					        ERR_LED_OFF();
							//syspara_t.PS_login_times=5;
							run_t.Confirm_newPassword =0;//WT.EIDT 2022.09.12
							run_t.motor_return_homePosition=0;
							run_t.inputDeepSleep_times =0; //WT.EDIT 2022.09.20
							run_t.inputNewPassword_Enable =0; //WT.EDIT 2022.12.08

							run_t.password_unlock_model =0;
				            syspara_t.ps_login_new_fp_success=1;
			                
							

							AT24CXX_WriteOneByte((EEPROM_AS608Addr),(syspara_t.ps_readEeprom_data+1));
							run_t.backlight_Cmd_lable = backlight_led_off; //run_t.led_blank	=1;//OK led blank three times
						    Buzzer_LongSound();
						    HAL_Delay(300);
							

							return ;

						}
						else{
							Buzzer_Fail_Sound();
							 OK_LED_OFF() ;
					         ERR_LED_ON();
							 syspara_t.ps_login_new_fp_success=1;
							 run_t.works_led_lable = works_led_error_blink;
							 return ;
							
						}
					}
					
					
		//	    }	
//				else{
//					
//					  Buzzer_LongSound();
//					
//					  OK_LED_ON() ;
//					  ERR_LED_OFF();
//
//					  run_t.password_unlock_model =0;
//				      syspara_t.ps_login_new_fp_success=1;
//			                
//							
//
//					   AT24CXX_WriteOneByte((EEPROM_AS608Addr),(syspara_t.ps_readEeprom_data+1));
//					    run_t.backlight_Cmd_lable = backlight_led_off; //run_t.led_blank	=1;//OK led blank three times
//						 Buzzer_LongSound();
//						 HAL_Delay(300);
//			       
//				
//					  return ;
//				}
				
				
   //         }
         break;  
       
     }
		//}  
}

/**********************************************************************
	*
	*Function Name:void press_FR(void)
	*Function : read fingerprint data 
	*Iinput Ref: NO
	*Return Ref:NO
	*
***********************************************************************/
void RunCommand_Unlock_Fingerprint(void)
{
	SearchResult seach;
     static uint8_t getImage=0xff,readData;
	 uint16_t ReadAddress;

  
    switch(run_t.Confirm_newPassword){

	   case 1: 
    
	  run_t.gTimer_8s=0;
	
       syspara_t.ps_judeg_read_templete_flag = PS_ValidTempleteNum(&syspara_t.ps_read_templete_numbers);//露脕
       syspara_t.ps_readEeprom_data = AT24CXX_ReadOneByte(EEPROM_AS608Addr);
	  if(syspara_t.ps_readEeprom_data >0){ //the first new fingerprint must be is administrator password "1234"
          syspara_t.FP_RunCmd_Lable = FP_SEARCH;
	  }
	  else{
	     
            syspara_t.ps_serch_getimage=0xff;
			run_t.open_lock_lable = open_lock_fail;//run_t.open_lock_fail = 1;
        
            syspara_t.FP_RunCmd_Lable = 0xff;  
		
	 }
	
	break;

	case 0:
        syspara_t.ps_judeg_read_templete_flag = PS_ValidTempleteNum(&syspara_t.ps_read_templete_numbers);//露脕
   	   syspara_t.ps_readEeprom_data = AT24CXX_ReadOneByte(EEPROM_AS608Addr);
       if(syspara_t.ps_readEeprom_data >0) syspara_t.FP_RunCmd_Lable = FP_SEARCH;//syspara_t.FP_RunCmd_Lable = FP_SEARCH_INIT;
       else{
         // if(FP_INPUT_KEY()==1){
			
	        ReadAddress = ADMINI;
	        EEPROM_Read_Byte(ReadAddress,&readData,1);
		    if(readData==0){
			  syspara_t.FP_RunCmd_Lable = FP_SEARCH_INIT;
			}
			else{
				 run_t.open_lock_lable = open_lock_fail;//run_t.open_lock_fail = 1;
	          
			    syspara_t.FP_RunCmd_Lable = 0xff;
				 
				}
	   		}
	   
     	//}
	   break;
   	}
  
   
  
    switch(syspara_t.FP_RunCmd_Lable){

      case FP_SEARCH:
	  if(FP_INPUT_KEY()==1){
        run_t.gTimer_8s=0;
	    run_t.touchkey_save_data_flag =0;
        syspara_t.PS_read_template=0;
      
			syspara_t.ps_serch_getimage=PS_GetImage();
			if(syspara_t.ps_serch_getimage==0x00)  syspara_t.ps_serch_lable=FP_GEN_CHAR;
            else syspara_t.FP_RunCmd_Lable=FP_SEARCH_FAIL;
	 
   
	  case FP_GEN_CHAR:
            syspara_t.ps_serch_genchar=PS_GenChar(CharBuffer1);
	
			if(syspara_t.ps_serch_genchar==0x00){ 
				if(run_t.Confirm_newPassword==1){
				     syspara_t.ps_serach_result=PS_Search(CharBuffer1,0,2,&seach);
				}
				else
				    syspara_t.ps_serach_result=PS_Search(CharBuffer1,0,45,&seach);

				if(syspara_t.ps_serach_result==0x00){

				syspara_t.ps_serch_getimage=0xff;
				run_t.open_lock_lable = open_lock_success;//run_t.open_lock_success=1;
				run_t.error_times=0; //clear error input fingerprint of times 
				syspara_t.FP_RunCmd_Lable = 0xff;
				
				return ;
				}
				else if(USART1_RX_BUF[9]==0X15){
                      
					   return ;
			    }
				else 
					syspara_t.FP_RunCmd_Lable=FP_SEARCH_FAIL;
			}
			else if(USART1_RX_BUF[9]==0X15){
			
				 return;
       
			}
			else{
		         syspara_t.FP_RunCmd_Lable=FP_SEARCH_FAIL;
			}
		   
		  case FP_SEARCH_FAIL:
			    			
            syspara_t.ps_serch_getimage=0xff;
		    run_t.open_lock_lable = open_lock_fail;//run_t.open_lock_fail = 1;
      
            syspara_t.FP_RunCmd_Lable = 0xff;
			
            return ;

			break;
        
	     }	
       
     break;
     //search initialize password of administrator is ",1,2,3,4"
     case FP_SEARCH_INIT:

	    
		   syspara_t.ps_serch_getimage=0xff;
		   run_t.open_lock_lable = open_lock_success;//run_t.open_lock_success=1;
		   run_t.error_times=0; //clear error input fingerprint of times 
		   syspara_t.FP_RunCmd_Lable = 0xff;

	 break;

	 default:

	 break;

  

     }
}
