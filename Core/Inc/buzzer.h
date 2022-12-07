#ifndef __BUZZER_H_
#define __BUZZER_H_
//#include "main.h"

//#define BUZZER_HANDLER

#define BEEP_GPIO_Pin            GPIO_PIN_0
#define BEEP_GPIO_Port           GPIOB


#define BUZZER_OFF()     HAL_GPIO_WritePin(BEEP_GPIO_Port , BEEP_GPIO_Pin , GPIO_PIN_RESET)
#define BUZZER_ON()      HAL_GPIO_WritePin(BEEP_GPIO_Port , BEEP_GPIO_Pin , GPIO_PIN_SET)

void BUZZER_KeySound(void);


void Buzzer_LongSound(void);

void Buzzer_High_Sound(void);
void Buzzer_High_Sound_2(void);




void Buzzer_ShortSound(void);
void Buzzer_Off(void);
void Buzzer_ErrorSound(void);
void Fail_Buzzer_Sound(void);



#endif 
