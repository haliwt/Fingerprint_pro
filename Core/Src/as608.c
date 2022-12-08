//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK ս��V3 STM32������
//ATK-AS608ָ��ʶ��ģ����������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2016/3/29
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
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

SysPara AS608Para;//指纹模块AS608参数


uint32_t AS608Addr = 0XFFFFFFFF; //Ĭ��

uint16_t EEPROM_AS608Addr = 0x60;

//��ʼ��PA6Ϊ��������		    
//��������Ӧ״̬(������Ӧʱ����ߵ�ƽ�ź�)
void PS_StaGPIO_Init(void)
{   
//  GPIO_InitTypeDef  GPIO_InitStructure;
//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//ʹ��GPIOAʱ��
//  //��ʼ����״̬����GPIOA
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;//��������ģʽ
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//50MHz
//  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIO	
}
//send data one byte
static void MYUSART_SendData(uint8_t data)
{
	while((USART1->ISR&0X40)==0); 
	USART1->TDR = data;
}
//���Ͱ�ͷ
static void SendHead(void)
{
	MYUSART_SendData(0xEF);
	MYUSART_SendData(0x01);
}
//���͵�ַ
static void SendAddr(void)
{
	MYUSART_SendData(AS608Addr>>24);
	MYUSART_SendData(AS608Addr>>16);
	MYUSART_SendData(AS608Addr>>8);
	MYUSART_SendData(AS608Addr);
}
//���Ͱ���ʶ,
static void SendFlag(uint8_t flag)
{
	MYUSART_SendData(flag);
}
//���Ͱ�����
static void SendLength(int length)
{
	MYUSART_SendData(length>>8);
	MYUSART_SendData(length);
}
//����ָ����
static void Sendcmd(uint8_t cmd)
{
	MYUSART_SendData(cmd);
}
//����У���
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
//判断中断接收的数组有没有应答匄1�7
//waittime为等待中断接收数据的时间（单佄1�71ms＄1�7
//返回值：数据包首地址
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
		if(USART1_RX_STA&0X8000 || syspara_t.uart1_rx_data==1)//½ÓÊÕµ½Ò»´ÎÊý¾Ý
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
//录入图像 PS_GetImage
//功能:探测手指，探测到后录入指纹图像存于ImageBuffer〄1�7 
//模块返回确认孄1�7
uint8_t PS_GetImage(void)
{
  uint16_t temp;
  uint8_t  ensure;
	uint8_t  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//命令包标评1�7
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
//生成特征 PS_GenChar
//功能:将ImageBuffer中的原始图像生成指纹特征文件存于CharBuffer1或CharBuffer2			 
//参数:BufferID --> charBuffer1:0x01	charBuffer1:0x02												
//模块返回确认孄1�7
uint8_t PS_GenChar(uint8_t BufferID)
{
	uint16_t temp;
  uint8_t  ensure;
	uint8_t  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
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
//精确比对两枚指纹特征 PS_Match
//功能:精确比对CharBuffer1 与CharBuffer2 中的特征文件 
//模块返回确认孄1�7
uint8_t PS_Match(void)
{
	uint16_t temp;
  uint8_t  ensure;
	uint8_t  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
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
//搜索指纹 PS_Search
//功能:以CharBuffer1或CharBuffer2中的特征文件搜索整个或部分指纹库.若搜索到，则返回页码〄1�7			
//参数:  BufferID @ref CharBuffer1	CharBuffer2
//说明:  模块返回确认字，页码（相配指纹模板）
uint8_t PS_Search(uint8_t BufferID,uint16_t StartPage,uint16_t PageNum,SearchResult *p)
{
	uint16_t temp;
  uint8_t  ensure;
	uint8_t  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
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
//合并特征（生成模板）PS_RegModel
//功能:将CharBuffer1与CharBuffer2中的特征文件合并生成 模板,结果存于CharBuffer1与CharBuffer2	
//说明:  模块返回确认孄1�7
uint8_t PS_RegModel(void)
{
	uint16_t temp;
  uint8_t  ensure;
	uint8_t  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
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
//储存模板 PS_StoreChar
//功能:射1�7 CharBuffer1 戄1�7 CharBuffer2 中的模板文件存到 PageID 号flash数据库位置��1�7			
//参数:  BufferID @ref charBuffer1:0x01	charBuffer1:0x02
//       PageID（指纹库位置号）
//说明:  模块返回确认孄1�7

uint8_t PS_StoreChar(uint8_t BufferID,uint16_t PageID)
{
	uint16_t temp;
  	uint8_t  ensure;
	uint8_t  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
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
//删除模板 PS_DeletChar
//功能:  删除flash数据库中指定ID号开始的N个指纹模杄1�7
//参数:  PageID(指纹库模板号)，N删除的模板个数��1�7
//说明:  模块返回确认孄1�7
//uint8_t PS_DeletChar(uint16_t PageID,uint16_t N)
//{
//	uint16_t temp;
//  uint8_t  ensure;
//	uint8_t  *data;
//	SendHead();
//	SendAddr();
//	SendFlag(0x01);//�������ʶ
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
//清空指纹庄1�7 PS_Empty
//功能:  删除flash数据库中扢�有指纹模杄1�7
//参数:  旄1�7
//说明:  模块返回确认孄1�7
uint8_t PS_Empty(void)
{
	uint16_t temp;
  uint8_t  ensure;
	uint8_t  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
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
//дϵͳ�Ĵ��� PS_WriteReg
//����:  дģ��Ĵ���
//����:  �Ĵ������RegNum:4\5\6
//˵��:  ģ�鷵��ȷ����
//uint8_t PS_WriteReg(uint8_t RegNum,uint8_t DATA)
//{
//	uint16_t temp;
//  uint8_t  ensure;
//	uint8_t  *data;
//	SendHead();
//	SendAddr();
//	SendFlag(0x01);//�������ʶ
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
//		printf("\r\n���ò����ɹ���");
//	else
//		//printf("\r\n%s",EnsureMessage(ensure));
//	return ensure;
//}
//��ϵͳ�������� PS_ReadSysPara
//����:  ��ȡģ��Ļ��������������ʣ�����С��)
//����:  ��
//˵��:  ģ�鷵��ȷ���� + ����������16bytes��
//uint8_t PS_ReadSysPara(SysPara *p)
//{
//	uint16_t temp;
//  uint8_t  ensure;
//	uint8_t  *data;
//	SendHead();
//	SendAddr();
//	SendFlag(0x01);//�������ʶ
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
//		printf("\r\nģ�����ָ������=%d",p->PS_max);
//		printf("\r\n�Աȵȼ�=%d",p->PS_level);
//		printf("\r\n��ַ=%x",p->PS_addr);
//		printf("\r\n������=%d",p->PS_N*9600);
//	}
//	else 
//			printf("\r\n%s",EnsureMessage(ensure));
//	return ensure;
//}
//����ģ���ַ PS_SetAddr
//����:  ����ģ���ַ
//����:  PS_addr
//˵��:  ģ�鷵��ȷ����
//uint8_t PS_SetAddr(uint32_t PS_addr)
//{
//	uint16_t temp;
//  uint8_t  ensure;
//	uint8_t  *data;
//	SendHead();
//	SendAddr();
//	SendFlag(0x01);//�������ʶ
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
//	AS608Addr=PS_addr;//������ָ�������ַ
//  data=JudgeStr(2000);
//	if(data)
//		ensure=data[9];
//	else
//		ensure=0xff;	
//		AS608Addr = PS_addr;
//	if(ensure==0x00)
//		//printf("\r\n���õ�ַ�ɹ���");
//	else
//		//printf("\r\n%s",EnsureMessage(ensure));
//	return ensure;
//}
//���ܣ� ģ���ڲ�Ϊ�û�������256bytes��FLASH�ռ����ڴ��û����±�,
//	�ü��±��߼��ϱ��ֳ� 16 ��ҳ��
//����:  NotePageNum(0~15),Byte32(Ҫд�����ݣ�32���ֽ�)
//˵��:  ģ�鷵��ȷ����
//uint8_t PS_WriteNotepad(uint8_t NotePageNum,uint8_t *Byte32)
//{
//	uint16_t temp;
//  uint8_t  ensure,i;
//	uint8_t  *data;
//	SendHead();
//	SendAddr();
//	SendFlag(0x01);//�������ʶ
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
//������PS_ReadNotepad
//���ܣ�  ��ȡFLASH�û�����128bytes����
//����:  NotePageNum(0~15)
//˵��:  ģ�鷵��ȷ����+�û���Ϣ
//uint8_t PS_ReadNotepad(uint8_t NotePageNum,uint8_t *Byte32)
//{
//	uint16_t temp;
//  uint8_t  ensure,i;
//	uint8_t  *data;
//	SendHead();
//	SendAddr();
//	SendFlag(0x01);//�������ʶ
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
//高��搜索PS_HighSpeedSearch
//功能：以 CharBuffer1或CharBuffer2中的特征文件高��搜索整个或部分指纹库��1�7
//		  若搜索到，则返回页码,该指令对于的确存在于指纹库中 ，且登录时质釄1�7
//		  很好的指纹，会很快给出搜索结果��1�7
//参数:  BufferID＄1�7 StartPage(起始顄1�7)，PageNum（页数）
//说明:  模块返回确认孄1�7+页码（相配指纹模板）
uint8_t PS_HighSpeedSearch(uint8_t BufferID,uint16_t StartPage,uint16_t PageNum,SearchResult *p)
{
	uint16_t temp;
  uint8_t  ensure;
	uint8_t  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
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
//����Чģ����� PS_ValidTempleteNum
//���ܣ�����Чģ�����
//����: ��
//˵��: ģ�鷵��ȷ����+��Чģ�����ValidN
uint8_t PS_ValidTempleteNum(uint16_t *ValidN)
{
	uint16_t temp;
  uint8_t  ensure;
	uint8_t  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
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
//		//printf("\r\n��Чָ�Ƹ���=%d",(data[10]<<8)+data[11]);
//	}
//	else
//		//printf("\r\n%s",EnsureMessage(ensure));
	return ensure;
}
//��AS608���� PS_HandShake
//����: PS_Addr��ַָ��
//˵��: ģ�鷵�µ�ַ����ȷ��ַ��	
//uint8_t PS_HandShake(uint32_t *PS_Addr)
//{
//	SendHead();
//	SendAddr();
//	MYUSART_SendData(0X01);
//	MYUSART_SendData(0X00);
//	MYUSART_SendData(0X00);	
//	delay_ms(200);
//	if(USART2_RX_STA&0X8000)//���յ�����
//	{		
//		if(//�ж��ǲ���ģ�鷵�ص�Ӧ���				
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
		while(syspara_t.ps_serch_getimage==0x00)//if(ensure==0x00)//获取图像成功 
		{	
           
			syspara_t.ps_serch_genchar=PS_GenChar(CharBuffer1);
			if( syspara_t.ps_readEeprom_data >0){
				if(syspara_t.ps_serch_genchar==0x00)//���������ɹ� 
				{	 
	                syspara_t.PS_read_template=2;//receive data is 16bytes by USART1 port
	            
					syspara_t.ps_serach_result=PS_Search(CharBuffer1,0,41,&seach);
					
					
					if(syspara_t.ps_serach_result==0x00)//�����ɹ�
					{				
						syspara_t.PS_check_fp_success =1;//OLED_ShowCH(0,2,"  指纹验证成功  ");				
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
						syspara_t.PS_check_fp_fail =1;//OLED_ShowCH(32,2,"验证失败");
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
					syspara_t.PS_check_fp_fail =1;//OLED_ShowCH(32,2,"验证失败");
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
		while(syspara_t.ps_serch_getimage==0x00)//if(ensure==0x00)//获取图像成功 
		{	
           
			syspara_t.ps_serch_genchar=PS_GenChar(CharBuffer1);
			if(syspara_t.ps_readEeprom_data ==1){
				if(syspara_t.ps_serch_genchar==0x00)//���������ɹ� 
				{	 
	                syspara_t.PS_read_template=2;//receive data is 16bytes.
	            
					syspara_t.ps_serach_result=PS_Search(CharBuffer1,0,1,&seach);
					
					
					if(syspara_t.ps_serach_result==0x00)//�����ɹ�
					{				
						syspara_t.PS_check_fp_success =1;//OLED_ShowCH(0,2,"  指纹验证成功  ");	
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
	*Function : 删除指纹
	*Iinput Ref: NO
	*Return Ref:NO
	*
***********************************************************************/
void Del_FR(void)
{
	uint8_t  ensure;
	ensure=PS_Empty();//清空指纹庄1�7
	if(ensure==0)
	{
			//OLED_Clear();
			//OLED_ShowCH(0,2," 清空指纹库成劄1�7 ");		
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
	*Function : 删除指纹
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
		   
				 ps_genChar =PS_GenChar(CharBuffer1);//生成特征
				
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
			   
					 ps_genChar =PS_GenChar(CharBuffer1);//生成特征
					
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
			   
					 ps_genChar =PS_GenChar(CharBuffer1);//生成特征
					
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
			  syspara_t.ps_judeg_read_templete_flag = PS_ValidTempleteNum(&syspara_t.ps_read_templete_numbers);//¶Á¿âÖ¸ÎÆ¸öÊý
				 
		 
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