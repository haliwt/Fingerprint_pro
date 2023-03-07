#ifndef __AS608_H
#define __AS608_H
#include <stdio.h>
#include "main.h" 


#define CharBuffer1 0x01
#define CharBuffer2 0x02



extern uint32_t AS608Addr;//ģ���ַ
extern uint16_t EEPROM_AS608Addr;

typedef struct  
{
	uint16_t pageID;//ָ��ID
	uint16_t mathscore;//ƥ��÷�
}SearchResult;

typedef enum _color{

  
  color_off=0x00,
  blue_on = 0x01,
  green_on = 0x02,
  red_on = 0x04
  
  
}color;

typedef enum _fun_color{

   breath_led =0x01,
	 blank_led ,
	 open_led,
	 close_led
}fun_color;

typedef enum _runcmd{
	FP_SEARCH=0x01,
	FP_GEN_CHAR,
	FP_SEARCH_FAIL,
	FP_SEARCH_INIT,
    FP_SET_NEW_FP,
    FP_SET_INIT
}runcmd;

typedef struct _syspara
{

uint8_t 	PS_clear_ps_success;
	uint8_t 	PS_clear_ps_fail;
	uint8_t     ps_pre_detector;
	uint8_t     fp_rx_times;

  uint8_t PS_login_flag;

  uint8_t handler_read_data_flag;
  uint8_t ps_login_new_fp_success;
    
	
    
  uint8_t uart1_rx_data;
  uint8_t PS_read_template;
	
  uint8_t ps_error_times_key;
  uint8_t ps_judeg_read_templete_flag;
	
	//eeprom 
	uint8_t ps_readEeprom_data;
  uint8_t FP_RunCmd_Lable;
  uint8_t ps_serch_lable;
  uint8_t fp_administrator_flag;


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

//void PS_StaGPIO_Init(void);//��ʼ��PA6��״̬����
	
uint8_t PS_GetImage(void); //¼��ͼ�� 
 
uint8_t PS_GenChar(uint8_t BufferID);//�������� 

uint8_t PS_Match(void);//��ȷ�ȶ���öָ������ 


 
uint8_t PS_RegModel(void);//�ϲ�����������ģ�壩 
 
uint8_t PS_StoreChar(uint8_t BufferID,uint16_t PageID);//����ģ�� 

uint8_t PS_DeletChar(uint16_t PageID,uint16_t N);//ɾ��ģ�� 

uint8_t PS_Empty(void);//���ָ�ƿ� 

uint8_t PS_WriteReg(uint8_t RegNum,uint8_t DATA);//дϵͳ�Ĵ��� 
 
//uint8_t PS_ReadSysPara(SysPara *p); //��ϵͳ�������� 

//uint8_t PS_SetAddr(uint32_t addr);  //����ģ���ַ 

//uint8_t PS_WriteNotepad(uint8_t NotePageNum,uint8_t *content);//д���±� 

//uint8_t PS_ReadNotepad(uint8_t NotePageNum,uint8_t *note);//������ 
uint8_t PS_Search(uint8_t BufferID,uint16_t StartPage,uint16_t PageNum,SearchResult *p);


uint8_t PS_HighSpeedSearch(uint8_t BufferID,uint16_t StartPage,uint16_t PageNum,SearchResult *p);//�������� 
  
uint8_t PS_ValidTempleteNum(uint16_t *ValidN);//����Чģ����� 

uint8_t PS_HandShake(uint32_t *PS_Addr); //��AS608ģ������

uint8_t PS_Sleep(void);

//uint8_t PS_ControlBLN(uint8_t fundata,uint8_t startcolor,uint8_t endcolor,uint8_t cycletimes);



void Add_FR(void);


void Del_FR(void);

void PS_Rx_InputInfo_Handler(void);
void Fingerprint_NewClinet_Login_Fun(void);
void RunCommand_Unlock_Fingerprint(void);
uint8_t  PS_LED_ALL_OFF(void);

uint8_t PS_Breath_LED(void) ;
uint8_t  PS_Error_Blink_LED(void);
uint8_t  PS_Red_Led_ON(void);
uint8_t  PS_Red_Led_OFF(void);

uint8_t PS_Green_Led_OFF(void);
uint8_t PS_Green_Led_ON(void);

uint8_t PS_Blue_Led_ON(void);
uint8_t PS_Blue_Led_OFF(void);





#endif

