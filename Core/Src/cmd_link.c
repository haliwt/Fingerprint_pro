#include "cmd_link.h"
#include "usart.h"
#include "as608.h"
#include "usart.h"
#include "led.h"
#include "fingerprint.h"
#include "run.h"
#include "tim.h"


uint8_t  USART1_RX_BUF[USART1_MAX_RECV_LEN]; 		//½ÓÊÕ»º³å,×î´óUSART3_MAX_RECV_LEN×Ö½Ú
//uint8_t  USART1_TX_BUF[USART1_MAX_SEND_LEN]; 		//·¢ËÍ»º³å,×î´óUSART3_MAX_SEND_LEN×Ö½Ú

uint8_t  UART1_RX_DataBuf[1];

uint8_t  UART1_DataBuf[1];

volatile uint16_t USART1_RX_STA; 

volatile static uint8_t transOngoingFlag;

void USART1_Cmd_Error_Handler(UART_HandleTypeDef *huart)
{

     uint32_t temp;

   if(huart==&huart1){


      if(run_t.gTimer_usart_error >27){
	  	run_t.gTimer_usart_error=0;
	     __HAL_UART_CLEAR_OREFLAG(&huart1);
		 temp = USART1->RDR;

		//  UART_Start_Receive_IT(&huart1,inputBuf,1);
		
		
         }
		  
          
        }
  	  
 }

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
	 uint8_t res;	      
	if(__HAL_UART_GET_FLAG(&huart1, UART_FLAG_RXNE) != RESET)//接收到数据
	{	 
		HAL_UART_Receive(&huart1,&res,0x01,0x50);		 
		if((USART1_RX_STA&(1<<15))==0)//接收完的一批数据,还没有被处理,则不再接收其他数据
		{ 
			if(USART1_RX_STA<USART1_MAX_RECV_LEN)	//还可以接收数据
			{
				//TIM_SetCounter(TIM4,0);//计数器清空 
				 __HAL_TIM_CLEAR_FLAG(&htim17, TIM_FLAG_UPDATE);//计数器清空
				if(USART1_RX_STA==0) 				//使能定时器4的中断 
				{
					HAL_TIM_Base_Start_IT(&htim17);//TIM_Cmd(TIM4,ENABLE);//使能定时器4
				}
				USART1_RX_BUF[USART1_RX_STA++]=res;	//记录接收到的值	 
			}else 
			{
				USART1_RX_STA|=1<<15;				//强制标记接收完成
			} 
		}
	}  				 					
	}
}

#if 0
		if(syspara_t.uart1_rx_data==0){
              
          


		   
		   if(UART1_RX_DataBuf[0]==0xEF) {
				run_temp= 1;
				syspara_t.ps_pre_detector=1; 

		    }

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
#endif 



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


