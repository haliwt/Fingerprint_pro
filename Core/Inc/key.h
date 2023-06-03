#ifndef __KEY_H_
#define __KEY_H_
#include "main.h"

#define FP_KEY_INPUT_DETECT()   			    HAL_GPIO_ReadPin(FP_INT_INPUT_GPIO_Port,FP_INT_INPUT_Pin)

#define TOUCH_KEY_INPUT_DETECT()  			HAL_GPIO_ReadPin(SC12B_INT_INPUT_GPIO_Port,SC12B_INT_INPUT_Pin)

#define Read_SC12B_KEY()         HAL_GPIO_ReadPin(SC12B_INT_INPUT_GPIO_Port,SC12B_INT_INPUT_Pin)





#define		_KEY_ALL_OFF				  0X1F

//normal times be pressed key
#define		_KEY_TRG_1_WK_UP     			0x01  //WK_UP
#define		_KEY_TRG_2_KEY_1     			0x02  //??????--???
#define		_KEY_TRG_3_TIMER     			0x04   //????
#define		_KEY_TRG_4_FILTER     			0x08
// combination of buttons 
#define		_KEY_TRG_5_WINDTI   			0x11   //?????
#define		_KEY_TRG_6_WINDTI   			0x12   //?????
#define		_KEY_TRG_7_WINDTI   			0x14   //?????
#define		_KEY_TRG_8_WINDTI   			0x18   //?????

//long times be pressed
#define		_KEY_CONT_1_WK_UP     	     0x81    //??????--???
#define		_KEY_CONT_2_KEY_1    		 0x82    //??????--???
#define		_KEY_CONT_3_TIMER     	     0x84    //?????? ?????
#define		_KEY_CONT_4_FILTER     	     0x88    //??? --?????
#define		_KEY_CONT_5_WINDTI     		 0x89




#define		_KEY_CONT_1     	    0x81    //?????
#define		_KEY_CONT_2     		0x82
#define		_KEY_CONT_3     		0x84
#define		_KEY_CONT_4     	    0x88

#define		_KEY_COM_1    		    0x91
#define		_KEY_COM_2    		    0x92
#define		_KEY_COM_3    		    0x94
#define		_KEY_COM_4    		    0x98

typedef  struct  _state_
{
 unsigned char          read;
 unsigned char          buffer;
 unsigned char          value;
 unsigned char          off_time;
 unsigned long          on_time;

 enum{
  start  = 0,
  first  = 1,
  second = 2,
  cont   = 3,
  end    = 4,
  finish = 5,
 }state;
}key_types;

extern key_types key;


typedef enum{

  STATISTICAL_DATA =0x0A,



}key_total;

extern uint8_t virtualPwd[20];

extern void (*ReadDigital_Key_Numbers_Handler)(void);
extern void (*Input_NewPwd_Digtial_Handler)(uint16_t dat);


void KeyFiles_Init(void);

uint8_t Scan_Key(void);

void  SideKey_Fun(uint8_t keyvalue);
void RunCheck_Mode(uint16_t dat);

void TouchKey_Handler(void);
void ReadInput_KeyNumber_Handler(void(*read_digital_key_handler)(void));
void Input_NewPwd_Digital_Handler(void(*input_new_digital_handler)(uint16_t data));
#endif 
