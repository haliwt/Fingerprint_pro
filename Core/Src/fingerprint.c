#include "fingerprint.h"
#include "interrupt_manager.h"
#include "run.h"
#include "at24c02.h"
#include "as608.h"
#include "led.h"


uint8_t confim_flag;
uint8_t check_flag;
uint8_t merge_flag;
static void Check_Instruct_AutoRegister(uint8_t ref_number);
static void FP_MergeTemplate_Fun(uint8_t ref_numbers);
static void Fingerprint_Unlcok_fun(void);



/**********************************************************************
	*
	*Function Name:void RunCommand_Unlock_Fingerprint(void)
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

  

    if(run_t.Confirm_newPassword==1){
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
			HAL_Delay(500);
	 }
	}
   else{
   	

      syspara_t.ps_judeg_read_templete_flag = PS_ValidTempleteNum(&syspara_t.ps_read_templete_numbers);//露脕
   	  syspara_t.ps_readEeprom_data = AT24CXX_ReadOneByte(EEPROM_AS608Addr);
     if(syspara_t.ps_readEeprom_data >0) syspara_t.FP_RunCmd_Lable = FP_SEARCH;//syspara_t.FP_RunCmd_Lable = FP_SEARCH_INIT;
     else{

	    if(FP_INPUT_KEY()==1)
			HAL_Delay(200);
		
		if(FP_INPUT_KEY()==1){
			
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
	   
     	}
   	}

	//fingerprint open lock doing 
	if(FP_INPUT_KEY()==1)
		  HAL_Delay(5);
		
	if(FP_INPUT_KEY()==1){
  
   switch(syspara_t.FP_RunCmd_Lable){

    

   	  case FP_SEARCH:
	    if(FP_INPUT_KEY()==1){
        run_t.gTimer_8s=0;
	
  
      
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
                      // HAL_Delay(1000);
					   return ;
			    }
				else 
					syspara_t.FP_RunCmd_Lable=FP_SEARCH_FAIL;
			}
			else if(USART1_RX_BUF[9]==0X15){
				// HAL_Delay(1000);
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
}
/*****************************************************
	*
	*Function Name :void FP_AutoRegister_Handler(void)
	*Function : auto register login 
	*
	*
	*
******************************************************/
#if 1
void FP_AutoRegister_Handler(void)
{
   
	switch(syspara_t.fp_login_label){



      case FP_THE_FIRST_LOGIN :
	  	 PS_AutoRegister_Template(0x001);
	     syspara_t.ps_serch_getimage=PS_GetImage();
	
	     Check_Instruct_AutoRegister(0x01);

	     if(check_flag == RX_SUCCESS){

		      check_flag = fp_check_confirm_code;

		     syspara_t.fp_login_label =FP_THE_SECOND_LOGIN;

		 }
		
	     
		  

	  break;

	  case FP_THE_SECOND_LOGIN:
	  	   PS_AutoRegister_Template(0x002);
		  //  syspara_t.ps_serch_getimage=PS_GetImage();

	      Check_Instruct_AutoRegister(0x02);

	     if(check_flag == RX_SUCCESS){

			 check_flag = fp_check_confirm_code;
			 syspara_t.fp_login_label =FP_THE_THIRD_LOGIN;

		  }
		  else{
		      syspara_t.fp_login_label =FP_THE_FIRST_LOGIN;


		  }
		 

	  break;

	  case FP_THE_THIRD_LOGIN :
	  	 PS_AutoRegister_Template(0x003);
		 // syspara_t.ps_serch_getimage=PS_GetImage();
	 Check_Instruct_AutoRegister(0x03);

	     if(check_flag == RX_SUCCESS){

			 check_flag = fp_check_confirm_code;
			 syspara_t.fp_login_label =FP_THE_FOURTH_LGOIN;

		 }
		 else{

		     syspara_t.fp_login_label =FP_THE_FIRST_LOGIN;

		}

	  break;

	  case FP_THE_FOURTH_LGOIN:
	  	 PS_AutoRegister_Template(0x004);
		// syspara_t.ps_serch_getimage=PS_GetImage();
	 Check_Instruct_AutoRegister(0x04);
		 
		 if(check_flag == RX_SUCCESS){
		 	

			 check_flag = fp_check_confirm_code;
			 syspara_t.fp_login_label =FP_MERGE_TEMPLATE_ONE;

		 }
		 else{

		     syspara_t.fp_login_label =FP_THE_FIRST_LOGIN;

		}

	  break;

      case FP_MERGE_TEMPLATE_ONE:
	  	FP_MergeTemplate_Fun(0x04);
		// syspara_t.ps_serch_getimage=PS_GetImage();
		if( merge_flag ==RX_SUCCESS){
             merge_flag = 0;
		     syspara_t.fp_login_label =FP_MERGE_TEMPLATE_TWO;

		}
		else{

		     syspara_t.fp_login_label =FP_THE_FIRST_LOGIN;

		}
	  	

	  break;

	  case FP_MERGE_TEMPLATE_TWO:
	  	FP_MergeTemplate_Fun(0x05);
		if( merge_flag ==RX_SUCCESS){
             merge_flag = 0;
		     syspara_t.fp_login_label =FP_MERGE_TEMPLATE_THREE;

		}
		else{

		     syspara_t.fp_login_label =FP_THE_FIRST_LOGIN;

		}

	  break;

	  case FP_MERGE_TEMPLATE_THREE:
	  	FP_MergeTemplate_Fun(0x06);
		if( merge_flag ==RX_SUCCESS){
             merge_flag = 0;
		     syspara_t.fp_login_label =FP_AUTO_REGISTER_SUCCESS;

		}
		else{

		     syspara_t.fp_login_label =FP_THE_FIRST_LOGIN;

		}

	  break;
	  

	  case  FP_AUTO_REGISTER_SUCCESS:

		 rx_t.auto_register_flag = RX_SUCCESS ;
			
	  
	  break;

	  case FP_AUTO_REGISTER_END:

	  break;

	  





	}




}

#endif 

void Check_Instruct_AutoRegister(uint8_t ref_number)
{
  
	if(rx_t.decoder_flag == RX_SUCCESS){
	 	rx_t.decoder_flag = RX_FAIL;
	   switch(check_flag){

             case fp_check_confirm_code:
			 	 if(rx_t.fp_rx_data[0]==0x00){
                     
				      check_flag =fp_check_ref_one;
		          }

		    break;

			case fp_check_ref_one:
				if(rx_t.fp_rx_data[1]==ref_number){

				      check_flag =fp_check_ref_two;
		          }
				  else{
				     check_flag = fp_check_confirm_code;

				  }

			

			break;

			case fp_check_ref_two:
				
				if(rx_t.fp_rx_data[2]==ref_number){

					  check_flag =fp_check_ref_three;
                    
				  }
				  else{
					 check_flag = fp_check_confirm_code;

				  }

			break; 

			case fp_check_ref_three:

					
			       if(rx_t.fp_rx_data[3]==ref_number){
			
						check_flag = RX_SUCCESS;
						
						
					}
				    else{
                       check_flag = RX_FAIL;
					  check_flag = fp_check_confirm_code;

					}
					
			

			break;


		  }

	 }
}


static void FP_MergeTemplate_Fun(uint8_t ref_numbers)
{
    static uint8_t merge_flag;
    if(rx_t.decoder_flag == RX_SUCCESS){
	 	rx_t.decoder_flag =RX_FAIL;
	   switch(merge_flag){

             case 0:
			 	 if(rx_t.fp_rx_data[0]==0x00){
                     
				     merge_flag = 1;
					 rx_t.fp_rx_receive_error_data_flag=0;
		          }
				  else{

                      rx_t.fp_rx_receive_error_data_flag = 1;
				  }

		    break;

			case 1:
				 if(rx_t.fp_rx_data[1]==ref_numbers){
                     
				     merge_flag = 2;
					  
		          }
				  else{
				     merge_flag =0;
				     rx_t.fp_rx_receive_error_data_flag = 1;


				  }

			

			break;

			case 2:

			    switch(ref_numbers){

				  case 0x04:
				  	
				      if(rx_t.fp_rx_data[2]==0xF0){

					     merge_flag =RX_SUCCESS;
						 

				      }
					  else{
					     merge_flag =0;
					     rx_t.fp_rx_receive_error_data_flag = 1;
						}

				  break;

				  case 0x05:
				  	 if(rx_t.fp_rx_data[2]==0xF1){

					     merge_flag =RX_SUCCESS;

				      }
					 else{
				     merge_flag =0;
				     rx_t.fp_rx_receive_error_data_flag = 1;


				  }

				  break;

				  case 0x06:
				  	 if(rx_t.fp_rx_data[2]==0xF2){

					      merge_flag =RX_SUCCESS;
                       
				  	 	}
					 else{
				     merge_flag =0;
				     rx_t.fp_rx_receive_error_data_flag = 1;


				     }


				  break;




				}
				
			
			break; 

	


		  }

	 }

}


void Fingerprint_Unlock_Handler(void)
{

   switch(run_t.Confirm_newPassword){ //register new fingerprint 

         case 1:

         break;

		 case 0:
		 	
		 	Fingerprint_Unlcok_fun();

         break;



   	}

 }



/***************************************************************
	*
	*Function Name: static void Fingerprint_Unlcok_fun(void)
	*Function : fingerprint open lock 
	*
	*
	*
	*
***************************************************************/
static void Fingerprint_Unlcok_fun(void)
{
     static uint8_t getImage=0xff,readData;
	 uint16_t ReadAddress;
     SearchResult seach;
	 
	  syspara_t.ps_judeg_read_templete_flag = PS_ValidTempleteNum(&syspara_t.ps_read_templete_numbers);//露脕
   	  syspara_t.ps_readEeprom_data = AT24CXX_ReadOneByte(EEPROM_AS608Addr);
	  
	  if(syspara_t.ps_readEeprom_data>0){
	  	syspara_t.FP_RunCmd_Lable = FP_SEARCH;
	  }
	  else{ //default original password is "1234"
			
			
        ReadAddress = ADMINI;
        EEPROM_Read_Byte(ReadAddress,&readData,1);
	    if(readData==0){
		  syspara_t.FP_RunCmd_Lable = FP_SEARCH_OVER;
		}
		else{
			 syspara_t.fp_open_lock_label = open_lock_fail; //run_t.open_lock_lable = open_lock_fail;//run_t.open_lock_fail = 1;
		    syspara_t.FP_RunCmd_Lable = 0xff;
			 
			}
	   }
	   
       switch(syspara_t.FP_RunCmd_Lable){

			    case FP_SEARCH:
					syspara_t.ps_serch_getimage=PS_GetImage();
					if(syspara_t.ps_serch_getimage==0x00)  syspara_t.ps_serch_lable=FP_GEN_CHAR;
		            else syspara_t.FP_RunCmd_Lable=FP_SEARCH_FAIL;

			    break;

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
					syspara_t.fp_open_lock_label = open_lock_success;//run_t.open_lock_lable = open_lock_success;//run_t.open_lock_success=1;
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
					// HAL_Delay(1000);
					 return;
	       
				}
				else{
			         syspara_t.FP_RunCmd_Lable=FP_SEARCH_FAIL;
				}
		  break;
			   
		  case FP_SEARCH_FAIL:			
            syspara_t.ps_serch_getimage=0xff;
		    syspara_t.fp_open_lock_label = open_lock_fail;//run_t.open_lock_lable = open_lock_fail;
			syspara_t.FP_RunCmd_Lable = 0xff;
            return ;

			break;


			case FP_SEARCH_OVER:
			   syspara_t.ps_serch_getimage=PS_GetImage();
			   syspara_t.ps_serch_getimage=0xff;
			   syspara_t.fp_open_lock_label= open_lock_success;//run_t.open_lock_lable = open_lock_success;//run_t.open_lock_success=1;
			   run_t.error_times=0; //clear error input fingerprint of times 
			   syspara_t.FP_RunCmd_Lable = 0xff;

			break;




	




   }
}
#if 0
/*******************************************************************************
    **
    *Function Name:void Tencent_Cloud_Rx_Handler(void)
    *Function: 
    *Input Ref: +TCMQTTCONN:OK
    *Return Ref:NO
    *
********************************************************************************/
void Tencent_Cloud_Rx_Handler(void)
{
    uint8_t i;
    static uint8_t wind_hundred, wind_decade,wind_unit,temp_decade,temp_unit;
    if( esp8266data.rx_data_success==1){
         esp8266data.rx_data_success=0;
    
    if(strstr((char *)UART2_DATA.UART_Data,"nowtemperature\":")){ //WT.EDIT 2023.update
              return ;
     }

    if(strstr((char *)UART2_DATA.UART_Data,"Humidity\":")){
              return ;
     }

   if(strstr((char *)UART2_DATA.UART_Data,"open\":0")){
			   run_t.response_wifi_signal_label = OPEN_OFF_ITEM;
		 
	}
	if(strstr((char *)UART2_DATA.UART_Data,"open\":1")){
	   
	   run_t.response_wifi_signal_label = OPEN_ON_ITEM;
	}
	if(strstr((char *)UART2_DATA.UART_Data,"ptc\":0")){
            if(run_t.gPower_flag ==POWER_ON){
	           run_t.response_wifi_signal_label = PTC_OFF_ITEM;
	         
             }
			
    }
    if(strstr((char *)UART2_DATA.UART_Data,"ptc\":1")){
            if(run_t.gPower_flag ==POWER_ON){
	           // run_t.gDry=1;
			  run_t.response_wifi_signal_label = PTC_ON_ITEM;
				
            }

    }
    if(strstr((char *)UART2_DATA.UART_Data,"Anion\":0")){
          if(run_t.gPower_flag ==POWER_ON){
	          //  run_t.gPlasma=0;
			run_t.response_wifi_signal_label = ANION_OFF_ITEM;
		    
             }
		 
    }
    if(strstr((char *)UART2_DATA.UART_Data,"Anion\":1")){
            if(run_t.gPower_flag ==POWER_ON){
            //run_t.gPlasma=1;
			run_t.response_wifi_signal_label = ANION_ON_ITEM;
		
            }
    }
    if(strstr((char *)UART2_DATA.UART_Data,"sonic\":0")){
            if(run_t.gPower_flag ==POWER_ON){
           // run_t.gUlransonic=0;
			run_t.response_wifi_signal_label = SONIC_OFF_ITEM;
        
                
            }
		
    }
    if(strstr((char *)UART2_DATA.UART_Data,"sonic\":1")){
            if(run_t.gPower_flag ==POWER_ON){
            run_t.gUlransonic=1;
			run_t.response_wifi_signal_label = SONIC_ON_ITEM;
       
           }
			
    }
    if(strstr((char *)UART2_DATA.UART_Data,"state\":1")){
           if(run_t.gPower_flag ==POWER_ON){
            //run_t.gModel=1;
			run_t.response_wifi_signal_label = STATE_ON_ITEM;
        	}
		  
    }
    if(strstr((char *)UART2_DATA.UART_Data,"state\":2")){
            if(run_t.gPower_flag ==POWER_ON){
           // run_t.gModel=2;
			run_t.response_wifi_signal_label = STATE_OFF_ITEM;
            }
			
    }
    if(strstr((char *)UART2_DATA.UART_Data,"temperature")){

	        if(run_t.gPower_flag ==POWER_ON){
			run_t.response_wifi_signal_label = TEMPERATURE_ITEM;
            
	        }
			
    }
   if(strstr((char *)UART2_DATA.UART_Data,"find")){

			if(run_t.gPower_flag ==POWER_ON){

			run_t.response_wifi_signal_label= FAN_ITEM;
			
            
		}
		
    }
   
   }
   switch(run_t.response_wifi_signal_label){

      case OPEN_OFF_ITEM:
	 //  Buzzer_KeySound();
        MqttData_Publish_SetOpen(0); //WT.EDIT add
        run_t.wifi_gPower_On= 0;
	    run_t.gPower_On = POWER_OFF;
        run_t.gPower_flag =POWER_OFF;
		run_t.RunCommand_Label=POWER_OFF;

		SendWifiCmd_To_Order(WIFI_POWER_OFF);
        run_t.response_wifi_signal_label = 0xff;
	  break;

	  case OPEN_ON_ITEM:
	  //	Buzzer_KeySound();
        MqttData_Publish_SetOpen(1);  //WT.EDIT new add item
	   run_t.wifi_gPower_On= POWER_ON;
       run_t.gPower_On = POWER_ON;
	   run_t.gPower_flag =POWER_ON;
	   run_t.RunCommand_Label=POWER_ON;
	   SendWifiCmd_To_Order(WIFI_POWER_ON);
	     
	  run_t.response_wifi_signal_label = 0xff;

	  break;

	  case PTC_ON_ITEM:
	  if(run_t.gPower_flag ==POWER_ON){
	  //	 Buzzer_KeySound();
         MqttData_Publish_SetPtc(0x01);
	     run_t.gDry=1;
		 SendWifiCmd_To_Order(WIFI_PTC_ON);
        
	  	}
	 run_t.response_wifi_signal_label = 0xff;
	   break;

	  case PTC_OFF_ITEM:
	  	if(run_t.gPower_flag ==POWER_ON){
		//Buzzer_KeySound();
         MqttData_Publish_SetPtc(0);
	     run_t.gDry=0;
		 SendWifiCmd_To_Order(WIFI_PTC_OFF);
         
	  	}
	     run_t.response_wifi_signal_label = 0xff;
	  	break;

	  case ANION_OFF_ITEM: //"杀菌"
	  	if(run_t.gPower_flag ==POWER_ON){
			// Buzzer_KeySound();
            MqttData_Publish_SetPlasma(0);
            run_t.gPlasma=0;
			SendWifiCmd_To_Order(WIFI_KILL_OFF);
	  	   
	  	}
	   run_t.response_wifi_signal_label = 0xff;
	  	break;
		
	  case ANION_ON_ITEM:
	  	if(run_t.gPower_flag ==POWER_ON){
		//	 Buzzer_KeySound();
            MqttData_Publish_SetPlasma(1);
            run_t.gPlasma=1;
			SendWifiCmd_To_Order(WIFI_KILL_ON);
	  	   
	  	}
	   run_t.response_wifi_signal_label=0xff;
	    break;

	  case SONIC_OFF_ITEM:
        if(run_t.gPower_flag ==POWER_ON){

		 //   Buzzer_KeySound();
            MqttData_Publish_SetUltrasonic(0);
            run_t.gUlransonic=0;
			SendWifiCmd_To_Order(WIFI_SONIC_OFF);
			
        }
	   run_t.response_wifi_signal_label=0xff;
	  	break;

	  case SONIC_ON_ITEM:
	    if(run_t.gPower_flag ==POWER_ON){
		//	Buzzer_KeySound();
             MqttData_Publish_SetUltrasonic(1);
            run_t.gUlransonic=1;
			SendWifiCmd_To_Order(WIFI_SONIC_ON);
			
        }
	   run_t.response_wifi_signal_label=0xff;
	  	break;

	  case STATE_OFF_ITEM:
	  if(run_t.gPower_flag ==POWER_ON){
	  	//   Buzzer_KeySound();
            MqttData_Publish_SetState(2);
            run_t.gModel=2;
			SendWifiCmd_To_Order(WIFI_MODE_2);
		   
        }
	   run_t.response_wifi_signal_label = 0xff;
	  break;
		
	  case STATE_ON_ITEM:
	  	 if(run_t.gPower_flag ==POWER_ON){
		//	 Buzzer_KeySound();
		 
            MqttData_Publish_SetState(1);
            run_t.gModel=1;
			SendWifiCmd_To_Order(WIFI_MODE_1);
		   
        }
	   run_t.response_wifi_signal_label = 0xff;
	  	break;

	  case TEMPERATURE_ITEM:
	   if(run_t.gPower_flag ==POWER_ON){
		//	Buzzer_KeySound();

            temp_decade=UART2_DATA.UART_Data[14]-0x30;
            temp_unit=UART2_DATA.UART_Data[15]-0x30;
            run_t.set_temperature_value = temp_decade*10 +  temp_unit;
            if( run_t.set_temperature_value > 40)  run_t.set_temperature_value=40;
            if( run_t.set_temperature_value <20 )   run_t.set_temperature_value=20;
            MqttData_Publis_SetTemp(run_t.set_temperature_value);
			SendWifiData_To_PanelTemp(run_t.set_temperature_value);
          
         }
	  run_t.response_wifi_signal_label = 0xff;
	  break;

	  case FAN_ITEM:
	    if(run_t.gPower_flag ==POWER_ON){
			//Buzzer_KeySound();
            if(UART2_DATA.UART_Data[7]==0x31 && UART2_DATA.UART_Data[8]==0x30 && UART2_DATA.UART_Data[9]==0x30){
		           run_t.set_wind_speed_value =100;
            }
			else{
           		 wind_decade=UART2_DATA.UART_Data[7]-0x30;
	       		 wind_unit=UART2_DATA.UART_Data[8]-0x30;
                 run_t.set_wind_speed_value = wind_decade*10 + wind_unit;
			}
            if(run_t.set_wind_speed_value > 100) run_t.set_wind_speed_value=100;
            if(run_t.set_wind_speed_value < 1) run_t.set_wind_speed_value=0;
			  MqttData_Publis_SetFan(run_t.set_wind_speed_value);
    		SendWifiData_To_PanelWindSpeed(run_t.set_wind_speed_value);
          
			
            
		}
	  
	    run_t.response_wifi_signal_label = 0xff;
	  	break;

	  default :
	  break;


   }

   if(run_t.response_wifi_signal_label==0xff){
   	     Buzzer_KeySound();
		run_t.response_wifi_signal_label=0xf0;

		for(i=0;i<17;i++){
		UART2_DATA.UART_Data[i]=0;


		}

   }
	
   
}
#endif 

