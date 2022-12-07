#ifndef __SINGLE_MODE_H_
#define   __SINGLE_MODE_H_
#include "main.h"

typedef enum{
  POWER_ON_MODEL=0x01,STORE_MODEL=0x02    

}open_model;


extern void (*TouchKey_Handler)(void);
void Start_PowerOn_Handler(void);
void CheckPassword_Lock_Handler(void);

void CheckTouchKey_Press_Handler(void);

void TouchKey_Run_Handler(void (*touchkey_huandler)(void));

void TouchKey(void);




#endif 

