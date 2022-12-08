//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK 战舰V3 STM32开发板
//ATK-AS608指纹识别模块驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2016/3/29
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
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
uint16_t ArraypageID ;
uint16_t Arraymathscore;

uint8_t STEP_CNT;
uint8_t state;
  
uint8_t ensure_getImage,ensure_regModel, ensure_storeChar;

SysPara AS608Para;//鎸囩汗妯″潡AS608鍙傛暟


uint32_t AS608Addr = 0XFFFFFFFF; //默认

uint16_t EEPROM_AS608Addr = 0x60;

//初始化PA6为下拉输入		    
//读摸出感应状态(触摸感应时输出高电平信号)
void PS_StaGPIO_Init(void)
{   
//  GPIO_InitTypeDef  GPIO_InitStructure;
//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//使能GPIOA时钟
//  //初始化读状态引脚GPIOA
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;//输入下拉模式
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//50MHz
//  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIO	
}
//send data one byte
static void MYUSART_SendData(uint8_t data)
{
	while((USART1->ISR&0X40)==0); 
	USART1->TDR = data;
}
//发送包头
static void SendHead(void)
{
	MYUSART_SendData(0xEF);
	MYUSART_SendData(0x01);
}
//发送地址
static void SendAddr(void)
{
	MYUSART_SendData(AS608Addr>>24);
	MYUSART_SendData(AS608Addr>>16);
	MYUSART_SendData(AS608Addr>>8);
	MYUSART_SendData(AS608Addr);
}
//发送包标识,
static void SendFlag(uint8_t flag)
{
	MYUSART_SendData(flag);
}
//发送包长度
static void SendLength(int length)
{
	MYUSART_SendData(length>>8);
	MYUSART_SendData(length);
}
//发送指令码
static void Sendcmd(uint8_t cmd)
{
	MYUSART_SendData(cmd);
}
//发送校验和
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
//鍒ゆ柇涓柇鎺ユ敹鐨勬暟缁勬湁娌℃湁搴旂瓟鍖�
//waittime涓虹瓑寰呬腑鏂帴鏀舵暟鎹殑鏃堕棿锛堝崟浣�1ms锛�
//杩斿洖鍊硷細鏁版嵁鍖呴鍦板潃
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
		if(USART1_RX_STA&0X8000 || syspara_t.uart1_rx_data==1)//陆脫脢脮碌陆脪禄麓脦脢媒戮脻
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
//褰曞叆鍥惧儚 PS_GetImage
//鍔熻兘:鎺㈡祴鎵嬫寚锛屾帰娴嬪埌鍚庡綍鍏ユ寚绾瑰浘鍍忓瓨浜嶪mageBuffer銆� 
//妯″潡杩斿洖纭瀛�
uint8_t PS_GetImage(void)
{
  uint16_t temp;
  uint8_t  ensure;
	uint8_t  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//鍛戒护鍖呮爣璇�
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
//鐢熸垚鐗瑰緛 PS_GenChar
//鍔熻兘:灏咺mageBuffer涓殑鍘熷鍥惧儚鐢熸垚鎸囩汗鐗瑰緛鏂囦欢瀛樹簬CharBuffer1鎴朇harBuffer2			 
//鍙傛暟:BufferID --> charBuffer1:0x01	charBuffer1:0x02												
//妯″潡杩斿洖纭瀛�
uint8_t PS_GenChar(uint8_t BufferID)
{
	uint16_t temp;
  uint8_t  ensure;
	uint8_t  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//命令包标识
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
//绮剧‘姣斿涓ゆ灇鎸囩汗鐗瑰緛 PS_Match
//鍔熻兘:绮剧‘姣斿CharBuffer1 涓嶤harBuffer2 涓殑鐗瑰緛鏂囦欢 
//妯″潡杩斿洖纭瀛�
uint8_t PS_Match(void)
{
	uint16_t temp;
  uint8_t  ensure;
	uint8_t  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//命令包标识
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
//鎼滅储鎸囩汗 PS_Search
//鍔熻兘:浠harBuffer1鎴朇harBuffer2涓殑鐗瑰緛鏂囦欢鎼滅储鏁翠釜鎴栭儴鍒嗘寚绾瑰簱.鑻ユ悳绱㈠埌锛屽垯杩斿洖椤电爜銆�			
//鍙傛暟:  BufferID @ref CharBuffer1	CharBuffer2
//璇存槑:  妯″潡杩斿洖纭瀛楋紝椤电爜锛堢浉閰嶆寚绾规ā鏉匡級
uint8_t PS_Search(uint8_t BufferID,uint16_t StartPage,uint16_t PageNum,SearchResult *p)
{
	uint16_t temp;
  uint8_t  ensure;
	uint8_t  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//命令包标识
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
//鍚堝苟鐗瑰緛锛堢敓鎴愭ā鏉匡級PS_RegModel
//鍔熻兘:灏咰harBuffer1涓嶤harBuffer2涓殑鐗瑰緛鏂囦欢鍚堝苟鐢熸垚 妯℃澘,缁撴灉瀛樹簬CharBuffer1涓嶤harBuffer2	
//璇存槑:  妯″潡杩斿洖纭瀛�
uint8_t PS_RegModel(void)
{
	uint16_t temp;
  uint8_t  ensure;
	uint8_t  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//命令包标识
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
//鍌ㄥ瓨妯℃澘 PS_StoreChar
//鍔熻兘:灏� CharBuffer1 鎴� CharBuffer2 涓殑妯℃澘鏂囦欢瀛樺埌 PageID 鍙穎lash鏁版嵁搴撲綅缃€�			
//鍙傛暟:  BufferID @ref charBuffer1:0x01	charBuffer1:0x02
//       PageID锛堟寚绾瑰簱浣嶇疆鍙凤級
//璇存槑:  妯″潡杩斿洖纭瀛�

uint8_t PS_StoreChar(uint8_t BufferID,uint16_t PageID)
{
	uint16_t temp;
  	uint8_t  ensure;
	uint8_t  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//命令包标识
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
//鍒犻櫎妯℃澘 PS_DeletChar
//鍔熻兘:  鍒犻櫎flash鏁版嵁搴撲腑鎸囧畾ID鍙峰紑濮嬬殑N涓寚绾规ā鏉�
//鍙傛暟:  PageID(鎸囩汗搴撴ā鏉垮彿)锛孨鍒犻櫎鐨勬ā鏉夸釜鏁般€�
//璇存槑:  妯″潡杩斿洖纭瀛�
//uint8_t PS_DeletChar(uint16_t PageID,uint16_t N)
//{
//	uint16_t temp;
//  uint8_t  ensure;
//	uint8_t  *data;
//	SendHead();
//	SendAddr();
//	SendFlag(0x01);//命令包标识
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
//娓呯┖鎸囩汗搴� PS_Empty
//鍔熻兘:  鍒犻櫎flash鏁版嵁搴撲腑鎵€鏈夋寚绾规ā鏉�
//鍙傛暟:  鏃�
//璇存槑:  妯″潡杩斿洖纭瀛�
uint8_t PS_Empty(void)
{
	uint16_t temp;
  uint8_t  ensure;
	uint8_t  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//命令包标识
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
//写系统寄存器 PS_WriteReg
//功能:  写模块寄存器
//参数:  寄存器序号RegNum:4\5\6
//说明:  模块返回确认字
//uint8_t PS_WriteReg(uint8_t RegNum,uint8_t DATA)
//{
//	uint16_t temp;
//  uint8_t  ensure;
//	uint8_t  *data;
//	SendHead();
//	SendAddr();
//	SendFlag(0x01);//命令包标识
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
//		printf("\r\n设置参数成功！");
//	else
//		//printf("\r\n%s",EnsureMessage(ensure));
//	return ensure;
//}
//读系统基本参数 PS_ReadSysPara
//功能:  读取模块的基本参数（波特率，包大小等)
//参数:  无
//说明:  模块返回确认字 + 基本参数（16bytes）
//uint8_t PS_ReadSysPara(SysPara *p)
//{
//	uint16_t temp;
//  uint8_t  ensure;
//	uint8_t  *data;
//	SendHead();
//	SendAddr();
//	SendFlag(0x01);//命令包标识
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
//		printf("\r\n模块最大指纹容量=%d",p->PS_max);
//		printf("\r\n对比等级=%d",p->PS_level);
//		printf("\r\n地址=%x",p->PS_addr);
//		printf("\r\n波特率=%d",p->PS_N*9600);
//	}
//	else 
//			printf("\r\n%s",EnsureMessage(ensure));
//	return ensure;
//}
//设置模块地址 PS_SetAddr
//功能:  设置模块地址
//参数:  PS_addr
//说明:  模块返回确认字
//uint8_t PS_SetAddr(uint32_t PS_addr)
//{
//	uint16_t temp;
//  uint8_t  ensure;
//	uint8_t  *data;
//	SendHead();
//	SendAddr();
//	SendFlag(0x01);//命令包标识
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
//	AS608Addr=PS_addr;//发送完指令，更换地址
//  data=JudgeStr(2000);
//	if(data)
//		ensure=data[9];
//	else
//		ensure=0xff;	
//		AS608Addr = PS_addr;
//	if(ensure==0x00)
//		//printf("\r\n设置地址成功！");
//	else
//		//printf("\r\n%s",EnsureMessage(ensure));
//	return ensure;
//}
//功能： 模块内部为用户开辟了256bytes的FLASH空间用于存用户记事本,
//	该记事本逻辑上被分成 16 个页。
//参数:  NotePageNum(0~15),Byte32(要写入内容，32个字节)
//说明:  模块返回确认字
//uint8_t PS_WriteNotepad(uint8_t NotePageNum,uint8_t *Byte32)
//{
//	uint16_t temp;
//  uint8_t  ensure,i;
//	uint8_t  *data;
//	SendHead();
//	SendAddr();
//	SendFlag(0x01);//命令包标识
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
//读记事PS_ReadNotepad
//功能：  读取FLASH用户区的128bytes数据
//参数:  NotePageNum(0~15)
//说明:  模块返回确认字+用户信息
//uint8_t PS_ReadNotepad(uint8_t NotePageNum,uint8_t *Byte32)
//{
//	uint16_t temp;
//  uint8_t  ensure,i;
//	uint8_t  *data;
//	SendHead();
//	SendAddr();
//	SendFlag(0x01);//命令包标识
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
//楂橀€熸悳绱S_HighSpeedSearch
//鍔熻兘锛氫互 CharBuffer1鎴朇harBuffer2涓殑鐗瑰緛鏂囦欢楂橀€熸悳绱㈡暣涓垨閮ㄥ垎鎸囩汗搴撱€�
//		  鑻ユ悳绱㈠埌锛屽垯杩斿洖椤电爜,璇ユ寚浠ゅ浜庣殑纭瓨鍦ㄤ簬鎸囩汗搴撲腑 锛屼笖鐧诲綍鏃惰川閲�
//		  寰堝ソ鐨勬寚绾癸紝浼氬緢蹇粰鍑烘悳绱㈢粨鏋溿€�
//鍙傛暟:  BufferID锛� StartPage(璧峰椤�)锛孭ageNum锛堥〉鏁帮級
//璇存槑:  妯″潡杩斿洖纭瀛�+椤电爜锛堢浉閰嶆寚绾规ā鏉匡級
uint8_t PS_HighSpeedSearch(uint8_t BufferID,uint16_t StartPage,uint16_t PageNum,SearchResult *p)
{
	uint16_t temp;
  uint8_t  ensure;
	uint8_t  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//命令包标识
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
        ArraypageID=(data[10]<<8) +data[11];
		//p->mathscore=(data[12]<<8) +data[13];
        Arraymathscore=(data[12]<<8) +data[13];
	}
	else
		 ensure=data[9]; //ensure=0xff;
   
	return ensure;
}
//读有效模板个数 PS_ValidTempleteNum
//功能：读有效模板个数
//参数: 无
//说明: 模块返回确认字+有效模板个数ValidN
uint8_t PS_ValidTempleteNum(uint16_t *ValidN)
{
	uint16_t temp;
  uint8_t  ensure;
	uint8_t  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//命令包标识
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
//		//printf("\r\n有效指纹个数=%d",(data[10]<<8)+data[11]);
//	}
//	else
//		//printf("\r\n%s",EnsureMessage(ensure));
	return ensure;
}
//与AS608握手 PS_HandShake
//参数: PS_Addr地址指针
//说明: 模块返新地址（正确地址）	
//uint8_t PS_HandShake(uint32_t *PS_Addr)
//{
//	SendHead();
//	SendAddr();
//	MYUSART_SendData(0X01);
//	MYUSART_SendData(0X00);
//	MYUSART_SendData(0X00);	
//	delay_ms(200);
//	if(USART2_RX_STA&0X8000)//接收到数据
//	{		
//		if(//判断是不是模块返回的应答包				
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
	*Function Name:void press_FR(void)
	*Function : read fingerprint data 
	*Iinput Ref: NO
	*Return Ref:NO
	*
***********************************************************************/
void Press_ReadFingerprint_Data(void)
{
	SearchResult seach;

	if(run_t.Confirm_newPassword==1){
	  run_t.gTimer_8s=0;
	  syspara_t.PS_wakeup_flag=1;
      syspara_t.ps_readEeprom_data = AT24CXX_ReadOneByte(EEPROM_AS608Addr);
	  if(syspara_t.ps_readEeprom_data ==0){ //the first new fingerprint must be is administrator password "1234"
         syspara_t.ps_thefist_input_fp =1; //the first input fingerprint administrator password
         run_t.open_lock_success =0;
		 run_t.open_lock_fail = 0;
		 run_t.Led_OK_flag =0;
		  run_t.Led_ERR_flag=0;
	  }
	  else{
	    syspara_t.ps_thefist_input_fp =0; //has been an administrator of passsword
	     run_t.open_lock_success =0;
		 run_t.open_lock_fail = 0;
		 run_t.Led_OK_flag =0;
		  run_t.Led_ERR_flag=0;

	 }
	}
	else{
	   syspara_t.ps_readEeprom_data = AT24CXX_ReadOneByte(EEPROM_AS608Addr);
	

	}
	
    //fingerprint open lock doing 
    if(run_t.Confirm_newPassword==0 && run_t.panel_lock==0){
		syspara_t.ps_thefist_input_fp =0;
		syspara_t.PS_read_template=0;
	    syspara_t.ps_serch_getimage=PS_GetImage();
		while(syspara_t.ps_serch_getimage==0x00)//if(ensure==0x00)//鑾峰彇鍥惧儚鎴愬姛 
		{	
           
			syspara_t.ps_serch_genchar=PS_GenChar(CharBuffer1);
			if( syspara_t.ps_readEeprom_data >0){
				if(syspara_t.ps_serch_genchar==0x00)//生成特征成功 
				{	 
	                syspara_t.PS_read_template=2;//receive data is 16bytes by USART1 port
	            
					syspara_t.ps_serach_result=PS_Search(CharBuffer1,0,41,&seach);
					
					
					if(syspara_t.ps_serach_result==0x00)//搜索成功
					{				
						syspara_t.PS_check_fp_success =1;//OLED_ShowCH(0,2,"  鎸囩汗楠岃瘉鎴愬姛  ");				
						syspara_t.PS_check_fp_fail =0;
						
	                    syspara_t.PS_wakeup_flag=0;
	                     syspara_t.ps_serch_getimage=0xff;
						 
						 run_t.open_lock_fail=0;
					
						  run_t.open_lock_success=1;
						 run_t.Led_OK_flag =1;
						 run_t.Led_ERR_flag=0;
						  syspara_t.PS_wakeup_flag=0;
						
						return ;
					}
					if(syspara_t.ps_serach_result==0x09) 
					{
						syspara_t.PS_check_fp_success =0;
						syspara_t.PS_check_fp_fail =1;//OLED_ShowCH(32,2,"楠岃瘉澶辫触");
	                     syspara_t.PS_wakeup_flag=0;
	                    syspara_t.ps_serch_getimage=0xff;
						
						run_t.open_lock_success =0;
						run_t.open_lock_fail = 1;
						  run_t.Led_OK_flag =0;
						  run_t.Led_ERR_flag=1;
						  syspara_t.PS_wakeup_flag=0;
						  run_t.error_times ++ ;
	                    
						return ;
					}		
			 	}
				else{
	                  
					syspara_t.PS_check_fp_success =0;
					syspara_t.PS_check_fp_fail =1;//OLED_ShowCH(32,2,"楠岃瘉澶辫触");
	                 syspara_t.PS_wakeup_flag=0;
	                 syspara_t.ps_serch_getimage=0xff;
					
					 run_t.open_lock_success =0;
					 run_t.open_lock_fail = 1;
						  run_t.Led_OK_flag =0;
						  run_t.Led_ERR_flag=1;
						   syspara_t.PS_wakeup_flag=0;
						   
					
	                return ;

			    }
            }
			else if(syspara_t.ps_readEeprom_data==0 )
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

    //judge input fingerprint error times
	 if(run_t.error_times > 4 ){ //OVER 5 error  times auto lock touchkey 60 s
        run_t.gTimer_10s_start=0;//WT.EDIT 2022.09.20
        run_t.gTimer_input_error_times_60s =0;
        run_t.panel_lock=1;
		run_t.gTimer_8s=0;
		}

	//New fingerprint intpu by save new fingerprint data statement
    if(run_t.Confirm_newPassword==1 &&  syspara_t.ps_thefist_input_fp ==0 && run_t.inputNewPassword_Enable==0){//don't the first input FP.and to store has FP data

		syspara_t.ps_serch_getimage=PS_GetImage();
		while(syspara_t.ps_serch_getimage==0x00)//if(ensure==0x00)//鑾峰彇鍥惧儚鎴愬姛 
		{	
           
			syspara_t.ps_serch_genchar=PS_GenChar(CharBuffer1);
			if(syspara_t.ps_readEeprom_data ==1){
				if(syspara_t.ps_serch_genchar==0x00)//生成特征成功 
				{	 
	                syspara_t.PS_read_template=2;//receive data is 16bytes.
	            
					syspara_t.ps_serach_result=PS_Search(CharBuffer1,0,1,&seach);
					
					
					if(syspara_t.ps_serach_result==0x00)//搜索成功
					{				
						syspara_t.PS_check_fp_success =1;//OLED_ShowCH(0,2,"  鎸囩汗楠岃瘉鎴愬姛  ");	
                        run_t.open_lock_success=1;
						run_t.open_lock_fail = 0;
						 syspara_t.ps_serch_getimage=0xff;
						return ;
						
					}
					else{
						run_t.open_lock_success=0;
						run_t.open_lock_fail = 1;
						syspara_t.ps_serch_getimage=0xff;
						return ;
					}
				}
		        else{
                    run_t.open_lock_success=0;
                    run_t.open_lock_fail = 1;
                    syspara_t.ps_serch_getimage=0xff;
                    return ;
                }

		}
    }
	
    }

	
}

/**********************************************************************
	*
	*Function Name:void Del_FR(void)
	*Function : 鍒犻櫎鎸囩汗
	*Iinput Ref: NO
	*Return Ref:NO
	*
***********************************************************************/
void Del_FR(void)
{
	uint8_t  ensure;
	ensure=PS_Empty();//娓呯┖鎸囩汗搴�
	if(ensure==0)
	{
			//OLED_Clear();
			//OLED_ShowCH(0,2," 娓呯┖鎸囩汗搴撴垚鍔� ");		
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
	*Function : 鍒犻櫎鎸囩汗
	*Iinput Ref: NO
	*Return Ref:NO
	*
***********************************************************************/
void Fingerprint_NewClinet_Login_Fun(void)
{
   static uint8_t fp_times=0xff,ps_regmodel=0xff,ps_genChar=0xff,ps_storechar=0xff;
   uint8_t ps_getImage;
   run_t.gTimer_8s=0;
    
      //  if(syspara_t.PS_login_times>4)syspara_t.PS_login_times=0;
    
     
	   do{
			
			 switch(syspara_t.PS_login_times){

			 	case 0:

		   syspara_t.PS_read_template=0;
		   ps_getImage=PS_GetImage();
			  
			if(ps_getImage==0){
		   
				 ps_genChar =PS_GenChar(CharBuffer1);//鐢熸垚鐗瑰緛
				
				if(ps_genChar==0 ){
                     if(fp_times != syspara_t.fp_login_key){
                         fp_times = syspara_t.fp_login_key;
				          				syspara_t.PS_login_times++;	
                       
					      					BUZZER_KeySound();
                          HAL_Delay(200);
                        }
					 }
                 }	   
        break; 

        case 1: //input 2 times
	        syspara_t.PS_read_template=0;
			   ps_getImage=PS_GetImage();
				  
				if(ps_getImage==0){
			   
					 ps_genChar =PS_GenChar(CharBuffer1);//鐢熸垚鐗瑰緛
					
					if(ps_genChar==0 ){
	                     if(fp_times != syspara_t.fp_login_key){
	                         fp_times = syspara_t.fp_login_key;
					          				syspara_t.PS_login_times++;	
	                       
						      					BUZZER_KeySound();
	                          HAL_Delay(200);
	                        }
						 }


                     }
        break;  

        case 2: //input 3  times 

         syspara_t.PS_read_template=0;
			   ps_getImage=PS_GetImage();
				  
				if(ps_getImage==0){
			   
					 ps_genChar =PS_GenChar(CharBuffer1);//鐢熸垚鐗瑰緛
					
					if(ps_genChar==0 ){
                 ps_regmodel=PS_RegModel();
                if(ps_regmodel==0){
	                     if(fp_times != syspara_t.fp_login_key){
	                         fp_times = syspara_t.fp_login_key;
					          				syspara_t.PS_login_times++;	
	                       
						      					BUZZER_KeySound();
	                          HAL_Delay(200);
	                        }

                     }


						 }
         }

        break;   

        case 3://input 4 times 

             syspara_t.ps_readEeprom_data = AT24CXX_ReadOneByte(EEPROM_AS608Addr);
	            if(syspara_t.ps_readEeprom_data ==0){
				         ps_storechar=PS_StoreChar(CharBuffer1,1);//administrator of fingerprint
	              }
				        else{
				  	       syspara_t.ps_readEeprom_data = AT24CXX_ReadOneByte(EEPROM_AS608Addr+0X01);
				  	      ps_storechar=PS_StoreChar(CharBuffer1,(syspara_t.ps_readEeprom_data+1));//????
				        }
				  if(ps_storechar==0){
	            if(syspara_t.ps_readEeprom_data < 41){
				 
					
					   run_t.Confirm_newPassword =0;//WT.EIDT 2022.09.12
					   run_t.motor_return_homePosition=0;
						 run_t.inputDeepSleep_times =0; //WT.EDIT 2022.09.20
						run_t.buzzer_key_sound_flag =0; //WT.EDIT 2022.10.05
						run_t.buzzer_longsound_flag =1;//WT.EDIT 2022.10.28
						run_t.inputNewPassword_Enable =0; //WT.EDIT 2022.12.08
						  
					
            OK_LED_ON(); //WT.EDIT 2022.10.28
						ERR_LED_OFF();
            run_t.gTimer_8s=7;
						run_t.login_in_success=1; //WT.EDIT 2022.10.31
						run_t.gTimer_1s=0;//WT.EDIT 2022.10.31
						syspara_t.PS_save_NewFP =0;
					   if(syspara_t.ps_readEeprom_data ==0){
				  	     AT24CXX_WriteOneByte(EEPROM_AS608Addr,0x01);
					   	}
					   else
				   	      AT24CXX_WriteOneByte((EEPROM_AS608Addr+0x01),(syspara_t.ps_readEeprom_data+1));
				       
				  run_t.open_lock_success=0;
          run_t.open_lock_fail = 0;
					ERR_LED_ON();
					OK_LED_OFF(); //WT.EDIT 2022.10.28
		      run_t.led_blank	=1;//OK led blank three times
			  syspara_t.PS_read_template=1;
			  syspara_t.ps_judeg_read_templete_flag = PS_ValidTempleteNum(&syspara_t.ps_read_templete_numbers);//露脕驴芒脰赂脦脝赂枚脢媒
				 
		 
		    }
		    else{//over times 40 is error
		    	 run_t.Confirm_newPassword =0;//WT.EIDT 2022.09.12
		    	run_t.inputNewPassword_Enable =0; //WT.EDIT 2022.09.28
					run_t.BackLight =1;
					OK_LED_OFF(); //WT.EDIT 2022.10.28
					ERR_LED_ON();
					run_t.gTimer_8s=5;//WT.EDIT 2022.11.01
          run_t.open_lock_success=0;
				  run_t.open_lock_fail=1;
				  run_t.Confirm_newPassword =0;  //be save eeprom data flag bit
				  run_t.buzzer_key_sound_flag =0;//WT.EDIT 2022.10.06	
					run_t.buzzer_fail_sound_flag=1; //WT.EDIT 2022.10.06	
					run_t.buzzer_longsound_flag =0;//WT.EDIT 2022.10.19	
		    }
		
			}
      syspara_t.PS_login_times=0;	

        break; 
               
         

   }

             
 }while(syspara_t.PS_login_times==6);
 }