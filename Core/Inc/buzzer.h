#ifndef __BUZZER_H_
#define __BUZZER_H_
#include "main.h"

//#define BUZZER_HANDLER

#define BEEP_GPIO_Pin            GPIO_PIN_0
#define BEEP_GPIO_Port           GPIOB


#define BUZZER_OFF()     HAL_GPIO_WritePin(BEEP_GPIO_Port , BEEP_GPIO_Pin , GPIO_PIN_RESET)
#define BUZZER_ON()      HAL_GPIO_WritePin(BEEP_GPIO_Port , BEEP_GPIO_Pin , GPIO_PIN_SET)


typedef enum {
   
   key_sound =0x01,
   two_short_one_sound,
   two_short_two_sound,
   high_sound,
   clear_eeprom_sound,
   confirm_sound,
   fail_sound,
   buzzer_sound_null


}buzzer_sound_state;


void Buzzer_KeySound(void);
void Buzzer_ShortSound(void);
void Buzzer_LongSound(void);
void Buzzer_Fail_Sound(void);
void Buzzer_Fail_Sound(void);
void Buzzer_Sound_Handler(void);
void Buzzer_Sound_Stop(void);



#endif 
