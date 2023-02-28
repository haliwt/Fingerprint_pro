#include "cmd_link.h"
#include "usart.h"
#include "as608.h"
#include "usart.h"
#include "led.h"



uint8_t  USART1_RX_BUF[USART1_MAX_RECV_LEN]; 		//½ÓÊÕ»º³å,×î´óUSART3_MAX_RECV_LEN×Ö½Ú
//uint8_t  USART1_TX_BUF[USART1_MAX_SEND_LEN]; 		//·¢ËÍ»º³å,×î´óUSART3_MAX_SEND_LEN×Ö½Ú

uint8_t  UART1_RX_DataBuf[1]

uint8_t  UART1_DataBuf[1];

uint16_t USART1_RX_STA=0; 

uint8_t rx_rx_data_full ;
volatile static uint8_t transOngoingFlag;
static void *my_memcpy(void *dest, const void *src, unsigned short count) ;
static void uart_receive_input(unsigned char value);
static void uart_receive_buff_input(unsigned char value[], unsigned short data_len);
static unsigned char with_data_rxbuff(void);
static unsigned char take_byte_rxbuff(void);
static void Fp_uart_write_data(unsigned char *in, unsigned short len);
static void Fp_uart_write_frame(unsigned char fr_type, unsigned char fr_ver, unsigned short len);
static void uart_transmit_output(unsigned char value);
static unsigned char get_check_sum(unsigned char *pack, unsigned short pack_len);



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
    uint8_t rx_value;
	if(huart->Instance==USART1)
    {	      
	   if(USART1->ISR & UART_FLAG_RXNE){ // 1->Read data is ready to be read. 0->data is not received
		rx_value=USART1->RDR; 			 
		if((USART1_RX_STA&(1<<15))==0)//接收完的一批数据，还没有被处理，则不再接收其数据。
		{ 
			if(USART1_RX_STA<USART1_MAX_RECV_LEN)	//可以接收数据
			{
				run_t.usart_rx_timer_flag = 1;//TIM4->CNT=0;         				//counter clear empty
				if(USART1_RX_STA==0) 				// 
				{
					if(run_t.gTimer_usar_rx_timer > 2){//Ê¹ÄÜ¶¨Ê±Æ÷4
					    goto exit_rx;

					}
				}
				USART1_RX_BUF[USART1_RX_STA++]=rx_value;//USART2_RX_BUF[USART2_RX_STA++]=res;	//¼ÇÂ¼½ÓÊÕµ½µÄÖµ	 
			}else 
			{
			 exit_rx: USART1_RX_STA|=1<<15;				//force exit receive process 
			} 
		}
	 
	}

	  #if 0
		if(syspara_t.uart1_rx_data==0){
              
            if(UART1_RX_DataBuf[0]==0x55 &&   run_t.fp_handshake==0 ){
				
                run_t.fp_handshake ++ ;
				syspara_t.ps_pre_detector=1; //ahead of detector of fingerprint handshake
            }

            switch(syspara_t.ps_pre_detector){

			 case 1: 
				if(UART1_RX_DataBuf[0]==0xEF) run_temp= 1; //fingerprint of sensor response answer 0xEF01 

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


			 break;

			 case 0:

			 break;
		   
		 
			}
        }
     
	 	}

	 HAL_UART_Receive_IT(&huart1,UART1_RX_DataBuf,1); 
	#endif 
//	#else
//	if(USART1->ISR & UART_FLAG_RXNE){
//       rx_value = USART1->RDR;
//	   uart_receive_input(rx_value); 
//
//
//	}
//	#endif 
  }

/**
 * @brief  串口接收数据暂存处理
 * @param[in] {value} 串口收到的1字节数据
 * @return Null
 * @note   在MCU串口处理函数中调用该函数,并将接收到的数据作为参数传入
 */
static void uart_receive_input(unsigned char value)
{
   // #error "请在串口接收中断中调用uart_receive_input(value),串口数据由MCU_SDK处理,用户请勿再另行处理,完成后删除该行" 
    
    if(1 == rx_buf_out - rx_buf_in) {
        //串口接收缓存已满
         rx_rx_data_full =1;
    }else if((rx_buf_in > rx_buf_out) && ((rx_buf_in - rx_buf_out) >= sizeof(fp_uart_rx_buf))) {
        //串口接收缓存已满
        rx_rx_data_full =1;
    }else {
        //串口接收缓存未满
        rx_rx_data_full = 0;
        if(rx_buf_in >= (unsigned char *)(fp_uart_rx_buf + sizeof(fp_uart_rx_buf))) {
            rx_buf_in = (unsigned char *)(fp_uart_rx_buf);
        }
        
        *rx_buf_in ++ = value;
    }
}

/**
 * @brief  串口接收多个字节数据暂存处理
 * @param[in] {value} 串口要接收的数据的源地址
 * @param[in] {data_len} 串口要接收的数据的数据长度
 * @return Null
 * @note   如需要支持一次多字节缓存，可调用该函数
 */
static void uart_receive_buff_input(unsigned char value[], unsigned short data_len)
{
   // #error "请在需要一次缓存多个字节串口数据处调用此函数,串口数据由MCU_SDK处理,用户请勿再另行处理,完成后删除该行" 
    
    unsigned short i = 0;
    for(i = 0; i < data_len; i++) {
        uart_receive_input(value[i]);
    }
}


/**
 * @brief  判断串口接收缓存中是否有数据
 * @param  Null
 * @return 是否有数据
 */
static unsigned char with_data_rxbuff(void)
{
    if(rx_buf_in != rx_buf_out)
        return 1;
    else
        return 0;
}

/**
 * @brief  读取队列1字节数据
 * @param  Null
 * @return Read the data
 */
static unsigned char take_byte_rxbuff(void)
{
    unsigned char value;
    
    if(rx_buf_out != rx_buf_in) {
        //有数据
        if(rx_buf_out >= (unsigned char *)(fp_uart_rx_buf + sizeof(fp_uart_rx_buf))) {
            //数据已经到末尾
            rx_buf_out = (unsigned char *)(fp_uart_rx_buf);
        }
        
        value = *rx_buf_out ++;   
    }
    
    return value;
}

/**
 * @brief  串口发送一段数据
 * @param[in] {in} 发送缓存指针
 * @param[in] {len} 数据发送长度
 * @return Null
 */
static void Fp_uart_write_data(unsigned char *in, unsigned short len)
{
    if((NULL == in) || (0 == len)) {
        return;
    }
    
    while(len --) {
        uart_transmit_output(*in);
        in ++;
    }
}

/**
 * @brief  向wifi串口发送一帧数据
 * @param[in] {fr_type} 帧类型
 * @param[in] {fr_ver} 帧版本
 * @param[in] {len} 数据长度
 * @return Null
 */
static void Fp_uart_write_frame(unsigned char fr_type, unsigned char fr_ver, unsigned short len)
{
    unsigned char check_sum = 0;
    
    fp_uart_tx_buf[HEAD_FIRST] = 0x55;
    fp_uart_tx_buf[HEAD_SECOND] = 0xaa;
    fp_uart_tx_buf[PACKAGE_SEND_ID] = fr_ver;
    fp_uart_tx_buf[PACKAGE_SEND_ORDER] = fr_type;
    fp_uart_tx_buf[LENGTH_HIGH] = len >> 8;
    fp_uart_tx_buf[LENGTH_LOW] = len & 0xff;
    
    len += PROTOCOL_HEAD;
    check_sum = get_check_sum((unsigned char *)fp_uart_tx_buf, len - 1);
    fp_uart_tx_buf[len - 1] = check_sum;
    
    	Fp_uart_write_data((unsigned char *)fp_uart_tx_buf, len);
}

/**
 * @brief  串口发送数据
 * @param[in] {value} 串口要发送的1字节数据
 * @return Null
 */
static void uart_transmit_output(unsigned char value)
{
   // #error "请将MCU串口发送函数填入该函数,并删除该行"
    fp_OutputBuf[0]=value;
 
	HAL_UART_Transmit(&huart1, fp_OutputBuf, 1,0xffff); //WT.EDIT 2022.08.27
    
/*
    //Example:
    extern void Uart_PutChar(unsigned char value);
    Uart_PutChar(value);	                                //串口发送函数
*/
}


/**
 * @brief  wifi串口数据处理服务
 * @param  Null
 * @return Null
 * @note   在MCU主函数while循环中调用该函数
 */
void Fp_uart_service(void)
{
   // #error "请直接在main函数的while(1){}中添加wifi_uart_service(),调用该函数不要加任何条件判断,完成后删除该行" 
    static unsigned short rx_in = 0,rx_1,rx_2,rx_data_label=0;
    unsigned short offset = 0;
    unsigned short rx_value_len = 0;
	
    
    while((rx_in < sizeof(fp_data_process_buf)) && with_data_rxbuff() > 0) {
        fp_data_process_buf[rx_in ++] = take_byte_rxbuff();
	
    }
    
    if(rx_in < RESPONSE_PACKAGE_LEN -1  )
        return;
    
    while((rx_in - offset) >= RESPONSE_PACKAGE_LEN) { //fp receive data len the less 12 
        switch(rx_data_label){

		case 0:
			if(fp_data_process_buf[HEAD_FIRST] == FRAME_FIRST)){
				 offset++;
				 rx_data_lable = 1;


			}
			else{
				offset =0;
				rx_data_label = 0;

			}
				
			break;
		case 1:
			if(fp_data_process_buf[HEAD_SECOND] == FRAME_SECOND)){
				 offset++;
				 rx_data_lable = 2


			}
			else{
				offset =0;
				rx_data_label = 0;

			}
			
			break;

	
		
	    case 2:
			if(fp_data_process_buf[PACKAGE_ID] == PACKAGE_ID_DATA)){
				
				 rx_data_lable = 3;


			}
			else{
			
				rx_data_label = 0;

			}
			
			break;

		case 3: //confirm code 
		         rx_confim_code_label =fp_data_process_buf[CONFIRM_CODE] ;
			     rx_data_label = 0;

			break;

       }
    }
}

/**
 * @brief  数据帧处理
 * @param[in] {offset} 数据起始位
 * @return Null
 */
void Fp_usart_receive_data_handle(unsigned short offset)
{


    switch(cmd_type)
    {
         

        default:break;
    }
}

/**
 * @brief  计算校验和
 * @param[in] {pack} 数据源指针
 * @param[in] {pack_len} 计算校验和长度
 * @return 校验和
 */
static unsigned char get_check_sum(unsigned char *pack, unsigned short pack_len)
{
    unsigned short i;
    unsigned char check_sum = 0;
    
    for(i = 0; i < pack_len; i ++) {
        check_sum += *pack ++;
    }
    
    return check_sum;
}

/**
 * @brief  内存拷贝
 * @param[out] {dest} 目标地址
 * @param[in] {src} 源地址
 * @param[in] {count} 拷贝数据个数
 * @return 数据处理完后的源地址
 */
static void *my_memcpy(void *dest, const void *src, unsigned short count)  
{  
    unsigned char *pdest = (unsigned char *)dest;  
    const unsigned char *psrc  = (const unsigned char *)src;  
    unsigned short i;
    
    if(dest == NULL || src == NULL) { 
        return NULL;
    }
    
    if((pdest <= psrc) || (pdest > psrc + count)) {  
        for(i = 0; i < count; i ++) {  
            pdest[i] = psrc[i];  
        }  
    }else {
        for(i = count; i > 0; i --) {  
            pdest[i - 1] = psrc[i - 1];  
        }  
    }  
    
    return dest;  
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
	
	if(huart== &huart1){

       transOngoingFlag=0; //UART Transmit interrupt flag =0 ,RUN

	}

}


