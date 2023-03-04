#ifndef __SINGLE_MODE_H_
#define   __SINGLE_MODE_H_
#include "main.h"

typedef enum{
  POWER_ON_MODEL=0x01,STORE_MODEL_EEPROM,EXIT_STORE_MODEL   

}open_model;

typedef enum{

   ADMINISTRATOR_ID = 0x01,
   PWD_ID ,
   FP_ID ,
   DISPOSE_STEP_COMPARE,
   DISPOSE_STEP_UNLOCK,
   DISPOSE_SAVE_DATA,
   DISPOSE_BUZZER_SOUND,
   DISPOSE_LED_DISPLAY,
   DISPOSE_MOTOR_RUN,
   DISPOSE_BACKLIGHT_OFF,
   DISPOSE_STANDBY,
   DISPOSE_ADM_RELEASE_KEY,

   DISPOSE_UNLOCK_FAIL,
   DISPOSE_KEY_FAIL,
   DISPOSE_KEY_LOCK_60S,
   DISPOSE_WKES_LED_STATE,
   DISPOSE_NULL

 }pwd_fp_eunum_t;

typedef enum
{
  KEY_FAIL ,
  KEY_SUCCESS,
  KEY_SOUND,
  KEY_LOCK_60S,
  KEY_NULL
} KEY_ErrorStatus;



extern void (*TouchKey_Handler)(void);
extern void (*FP_ReadData_Handler)(void);
void Start_PowerOn_Handler(void);
void CheckPassword_Lock_Handler(void);

void CheckTouchKey_Press_Handler(void);

void TouchKey_Run_Handler(void (*touchkey_huandler)(void));

void TouchKey(void);

void Fingerprint_ReadData_Handler(void(*fp_read_handler)(void));




#endif 

