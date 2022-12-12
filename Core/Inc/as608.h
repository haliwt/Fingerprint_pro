#ifndef __AS608_H
#define __AS608_H
#include <stdio.h>
#include "main.h" 


#define CharBuffer1 0x01
#define CharBuffer2 0x02



extern uint32_t AS608Addr;//模块地址
extern uint16_t EEPROM_AS608Addr;

typedef struct  
{
	uint16_t pageID;//指纹ID
	uint16_t mathscore;//匹配得分
}SearchResult;

typedef struct _syspara
{
	uint16_t PS_max;//指纹最大容量
	uint8_t  PS_level;//安全等级
	uint32_t PS_addr;
	uint8_t  PS_size;//通讯数据包大小
	uint8_t  PS_N;//波特率基数N
	uint8_t  PS_rx_data_flag;
	uint8_t  PS_input_success;
	uint8_t  PS_input_fail;
	uint8_t  PS_quit_input_ps_flag;
	uint8_t  PS_finish_input_ps_flag;

	uint8_t  	PS_check_fp_success;
	uint8_t  	PS_check_fp_fail;
	uint8_t 	PS_clear_ps_success;
	uint8_t 	PS_clear_ps_fail;

	uint8_t PS_login_flag;
	uint8_t PS_wakeup_flag;
	uint8_t PS_pressed_flag;
	uint8_t PS_login_success;
    uint8_t uart1_rx_data;
    uint8_t PS_read_template;
	
    uint8_t ps_error_times_key;
    
    uint8_t ps_judeg_read_templete_flag;
	
	//eeprom 
	uint8_t ps_readEeprom_data;
	uint8_t ps_thefirst_input_fp;


	//search
	uint8_t ps_serch_getimage;
	uint8_t ps_serch_genchar;
	uint8_t ps_serach_result;

	uint8_t PS_login_times;
    uint8_t fp_login_key;

	uint8_t PS_save_numbers;
    uint8_t PS_exit_login_flag;
    uint8_t PS_exit_times;
    
    uint16_t ps_read_templete_numbers;
	
    
}SysPara;


extern SysPara syspara_t;

//void PS_StaGPIO_Init(void);//初始化PA6读状态引脚
	
uint8_t PS_GetImage(void); //录入图像 
 
uint8_t PS_GenChar(uint8_t BufferID);//生成特征 

uint8_t PS_Match(void);//精确比对两枚指纹特征 


 
uint8_t PS_RegModel(void);//合并特征（生成模板） 
 
uint8_t PS_StoreChar(uint8_t BufferID,uint16_t PageID);//储存模板 

uint8_t PS_DeletChar(uint16_t PageID,uint16_t N);//删除模板 

uint8_t PS_Empty(void);//清空指纹库 

uint8_t PS_WriteReg(uint8_t RegNum,uint8_t DATA);//写系统寄存器 
 
//uint8_t PS_ReadSysPara(SysPara *p); //读系统基本参数 

//uint8_t PS_SetAddr(uint32_t addr);  //设置模块地址 

//uint8_t PS_WriteNotepad(uint8_t NotePageNum,uint8_t *content);//写记事本 

//uint8_t PS_ReadNotepad(uint8_t NotePageNum,uint8_t *note);//读记事 
uint8_t PS_Search(uint8_t BufferID,uint16_t StartPage,uint16_t PageNum,SearchResult *p);


uint8_t PS_HighSpeedSearch(uint8_t BufferID,uint16_t StartPage,uint16_t PageNum,SearchResult *p);//高速搜索 
  
uint8_t PS_ValidTempleteNum(uint16_t *ValidN);//读有效模板个数 

uint8_t PS_HandShake(uint32_t *PS_Addr); //与AS608模块握手



void Add_FR(void);
void Press_ReadFingerprint_Data(void);

void Del_FR(void);

void PS_Rx_InputInfo_Handler(void);
void Fingerprint_NewClinet_Login_Fun(void);


#endif

