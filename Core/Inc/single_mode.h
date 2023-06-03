#ifndef __SINGLE_MODE_H_
#define   __SINGLE_MODE_H_
#include "main.h"

typedef enum{
  POWER_ON_MODEL=0x01,STORE_MODEL_EEPROM,EXIT_STORE_MODEL   

}open_model;


typedef enum{

   confirm_button_donot_pressed,
   confirm_button_pressed,
   confirm_button_unlock,
   confirm_button_save_new_password,
   confirm_button_clearn_eeprom,
   confirm_button_lock_panel,
   confirm_button_over_numbers,
   confirm_button_error_times,
   confirm_button_null

}confirm_button_stat;




extern void (*FP_ReadData_Handler)(void);
void Start_PowerOn_Handler(void);
void CheckPassword_Lock_Handler(void);

void CheckTouchKey_Press_Handler(void);

void TouchKey_Run_Handler(void (*touchkey_huandler)(void));



void Fingerprint_ReadData_Handler(void(*fp_read_handler)(void));

void CheckPassword_UnLock_Handler(void);



#endif 

