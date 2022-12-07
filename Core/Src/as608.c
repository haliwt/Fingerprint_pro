//////////////////////////////////////////////////////////////////////////////////	 
//±¾³ÌĞòÖ»¹©Ñ§Ï°Ê¹ÓÃ£¬Î´¾­×÷ÕßĞí¿É£¬²»µÃÓÃÓÚÆäËüÈÎºÎÓÃÍ¾
//ALIENTEK Õ½½¢V3 STM32¿ª·¢°å
//ATK-AS608Ö¸ÎÆÊ¶±ğÄ£¿éÇı¶¯´úÂë	   
//ÕıµãÔ­×Ó@ALIENTEK
//¼¼ÊõÂÛÌ³:www.openedv.com
//´´½¨ÈÕÆÚ:2016/3/29
//°æ±¾£ºV1.0
//°æÈ¨ËùÓĞ£¬µÁ°æ±Ø¾¿¡£
//Copyright(C) ¹ãÖİÊĞĞÇÒíµç×Ó¿Æ¼¼ÓĞÏŞ¹«Ë¾ 2014-2024
//All rights dataerved									  
////////////////////////////////////////////////////////////////////////////////// 	
#include <string.h>
#include "delay.h" 	
#include "usart.h"
#include "as608.h"
#include "cmd_link.h"
#include "at24c02.h"
#include "run.h"
#include "led.h"
#include "buzzer.h"
SysPara syspara_t;

uint8_t ps_buffer[50];
uint16_t ArraypageID[16] ;
uint16_t Arraymathscore[16];

uint8_t ensure_getImage,ensure_regModel, ensure_storeChar;

SysPara AS608Para;//æŒ‡çº¹æ¨¡å—AS608å‚æ•°


uint32_t AS608Addr = 0XFFFFFFFF; //Ä¬ÈÏ

uint16_t EEPROM_AS608Addr = 0x61;

//³õÊ¼»¯PA6ÎªÏÂÀ­ÊäÈë		    
//¶ÁÃş³ö¸ĞÓ¦×´Ì¬(´¥Ãş¸ĞÓ¦Ê±Êä³ö¸ßµçÆ½ĞÅºÅ)
void PS_StaGPIO_Init(void)
{   
//  GPIO_InitTypeDef  GPIO_InitStructure;
//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//Ê¹ÄÜGPIOAÊ±ÖÓ
//  //³õÊ¼»¯¶Á×´Ì¬Òı½ÅGPIOA
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;//ÊäÈëÏÂÀ­Ä£Ê½
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//50MHz
//  GPIO_Init(GPIOA, &GPIO_InitStructure);//³õÊ¼»¯GPIO	
}
//send data one byte
static void MYUSART_SendData(uint8_t data)
{
	while((USART1->ISR&0X40)==0); 
	USART1->TDR = data;
}
//·¢ËÍ°üÍ·
static void SendHead(void)
{
	MYUSART_SendData(0xEF);
	MYUSART_SendData(0x01);
}
//·¢ËÍµØÖ·
static void SendAddr(void)
{
	MYUSART_SendData(AS608Addr>>24);
	MYUSART_SendData(AS608Addr>>16);
	MYUSART_SendData(AS608Addr>>8);
	MYUSART_SendData(AS608Addr);
}
//·¢ËÍ°ü±êÊ¶,
static void SendFlag(uint8_t flag)
{
	MYUSART_SendData(flag);
}
//·¢ËÍ°ü³¤¶È
static void SendLength(int length)
{
	MYUSART_SendData(length>>8);
	MYUSART_SendData(length);
}
//·¢ËÍÖ¸ÁîÂë
static void Sendcmd(uint8_t cmd)
{
	MYUSART_SendData(cmd);
}
//·¢ËÍĞ£ÑéºÍ
static void SendCheck(uint16_t check)
{
	MYUSART_SendData(check>>8);
	MYUSART_SendData(check);
}
/*******************************************************************************
**
*Function Name:void Subscribe_Rx_IntHandler(void)
*Function: interrupt USART2 receive data fun
*Input Ref: +TCMQTTCONN:OK
*Return Ref:NO
*
********************************************************************************/
void PS_Rx_InputInfo_Handler(void)
{
    
    
          strcpy((char *)ps_buffer, (const char *)USART1_RX_BUF);
         // USART1_RX_STA=0;
        
         
         
        
		   	
}
//åˆ¤æ–­ä¸­æ–­æ¥æ”¶çš„æ•°ç»„æœ‰æ²¡æœ‰åº”ç­”åŒ…
//waittimeä¸ºç­‰å¾…ä¸­æ–­æ¥æ”¶æ•°æ®çš„æ—¶é—´ï¼ˆå•ä½1msï¼‰
//è¿”å›å€¼ï¼šæ•°æ®åŒ…é¦–åœ°å€
static uint8_t *JudgeStr(uint16_t waittime)
{
	char *data;
	uint8_t str[8];
	str[0]=0xef;
    str[1]=0x01;
    str[2]=AS608Addr>>24;
	str[3]=AS608Addr>>16;
    str[4]=AS608Addr>>8;
	str[5]=AS608Addr;
    str[6]=0x07;
    str[7]='\0';
	USART1_RX_STA=0;
    syspara_t.uart1_rx_data= 0;
    while(--waittime)
	{
		delay_ms(1);
		if(USART1_RX_STA&0X8000 || syspara_t.uart1_rx_data==1)//Â½Ã“ÃŠÃ•ÂµÂ½Ã’Â»Â´ÃÃŠÃ½Â¾Ã
		{
			USART1_RX_STA=0;
			syspara_t.uart1_rx_data= 0;
			data=strstr((const char*)USART1_RX_BUF,(const char*)str);
			if(data)
				return (uint8_t*)data;	
		}
	}
	return 0;
   
}
//å½•å…¥å›¾åƒ PS_GetImage
//åŠŸèƒ½:æ¢æµ‹æ‰‹æŒ‡ï¼Œæ¢æµ‹åˆ°åå½•å…¥æŒ‡çº¹å›¾åƒå­˜äºImageBufferã€‚ 
//æ¨¡å—è¿”å›ç¡®è®¤å­—
uint8_t PS_GetImage(void)
{
  uint16_t temp;
  uint8_t  ensure;
	uint8_t  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//å‘½ä»¤åŒ…æ ‡è¯†
	SendLength(0x03);
	Sendcmd(0x01);
     temp =  0x01+0x03+0x01;
	SendCheck(temp);
	data=JudgeStr(2000);
	if(data){
		ensure=data[9]; //PS_GetImage confirmation Code
    }
	else{
		ensure=0xFF;//

    }
	return ensure;
}
//ç”Ÿæˆç‰¹å¾ PS_GenChar
//åŠŸèƒ½:å°†ImageBufferä¸­çš„åŸå§‹å›¾åƒç”ŸæˆæŒ‡çº¹ç‰¹å¾æ–‡ä»¶å­˜äºCharBuffer1æˆ–CharBuffer2			 
//å‚æ•°:BufferID --> charBuffer1:0x01	charBuffer1:0x02												
//æ¨¡å—è¿”å›ç¡®è®¤å­—
uint8_t PS_GenChar(uint8_t BufferID)
{
	uint16_t temp;
  uint8_t  ensure;
	uint8_t  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//ÃüÁî°ü±êÊ¶
	SendLength(0x04);
	Sendcmd(0x02);
	MYUSART_SendData(BufferID);
	temp = 0x01+0x04+0x02+BufferID;
	SendCheck(temp);
	data=JudgeStr(2000);
	if(data)
		ensure=data[9];
	else
		ensure=0xff;
	return ensure;
}
//ç²¾ç¡®æ¯”å¯¹ä¸¤æšæŒ‡çº¹ç‰¹å¾ PS_Match
//åŠŸèƒ½:ç²¾ç¡®æ¯”å¯¹CharBuffer1 ä¸CharBuffer2 ä¸­çš„ç‰¹å¾æ–‡ä»¶ 
//æ¨¡å—è¿”å›ç¡®è®¤å­—
uint8_t PS_Match(void)
{
	uint16_t temp;
  uint8_t  ensure;
	uint8_t  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//ÃüÁî°ü±êÊ¶
	SendLength(0x03);
	Sendcmd(0x03);
	temp = 0x01+0x03+0x03;
	SendCheck(temp);
	data=JudgeStr(2000);
	if(data)
		ensure=data[9];
	else
		ensure=0xff;
	return ensure;
}
//æœç´¢æŒ‡çº¹ PS_Search
//åŠŸèƒ½:ä»¥CharBuffer1æˆ–CharBuffer2ä¸­çš„ç‰¹å¾æ–‡ä»¶æœç´¢æ•´ä¸ªæˆ–éƒ¨åˆ†æŒ‡çº¹åº“.è‹¥æœç´¢åˆ°ï¼Œåˆ™è¿”å›é¡µç ã€‚			
//å‚æ•°:  BufferID @ref CharBuffer1	CharBuffer2
//è¯´æ˜:  æ¨¡å—è¿”å›ç¡®è®¤å­—ï¼Œé¡µç ï¼ˆç›¸é…æŒ‡çº¹æ¨¡æ¿ï¼‰
uint8_t PS_Search(uint8_t BufferID,uint16_t StartPage,uint16_t PageNum,SearchResult *p)
{
	uint16_t temp;
  uint8_t  ensure;
	uint8_t  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//ÃüÁî°ü±êÊ¶
	SendLength(0x08);
	Sendcmd(0x04);
	MYUSART_SendData(BufferID);
	MYUSART_SendData(StartPage>>8);
	MYUSART_SendData(StartPage);
	MYUSART_SendData(PageNum>>8);
	MYUSART_SendData(PageNum);
	temp = 0x01+0x08+0x04+BufferID
	+(StartPage>>8)+(uint8_t)StartPage
	+(PageNum>>8)+(uint8_t)PageNum;
	SendCheck(temp);
	data=JudgeStr(2000);
	if(data)
	{
		ensure = data[9];
		p->pageID   =(data[10]<<8)+data[11];
		p->mathscore=(data[12]<<8)+data[13];	
	}
	else
		ensure = 0xff;
	return ensure;	
}
//åˆå¹¶ç‰¹å¾ï¼ˆç”Ÿæˆæ¨¡æ¿ï¼‰PS_RegModel
//åŠŸèƒ½:å°†CharBuffer1ä¸CharBuffer2ä¸­çš„ç‰¹å¾æ–‡ä»¶åˆå¹¶ç”Ÿæˆ æ¨¡æ¿,ç»“æœå­˜äºCharBuffer1ä¸CharBuffer2	
//è¯´æ˜:  æ¨¡å—è¿”å›ç¡®è®¤å­—
uint8_t PS_RegModel(void)
{
	uint16_t temp;
  uint8_t  ensure;
	uint8_t  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//ÃüÁî°ü±êÊ¶
	SendLength(0x03);
	Sendcmd(0x05);
	temp = 0x01+0x03+0x05;
	SendCheck(temp);
	data=JudgeStr(200);
	if(data)
		ensure=data[9];
	else
		ensure=0xff;
	return ensure;		
}
//å‚¨å­˜æ¨¡æ¿ PS_StoreChar
//åŠŸèƒ½:å°† CharBuffer1 æˆ– CharBuffer2 ä¸­çš„æ¨¡æ¿æ–‡ä»¶å­˜åˆ° PageID å·flashæ•°æ®åº“ä½ç½®ã€‚			
//å‚æ•°:  BufferID @ref charBuffer1:0x01	charBuffer1:0x02
//       PageIDï¼ˆæŒ‡çº¹åº“ä½ç½®å·ï¼‰
//è¯´æ˜:  æ¨¡å—è¿”å›ç¡®è®¤å­—

uint8_t PS_StoreChar(uint8_t BufferID,uint16_t PageID)
{
	uint16_t temp;
  uint8_t  ensure;
	uint8_t  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//ÃüÁî°ü±êÊ¶
	SendLength(0x06);
	Sendcmd(0x06);
	MYUSART_SendData(BufferID);
	MYUSART_SendData(PageID>>8);
	MYUSART_SendData(PageID);
	temp = 0x01+0x06+0x06+BufferID
	+(PageID>>8)+(uint8_t)PageID;
	SendCheck(temp);
	data=JudgeStr(2000);
	if(data)
		ensure=data[9];
	else
		ensure=data[9];
	return ensure;	
}
//åˆ é™¤æ¨¡æ¿ PS_DeletChar
//åŠŸèƒ½:  åˆ é™¤flashæ•°æ®åº“ä¸­æŒ‡å®šIDå·å¼€å§‹çš„Nä¸ªæŒ‡çº¹æ¨¡æ¿
//å‚æ•°:  PageID(æŒ‡çº¹åº“æ¨¡æ¿å·)ï¼ŒNåˆ é™¤çš„æ¨¡æ¿ä¸ªæ•°ã€‚
//è¯´æ˜:  æ¨¡å—è¿”å›ç¡®è®¤å­—
//uint8_t PS_DeletChar(uint16_t PageID,uint16_t N)
//{
//	uint16_t temp;
//  uint8_t  ensure;
//	uint8_t  *data;
//	SendHead();
//	SendAddr();
//	SendFlag(0x01);//ÃüÁî°ü±êÊ¶
//	SendLength(0x07);
//	Sendcmd(0x0C);
//	MYUSART_SendData(PageID>>8);
//	MYUSART_SendData(PageID);
//	MYUSART_SendData(N>>8);
//	MYUSART_SendData(N);
//	temp = 0x01+0x07+0x0C
//	+(PageID>>8)+(uint8_t)PageID
//	+(N>>8)+(uint8_t)N;
//	SendCheck(temp);
//	data=JudgeStr(2000);
//	if(data)
//		ensure=data[9];
//	else
//		ensure=0xff;
//	return ensure;
//}
//æ¸…ç©ºæŒ‡çº¹åº“ PS_Empty
//åŠŸèƒ½:  åˆ é™¤flashæ•°æ®åº“ä¸­æ‰€æœ‰æŒ‡çº¹æ¨¡æ¿
//å‚æ•°:  æ— 
//è¯´æ˜:  æ¨¡å—è¿”å›ç¡®è®¤å­—
uint8_t PS_Empty(void)
{
	uint16_t temp;
  uint8_t  ensure;
	uint8_t  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//ÃüÁî°ü±êÊ¶
	SendLength(0x03);
	Sendcmd(0x0D);
	temp = 0x01+0x03+0x0D;
	SendCheck(temp);
	data=JudgeStr(2000);
	if(data)
		ensure=data[9];
	else
		ensure=0xff;
	return ensure;
}
//Ğ´ÏµÍ³¼Ä´æÆ÷ PS_WriteReg
//¹¦ÄÜ:  Ğ´Ä£¿é¼Ä´æÆ÷
//²ÎÊı:  ¼Ä´æÆ÷ĞòºÅRegNum:4\5\6
//ËµÃ÷:  Ä£¿é·µ»ØÈ·ÈÏ×Ö
//uint8_t PS_WriteReg(uint8_t RegNum,uint8_t DATA)
//{
//	uint16_t temp;
//  uint8_t  ensure;
//	uint8_t  *data;
//	SendHead();
//	SendAddr();
//	SendFlag(0x01);//ÃüÁî°ü±êÊ¶
//	SendLength(0x05);
//	Sendcmd(0x0E);
//	MYUSART_SendData(RegNum);
//	MYUSART_SendData(DATA);
//	temp = RegNum+DATA+0x01+0x05+0x0E;
//	SendCheck(temp);
//	data=JudgeStr(2000);
//	if(data)
//		ensure=data[9];
//	else
//		ensure=0xff;
//	if(ensure==0)
//		printf("\r\nÉèÖÃ²ÎÊı³É¹¦£¡");
//	else
//		//printf("\r\n%s",EnsureMessage(ensure));
//	return ensure;
//}
//¶ÁÏµÍ³»ù±¾²ÎÊı PS_ReadSysPara
//¹¦ÄÜ:  ¶ÁÈ¡Ä£¿éµÄ»ù±¾²ÎÊı£¨²¨ÌØÂÊ£¬°ü´óĞ¡µÈ)
//²ÎÊı:  ÎŞ
//ËµÃ÷:  Ä£¿é·µ»ØÈ·ÈÏ×Ö + »ù±¾²ÎÊı£¨16bytes£©
//uint8_t PS_ReadSysPara(SysPara *p)
//{
//	uint16_t temp;
//  uint8_t  ensure;
//	uint8_t  *data;
//	SendHead();
//	SendAddr();
//	SendFlag(0x01);//ÃüÁî°ü±êÊ¶
//	SendLength(0x03);
//	Sendcmd(0x0F);
//	temp = 0x01+0x03+0x0F;
//	SendCheck(temp);
//	data=JudgeStr(1000);
//	if(data)
//	{
//		ensure = data[9];
//		p->PS_max = (data[14]<<8)+data[15];
//		p->PS_level = data[17];
//		p->PS_addr=(data[18]<<24)+(data[19]<<16)+(data[20]<<8)+data[21];
//		p->PS_size = data[23];
//		p->PS_N = data[25];
//	}		
//	else
//		ensure=0xff;
//	if(ensure==0x00)
//	{
//		printf("\r\nÄ£¿é×î´óÖ¸ÎÆÈİÁ¿=%d",p->PS_max);
//		printf("\r\n¶Ô±ÈµÈ¼¶=%d",p->PS_level);
//		printf("\r\nµØÖ·=%x",p->PS_addr);
//		printf("\r\n²¨ÌØÂÊ=%d",p->PS_N*9600);
//	}
//	else 
//			printf("\r\n%s",EnsureMessage(ensure));
//	return ensure;
//}
//ÉèÖÃÄ£¿éµØÖ· PS_SetAddr
//¹¦ÄÜ:  ÉèÖÃÄ£¿éµØÖ·
//²ÎÊı:  PS_addr
//ËµÃ÷:  Ä£¿é·µ»ØÈ·ÈÏ×Ö
//uint8_t PS_SetAddr(uint32_t PS_addr)
//{
//	uint16_t temp;
//  uint8_t  ensure;
//	uint8_t  *data;
//	SendHead();
//	SendAddr();
//	SendFlag(0x01);//ÃüÁî°ü±êÊ¶
//	SendLength(0x07);
//	Sendcmd(0x15);
//	MYUSART_SendData(PS_addr>>24);
//	MYUSART_SendData(PS_addr>>16);
//	MYUSART_SendData(PS_addr>>8);
//	MYUSART_SendData(PS_addr);
//	temp = 0x01+0x07+0x15
//	+(uint8_t)(PS_addr>>24)+(uint8_t)(PS_addr>>16)
//	+(uint8_t)(PS_addr>>8) +(uint8_t)PS_addr;				
//	SendCheck(temp);
//	AS608Addr=PS_addr;//·¢ËÍÍêÖ¸Áî£¬¸ü»»µØÖ·
//  data=JudgeStr(2000);
//	if(data)
//		ensure=data[9];
//	else
//		ensure=0xff;	
//		AS608Addr = PS_addr;
//	if(ensure==0x00)
//		//printf("\r\nÉèÖÃµØÖ·³É¹¦£¡");
//	else
//		//printf("\r\n%s",EnsureMessage(ensure));
//	return ensure;
//}
//¹¦ÄÜ£º Ä£¿éÄÚ²¿ÎªÓÃ»§¿ª±ÙÁË256bytesµÄFLASH¿Õ¼äÓÃÓÚ´æÓÃ»§¼ÇÊÂ±¾,
//	¸Ã¼ÇÊÂ±¾Âß¼­ÉÏ±»·Ö³É 16 ¸öÒ³¡£
//²ÎÊı:  NotePageNum(0~15),Byte32(ÒªĞ´ÈëÄÚÈİ£¬32¸ö×Ö½Ú)
//ËµÃ÷:  Ä£¿é·µ»ØÈ·ÈÏ×Ö
//uint8_t PS_WriteNotepad(uint8_t NotePageNum,uint8_t *Byte32)
//{
//	uint16_t temp;
//  uint8_t  ensure,i;
//	uint8_t  *data;
//	SendHead();
//	SendAddr();
//	SendFlag(0x01);//ÃüÁî°ü±êÊ¶
//	SendLength(36);
//	Sendcmd(0x18);
//	MYUSART_SendData(NotePageNum);
//	for(i=0;i<32;i++)
//	 {
//		 MYUSART_SendData(Byte32[i]);
//		 temp += Byte32[i];
//	 }
//  temp =0x01+36+0x18+NotePageNum+temp;
//	SendCheck(temp);
//  data=JudgeStr(2000);
//	if(data)
//		ensure=data[9];
//	else
//		ensure=0xff;
//	return ensure;
//}
//¶Á¼ÇÊÂPS_ReadNotepad
//¹¦ÄÜ£º  ¶ÁÈ¡FLASHÓÃ»§ÇøµÄ128bytesÊı¾İ
//²ÎÊı:  NotePageNum(0~15)
//ËµÃ÷:  Ä£¿é·µ»ØÈ·ÈÏ×Ö+ÓÃ»§ĞÅÏ¢
//uint8_t PS_ReadNotepad(uint8_t NotePageNum,uint8_t *Byte32)
//{
//	uint16_t temp;
//  uint8_t  ensure,i;
//	uint8_t  *data;
//	SendHead();
//	SendAddr();
//	SendFlag(0x01);//ÃüÁî°ü±êÊ¶
//	SendLength(0x04);
//	Sendcmd(0x19);
//	MYUSART_SendData(NotePageNum);
//	temp = 0x01+0x04+0x19+NotePageNum;
//	SendCheck(temp);
//  data=JudgeStr(2000);
//	if(data)
//	{
//		ensure=data[9];
//		for(i=0;i<32;i++)
//		{
//			Byte32[i]=data[10+i];
//		}
//	}
//	else
//		ensure=0xff;
//	return ensure;
//}
//é«˜é€Ÿæœç´¢PS_HighSpeedSearch
//åŠŸèƒ½ï¼šä»¥ CharBuffer1æˆ–CharBuffer2ä¸­çš„ç‰¹å¾æ–‡ä»¶é«˜é€Ÿæœç´¢æ•´ä¸ªæˆ–éƒ¨åˆ†æŒ‡çº¹åº“ã€‚
//		  è‹¥æœç´¢åˆ°ï¼Œåˆ™è¿”å›é¡µç ,è¯¥æŒ‡ä»¤å¯¹äºçš„ç¡®å­˜åœ¨äºæŒ‡çº¹åº“ä¸­ ï¼Œä¸”ç™»å½•æ—¶è´¨é‡
//		  å¾ˆå¥½çš„æŒ‡çº¹ï¼Œä¼šå¾ˆå¿«ç»™å‡ºæœç´¢ç»“æœã€‚
//å‚æ•°:  BufferIDï¼Œ StartPage(èµ·å§‹é¡µ)ï¼ŒPageNumï¼ˆé¡µæ•°ï¼‰
//è¯´æ˜:  æ¨¡å—è¿”å›ç¡®è®¤å­—+é¡µç ï¼ˆç›¸é…æŒ‡çº¹æ¨¡æ¿ï¼‰
uint8_t PS_HighSpeedSearch(uint8_t BufferID,uint16_t StartPage,uint16_t PageNum,SearchResult *p)
{
	uint16_t temp;
  uint8_t  ensure;
	uint8_t  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//ÃüÁî°ü±êÊ¶
	SendLength(0x08);
    Sendcmd(0x1b);//AS608_MODEL
   
	MYUSART_SendData(BufferID);
	MYUSART_SendData(StartPage>>8);
	MYUSART_SendData(StartPage);
	MYUSART_SendData(PageNum>>8);
	MYUSART_SendData(PageNum);
	temp = 0x01+0x08+0x1b+BufferID
	+(StartPage>>8)+(uint8_t)StartPage
	+(PageNum>>8)+(uint8_t)PageNum;
	SendCheck(temp);
	data=JudgeStr(2000);
 	if(data)
	{
		ensure=data[9];
		//p->pageID 	=(data[10]<<8) +data[11];
        ArraypageID[16]=(data[10]<<8) +data[11];
		//p->mathscore=(data[12]<<8) +data[13];
        Arraymathscore[16]=(data[12]<<8) +data[13];
	}
	else
		 ensure=data[9]; //ensure=0xff;
   
	return ensure;
}
//¶ÁÓĞĞ§Ä£°å¸öÊı PS_ValidTempleteNum
//¹¦ÄÜ£º¶ÁÓĞĞ§Ä£°å¸öÊı
//²ÎÊı: ÎŞ
//ËµÃ÷: Ä£¿é·µ»ØÈ·ÈÏ×Ö+ÓĞĞ§Ä£°å¸öÊıValidN
uint8_t PS_ValidTempleteNum(uint16_t *ValidN)
{
	uint16_t temp;
  uint8_t  ensure;
	uint8_t  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//ÃüÁî°ü±êÊ¶
	SendLength(0x03);
	Sendcmd(0x1d);
	temp = 0x01+0x03+0x1d;
	SendCheck(temp);
  data=JudgeStr(2000);
	if(data)
	{
		ensure=data[9];
		*ValidN = (data[10]<<8) +data[11];
	}		
	else
		ensure=0xff;
	
//	if(ensure==0x00)
//	{
//		//printf("\r\nÓĞĞ§Ö¸ÎÆ¸öÊı=%d",(data[10]<<8)+data[11]);
//	}
//	else
//		//printf("\r\n%s",EnsureMessage(ensure));
	return ensure;
}
//ÓëAS608ÎÕÊÖ PS_HandShake
//²ÎÊı: PS_AddrµØÖ·Ö¸Õë
//ËµÃ÷: Ä£¿é·µĞÂµØÖ·£¨ÕıÈ·µØÖ·£©	
//uint8_t PS_HandShake(uint32_t *PS_Addr)
//{
//	SendHead();
//	SendAddr();
//	MYUSART_SendData(0X01);
//	MYUSART_SendData(0X00);
//	MYUSART_SendData(0X00);	
//	delay_ms(200);
//	if(USART2_RX_STA&0X8000)//½ÓÊÕµ½Êı¾İ
//	{		
//		if(//ÅĞ¶ÏÊÇ²»ÊÇÄ£¿é·µ»ØµÄÓ¦´ğ°ü				
//					USART2_RX_BUF[0]==0XEF
//				&&USART2_RX_BUF[1]==0X01
//				&&USART2_RX_BUF[6]==0X07
//			)
//			{
//				*PS_Addr=(USART2_RX_BUF[2]<<24) + (USART2_RX_BUF[3]<<16)
//								+(USART2_RX_BUF[4]<<8) + (USART2_RX_BUF[5]);
//				USART2_RX_STA=0;
//				return 0;
//			}
//		USART2_RX_STA=0;					
//	}
//	return 1;		
//}



//void ShowErrMessage(uint8_t ensure)
//{
//	EnsureMessage(ensure);
//}

/**********************************************************************
	*
	*Function Name:void Add_FR(void)
	*Function : å½•æŒ‡çº¹
	*Iinput Ref: NO
	*Return Ref:NO
	*
***********************************************************************/
void Add_FR(void)
{
	uint8_t ensure ,processnum=0;
	static uint8_t genChar,genModel,i;
	while(syspara_t.PS_wakeup_flag==1)
	{

       run_t.gTimer_8s=0;

		switch(processnum){

		case 0:
		i++;
		ensure_getImage=PS_GetImage();
		if(ensure_getImage==0x00) 
		{
			ensure=PS_GenChar(CharBuffer1);//ç”Ÿæˆç‰¹å¾
			if(ensure==0x00)
			{
						
				//run_t.buzzer_fail_sound_flag=0;
				//run_t.buzzer_key_sound_flag =1;
				i=0;	
				genChar= 1;
				processnum=1;//è·³åˆ°ç¬¬äºŒæ­¥						
			}
			else{

			i=0;
			genChar= 0;
			processnum=3;//è·³åˆ°ç¬¬3æ­¥	

			}						
				
		}
        else{
            i=0;
			genChar= 0;
			processnum=3;//è·³åˆ°ç¬¬3æ­¥	

        
        }
	
	   break;


	  case 1:
      
	      ensure_regModel = PS_RegModel();// generate FP template
	      if(ensure_regModel == 0x00)
	      {
	        genModel = 1;            
	        processnum = 2; //generate FP template is success 
	      }
	      else
	      {
	        genModel = 0;  
	        processnum = 3;
	        
	      }
      
      break;

	  case 2:

	
	  	
          syspara_t.PS_save_numbers = AT24CXX_ReadOneByte(EEPROM_AS608Addr);
		  HAL_Delay(5);
          syspara_t.PS_save_numbers++;//maximum number is 40
		  ensure_storeChar = PS_StoreChar(CharBuffer1, (syspara_t.PS_save_numbers)); //save template 
	      if(ensure_storeChar  ==0x00)
	      {
	       
	        syspara_t.PS_login_success=1;
			syspara_t.PS_login_times++;   //FP login four times exit
			if(syspara_t.PS_login_times ==4){
				i=0;
				syspara_t.PS_login_times=0;
				syspara_t.PS_save_numbers=0; //maximum number is 40
			    syspara_t.PS_wakeup_flag=0; //exit login this is times 
				run_t.buzzer_fail_sound_flag=0;
				run_t.buzzer_key_sound_flag =0; //WT.EDIT 2022.10.05
				run_t.buzzer_longsound_flag = 1; //buzzer sound long 
                AT24CXX_WriteOneByte(EEPROM_AS608Addr,syspara_t.PS_save_numbers);
				HAL_Delay(5);
				run_t.motor_return_homePosition=0;
				run_t.Confirm_newPassword =0; //WT.EDIT 2022.12.02
				OK_LED_ON(); //WT.EDIT 2022.10.28
				ERR_LED_OFF();
				run_t.gTimer_8s=7;
				 return;

			 }
			 else{
			 	i=0;
			 	AT24CXX_WriteOneByte(EEPROM_AS608Addr,syspara_t.PS_save_numbers);
				HAL_Delay(5);
			    run_t.buzzer_fail_sound_flag=0;
				run_t.buzzer_key_sound_flag =1;
			    run_t.gTimer_8s=0;
				syspara_t.PS_wakeup_flag=0; //exit FP login onece 
				syspara_t.PS_login_success=1;


			 }
	      }
		  else{

			processnum = 3;
		  }
	  
      break;

	  case 3://generate char is fail 
        
		syspara_t.PS_login_success=0;

        syspara_t.PS_login_times++;
		if(syspara_t.PS_login_times ==4){
			i=0;
            processnum=0;
			syspara_t.PS_login_times=0;
		    syspara_t.PS_wakeup_flag=0;
				run_t.buzzer_fail_sound_flag=0;
				run_t.buzzer_key_sound_flag =0; //WT.EDIT 2022.10.05
				run_t.buzzer_longsound_flag = 1; //buzzer sound long
				run_t.motor_return_homePosition=0;
				 run_t.Confirm_newPassword =0; //WT.EDIT 2022.12.02
				OK_LED_ON(); //WT.EDIT 2022.10.28
				ERR_LED_OFF();
				run_t.gTimer_8s=7;
			return;

		}
		else{ //don't has been save FP data

           i=0;
		   processnum = 0;
		syspara_t.PS_login_success=0;
	       syspara_t.PS_wakeup_flag=0; //exit FP login onece 
		   
		    run_t.buzzer_fail_sound_flag=0;
			run_t.buzzer_key_sound_flag =1;
            run_t.gTimer_8s=0;

		}
	  break;

	  }
	if(i> 30) //Â³Â¬Â¹Ã½5Â´ÃÃƒÂ»Ã“ÃÂ°Â´ÃŠÃ–Ã–Â¸Ã”Ã²ÃÃ‹Â³Ã¶
    {
      i=0;
	  run_t.led_blank	=1;
	  syspara_t.PS_exit_login_flag =1;
	  return ;
    }
	
	
}
}
/**********************************************************************
	*
	*Function Name:void press_FR(void)
	*Function : åˆ·æŒ‡çº¹
	*Iinput Ref: NO
	*Return Ref:NO
	*
***********************************************************************/
void Press_ReadFingerprint_Data(void)
{
	SearchResult seach;
	static uint8_t read_at;
	
    read_at = AT24CXX_ReadOneByte(EEPROM_AS608Addr);


	syspara_t.PS_read_template=0;
    syspara_t.ps_serch_getimage=PS_GetImage();
		while(syspara_t.ps_serch_getimage==0x00)//if(ensure==0x00)//è·å–å›¾åƒæˆåŠŸ 
		{	
           
			syspara_t.ps_serch_genchar=PS_GenChar(CharBuffer1);
			if(read_at==1){
				if(syspara_t.ps_serch_genchar==0x00)//Éú³ÉÌØÕ÷³É¹¦ 
				{	 
	                syspara_t.PS_read_template=2;//receive data is 16bytes.
	                if(read_at==1 && run_t.Confirm_newPassword==1){
	                       syspara_t.ps_serach_result=PS_Search(CharBuffer1,0,1,&seach);
	                }
					else{
					    syspara_t.ps_serach_result=PS_Search(CharBuffer1,0,41,&seach);
					}
					
					if(syspara_t.ps_serach_result==0x00)//ËÑË÷³É¹¦
					{				
						syspara_t.PS_check_fp_success =1;//OLED_ShowCH(0,2,"  æŒ‡çº¹éªŒè¯æˆåŠŸ  ");				
						syspara_t.PS_check_fp_fail =0;
						
	                    syspara_t.PS_wakeup_flag=0;
	                     syspara_t.ps_serch_getimage=0xff;
						 
						 run_t.open_lock_fail=0;
					
						  run_t.open_lock_success=1;
						 run_t.Led_OK_flag =1;
						 run_t.Led_ERR_flag=0;
						  syspara_t.PS_wakeup_flag=0;
						// Buzzer_LongSound();
						return ;
					}
					if(syspara_t.ps_serach_result==0x09) 
					{
						syspara_t.PS_check_fp_success =0;
						syspara_t.PS_check_fp_fail =1;//OLED_ShowCH(32,2,"éªŒè¯å¤±è´¥");
	                     syspara_t.PS_wakeup_flag=0;
	                    syspara_t.ps_serch_getimage=0xff;
						
						run_t.open_lock_success =0;
						run_t.open_lock_fail = 1;
						  run_t.Led_OK_flag =0;
						  run_t.Led_ERR_flag=1;
						  syspara_t.PS_wakeup_flag=0;
						//Fail_Buzzer_Sound();
	                    
						return ;
					}		
			 	}
				else{
	                  
					syspara_t.PS_check_fp_success =0;
					syspara_t.PS_check_fp_fail =1;//OLED_ShowCH(32,2,"éªŒè¯å¤±è´¥");
	                 syspara_t.PS_wakeup_flag=0;
	                 syspara_t.ps_serch_getimage=0xff;
					
					 run_t.open_lock_success =0;
					 run_t.open_lock_fail = 1;
						  run_t.Led_OK_flag =0;
						  run_t.Led_ERR_flag=1;
						   syspara_t.PS_wakeup_flag=0;
					// Fail_Buzzer_Sound();
	                return ;

			    }
            }
			else if(read_at==0)
			{
                  
				 syspara_t.ps_serch_getimage=0xff;
				run_t.open_lock_success=1;
				run_t.Led_OK_flag =1;
				run_t.Led_ERR_flag=0;
				 syspara_t.PS_wakeup_flag=0;
                return ;

			}
			
		}
}


/**********************************************************************
	*
	*Function Name:void Del_FR(void)
	*Function : åˆ é™¤æŒ‡çº¹
	*Iinput Ref: NO
	*Return Ref:NO
	*
***********************************************************************/
void Del_FR(void)
{
	uint8_t  ensure;
	ensure=PS_Empty();//æ¸…ç©ºæŒ‡çº¹åº“
	if(ensure==0)
	{
			//OLED_Clear();
			//OLED_ShowCH(0,2," æ¸…ç©ºæŒ‡çº¹åº“æˆåŠŸ ");		
			syspara_t.PS_clear_ps_success=1;
			syspara_t.PS_clear_ps_fail=0;
	}
	else{

        syspara_t.PS_clear_ps_fail=1;
		syspara_t.PS_clear_ps_success=0;
	}
				
}
/**********************************************************************
	*
	*Function Name:void Fingerprint_NewClinet_Login_Fun(void)
	*Function : åˆ é™¤æŒ‡çº¹
	*Iinput Ref: NO
	*Return Ref:NO
	*
***********************************************************************/
void Fingerprint_NewClinet_Login_Fun(void)
{
    static uint8_t state;
  uint8_t ps_getImage=0xff,ps_genChar=0xff,ps_match=0xff,ps_getImage_2=0xff,cnt,STEP_CNT;
 uint8_t ps_genChar_2=0xff,ps_storechar=0xff,ps_regmodel=0xff,del_ensure=0xff;
 uint8_t  ser_getImage=0xff,ser_genChar=0xff,ser_speedSer=0xff,ps_templete=0xff,del_empty=0xff;
 uint16_t Valid_n[1];

 
	switch(state){
			
		   case 0:
				syspara_t.PS_read_template=0;
			   ps_getImage=PS_GetImage();
			  
			  if(ps_getImage==0){
		   
				 ps_genChar =PS_GenChar(CharBuffer1);//ç”Ÿæˆç‰¹å¾
				
				if(ps_genChar==0 ){
					 state=1;
					 STEP_CNT=1;
					 BUZZER_KeySound();
				}
				else {
				   state=0;
					 syspara_t.PS_wakeup_flag = 0;
				
				}
			  }
			 else{
			   state=0;
			   syspara_t.PS_wakeup_flag = 0;
			 }
		  break;
			 
		   case 1:
					   BACKLIGHT_2_OFF() ;
					  HAL_Delay(100);
					  BACKLIGHT_2_ON() ;
					  HAL_Delay(100);
					   BACKLIGHT_2_OFF() ;
					  HAL_Delay(100);
					  BACKLIGHT_2_ON() ;
					  HAL_Delay(100);
			   
				syspara_t.PS_read_template=0;
			   ps_getImage_2=PS_GetImage();
			 
			 if(ps_getImage_2==0){
	   
				 ps_genChar_2=PS_GenChar(CharBuffer2);//????
				 if(ps_genChar_2==0){
					  state=2;
					   STEP_CNT=2;
				  BUZZER_KeySound();
				 }
				 else{
				   state=0;
				  syspara_t.PS_wakeup_flag = 0;
				  
				 }
			 }
			 else{
			   state=0;
			  syspara_t.PS_wakeup_flag = 0;
			 }
		   break;
			 
			 
		   case 2:
				 syspara_t.PS_read_template=1;
				 ps_match=PS_Match();
				if(ps_match ==0){
				 state=3;
					  STEP_CNT=3;
				 BUZZER_KeySound();
				}
				else{
					   //state=0;
					syspara_t.PS_wakeup_flag = 0;
					 state=3;
				
				}
		   
		   break;
		 
	
		   case 3:
				syspara_t.PS_read_template=0;
			   ps_regmodel=PS_RegModel();
		  
			if(ps_regmodel==0){
				state=4;
				  STEP_CNT=4;
				 BUZZER_KeySound();
			}
			 else{
			   state=0;
			   syspara_t.PS_wakeup_flag = 0;
				 
			  
			 }
		   break;
		   
		   case 4:
				
	   
				  ps_storechar=PS_StoreChar(CharBuffer2,syspara_t.PS_login_success);//????
	
				  if(ps_storechar==0){
						STEP_CNT=5;
					  Buzzer_LongSound();
					  syspara_t.PS_login_success++;
					  syspara_t.PS_login_flag=1;		  
					  BACKLIGHT_2_OFF() ;
					  HAL_Delay(500);
					  BACKLIGHT_2_ON() ;
					  HAL_Delay(500);
					  BACKLIGHT_2_OFF() ;
					   HAL_Delay(500);
					  BACKLIGHT_2_ON() ;
					  syspara_t.PS_read_template=1;
					   state= 5;
				   
				   
				   }
				   else{
					state= 0;
				   syspara_t.PS_wakeup_flag = 0;
					 STEP_CNT=0;
				   }
			break;
				   
		   case 5:
			   
			 ps_templete = PS_ValidTempleteNum(Valid_n);//Â¶ÃÂ¿Ã¢Ã–Â¸ÃÃ†Â¸Ã¶ÃŠÃ½
				 syspara_t.PS_wakeup_flag = 0;
		   state=0;
			 STEP_CNT=0;
		   
		   break;
			   }
		   
		
 }


/**********************************************************************
	*
	*Function Name:void Del_FR(void)
	*Function : åˆ é™¤æŒ‡çº¹
	*Iinput Ref: NO
	*Return Ref:NO
	*
***********************************************************************/




