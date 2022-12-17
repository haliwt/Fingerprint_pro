#ifndef __BUZZER_H_
#define __BUZZER_H_
#include "main.h"

//#define BUZZER_HANDLER

#define BEEP_GPIO_Pin            GPIO_PIN_0
#define BEEP_GPIO_Port           GPIOB


#define BUZZER_OFF()     HAL_GPIO_WritePin(BEEP_GPIO_Port , BEEP_GPIO_Pin , GPIO_PIN_RESET)
#define BUZZER_ON()      HAL_GPIO_WritePin(BEEP_GPIO_Port , BEEP_GPIO_Pin , GPIO_PIN_SET)

typedef enum _sound_state{
	sound_key=0x01,
	sound_fail,
	sound_new_pwd_the_first,
	sound_new_pwd_the_second,
	sound_high,
	sound_excute,
	sound_over


}sound_state;

void Buzzer_KeySound(void);
void Buzzer_ShortSound(void);
void Buzzer_LongSound(void);
void Buzzer_Fail_Sound(void);
void Buzzer_Fail_Sound(void);
void Buzzer_Sound_Handler(void);

#endif 
