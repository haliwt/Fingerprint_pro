#include "cmd_link.h"
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
   
    static uint8_t temp=0;

	if(huart->Instance==USART1)
    {	      
	    if(syspara_t.uart1_rx_data==0){
              
            if(UART1_RX_DataBuf[0]==0x55){
				temp=0;
				syspara_t.ps_pre_detector=1; //ahead of detector of fingerprint
				syspara_t.uart1_rx_data= 0;
            }
            
			if(UART1_RX_DataBuf[0]==0xEF) temp= 1;
			if(temp==1){
          
			USART1_RX_BUF[USART1_RX_STA]=UART1_RX_DataBuf[0];	//¼ÇÂ¼½ÓÊÕµ½µÄÖµ	
		    USART1_RX_STA++;
			if(USART1_RX_STA ==11  && syspara_t.PS_read_template==0){
				  USART1_RX_STA |=(1<<15);
				  USART1_RX_STA = USART1_RX_STA & 0X8000;
				  //Ç¿ÖÆ±ê¼Ç½ÓÊÕÍê³É
                  syspara_t.uart1_rx_data= 1;
				  syspara_t.ps_pre_detector=0;
				  temp= 0;
                      
                
		    } 
            if(USART1_RX_STA ==13  && syspara_t.PS_read_template==1){
                USART1_RX_STA |=(1<<15);
				  USART1_RX_STA = USART1_RX_STA & 0X8000;
				  //Ç¿ÖÆ±ê¼Ç½ÓÊÕÍê³É
                  syspara_t.uart1_rx_data= 1;
				  syspara_t.ps_pre_detector=0;
				 
                 temp= 0;
            }
            if(USART1_RX_STA ==15  && syspara_t.PS_read_template==2){
                USART1_RX_STA |=(1<<15);
				  USART1_RX_STA = USART1_RX_STA & 0X8000;
				  //Ç¿ÖÆ±ê¼Ç½ÓÊÕÍê³É
                  syspara_t.uart1_rx_data= 1;
				  syspara_t.ps_pre_detector=0;
				  temp= 0;
                
            }
			}
        }
        
         HAL_UART_Receive_IT(&huart1,UART1_RX_DataBuf,1); 
	}
               
  
	
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


