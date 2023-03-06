#include "interrupt_manager.h"
#include "run.h"
#include "usart.h"
#include "as608.h"
#include "usart.h"
#include "led.h"



uint8_t  USART1_RX_BUF[USART1_MAX_RECV_LEN]; 		//½ÓÊÕ»º³å,×î´óUSART3_MAX_RECV_LEN×Ö½Ú
//uint8_t  USART1_TX_BUF[USART1_MAX_SEND_LEN]; 		//·¢ËÍ»º³å,×î´óUSART3_MAX_SEND_LEN×Ö½Ú

uint8_t  UART1_RX_DataBuf[1];

uint8_t  UART1_DataBuf[1];

uint16_t USART1_RX_STA=0; 

volatile static uint8_t transOngoingFlag;

/********************************************************************************
	**
	*Function Name:void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
	*Function :UART callback function  for UART interrupt for receive data
	*Input Ref: structure UART_HandleTypeDef pointer
	*Return Ref:NO
	*
*******************************************************************************/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
   
    static uint8_t run_temp;

	if(huart->Instance==USART1)
    {	      
	    if(syspara_t.uart1_rx_data==0){
              
            if(UART1_RX_DataBuf[0]==0x55){
				
				syspara_t.ps_pre_detector=1; //ahead of detector of fingerprint
            }


		   
		    if(UART1_RX_DataBuf[0]==0xEF) run_temp= 1;

			if(run_temp==1){
          
			USART1_RX_BUF[USART1_RX_STA]=UART1_RX_DataBuf[0];	//¼ÇÂ¼½ÓÊÕµ½µÄÖµ	
		    USART1_RX_STA++;
			if(USART1_RX_STA ==11  && syspara_t.PS_read_template==0){
				  USART1_RX_STA |=(1<<15);
				  USART1_RX_STA = USART1_RX_STA & 0X8000;
				  //Ç¿ÖÆ±ê¼Ç½ÓÊÕÍê³É
                  syspara_t.uart1_rx_data= 1;
				  syspara_t.ps_pre_detector=0;
				  run_temp=0;
                      
                
		    } 
            if(USART1_RX_STA ==13  && syspara_t.PS_read_template==1){
                USART1_RX_STA |=(1<<15);
				  USART1_RX_STA = USART1_RX_STA & 0X8000;
				  //Ç¿ÖÆ±ê¼Ç½ÓÊÕÍê³É
                  syspara_t.uart1_rx_data= 1;
				  syspara_t.ps_pre_detector=0;
				 
				 
                   run_temp=0;
            }
            if(USART1_RX_STA ==15  && syspara_t.PS_read_template==2){
                USART1_RX_STA |=(1<<15);
				  USART1_RX_STA = USART1_RX_STA & 0X8000;
				  //Ç¿ÖÆ±ê¼Ç½ÓÊÕÍê³É
                  syspara_t.uart1_rx_data= 1;
				  syspara_t.ps_pre_detector=0;
	
				    run_temp=0;
                
            }
			}
        }
     
       

        }
	 HAL_UART_Receive_IT(&huart1,UART1_RX_DataBuf,1); 
  }

/********************************************************************************
**
*Function Name:void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
*Function :UART callback function  for UART interrupt for transmit data
*Input Ref: structure UART_HandleTypeDef pointer
*Return Ref:NO
*
*******************************************************************************/
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
//	
//	if(huart== &huart1){

//      // transOngoingFlag=0; //UART Transmit interrupt flag =0 ,RUN

//	}

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
    run_t.gTimer_motor_reverse++ ;

	if(t1>9){ //10ms x10=100ms
        t1=0;
        
	    run_t.gTimer_led_blink_500ms++;
	}
    if(t0>99){ //10*100 =1000ms "1s"
       t0=0;
	  run_t.gTimer_10s_start++;
	  run_t.gTimer_8s++;
	  run_t.gTimer_60s ++;
	   run_t.gTimer_ADC ++;
	  run_t.motorRunCount++; 
	  
	   if(run_t.gTimer_10s_start>9){ //10s
	   	 run_t.gTimer_10s_start=0;
	   	
		run_t.gTimer_input_error_times_60s++;
	   }
	
    } 
	}

}


