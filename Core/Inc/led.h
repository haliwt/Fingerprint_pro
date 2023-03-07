#ifndef    __LED_H_
#define    __LED_H_
#include "main.h"
#include "gpio.h"

typedef enum _works_led{
    works_ok_led_on=0x01,
    works_ok_led_off,
    works_error_led_on,
    works_error_led_off,
    works_ok_blink,
    works_error_blink,
    works_null,
    factory_led_test,  
    backlight_led_on,
	backlight_led_off,
    backlight_led_confirm,
    standby_led,
    clear_eeprom_data
}works_led;

typedef enum{

   HOLD =1,
   RELEASE =!HOLD

}release;




#define  POWER_ON()         HAL_GPIO_WritePin(POWER_GPIO_Port ,POWER_Pin ,GPIO_PIN_SET)        // (PORTCbits.RC0= 1)
#define  POWER_OFF()        HAL_GPIO_WritePin(POWER_GPIO_Port  ,POWER_Pin ,GPIO_PIN_RESET)  ///  (PORTCbits.RC0 =0)

#define  BACKLIGHT_ON()       HAL_GPIO_WritePin(LED_EN_GPIO_Port,LED_EN_Pin,GPIO_PIN_SET)//  (PORTCbits.RC3 = 1)
#define  BACKLIGHT_OFF()      HAL_GPIO_WritePin(LED_EN_GPIO_Port,LED_EN_Pin,GPIO_PIN_RESET)//  (PORTCbits.RC3 = 0)

#define  BAT_LED_ON()          HAL_GPIO_WritePin(BATT_LED_GPIO_Port,BATT_LED_Pin,GPIO_PIN_RESET)//  (PORTCbits.RC4 = 0)
#define  BAT_LED_OFF()         HAL_GPIO_WritePin(BATT_LED_GPIO_Port,BATT_LED_Pin,GPIO_PIN_SET) //  (PORTCbits.RC4 =1 )

#define  OK_LED_ON()           HAL_GPIO_WritePin(OK_LED_GPIO_Port,OK_LED_Pin,GPIO_PIN_RESET) //(PORTAbits.RA1 = 0)
#define  OK_LED_OFF()          HAL_GPIO_WritePin(OK_LED_GPIO_Port,OK_LED_Pin,GPIO_PIN_SET)///   (PORTAbits.RA1 = 1)

#define  ERR_LED_ON()          HAL_GPIO_WritePin(ERR_LED_GPIO_Port,ERR_LED_Pin,GPIO_PIN_RESET)//  (PORTAbits.RA0 =0)
#define  ERR_LED_OFF()         HAL_GPIO_WritePin(ERR_LED_GPIO_Port,ERR_LED_Pin,GPIO_PIN_SET)   //  (PORTAbits.RA0 =1)

#define  FP_POWER_ON()          HAL_GPIO_WritePin(FP_POWER_GPIO_Port ,FP_POWER_Pin,GPIO_PIN_SET)//  (PORTAbits.RA0 =0)
#define  FP_POWER_OFF()         HAL_GPIO_WritePin(FP_POWER_GPIO_Port ,FP_POWER_Pin,GPIO_PIN_RESET)//  (PORTAbits.RA0 =0)

#define FP_INPUT_KEY()         HAL_GPIO_ReadPin(FP_INT_INPUT_GPIO_Port  ,FP_INT_INPUT_Pin)// 



//#define KEY1_RA2_GetValue()        //   PORTAbits.RA2
void DisplayLed_Handler(void);


void TouchKey_Led_Handler(void);



void Panel_LED_Off(void);
void Standby_Model_Handler(void);


#endif 
