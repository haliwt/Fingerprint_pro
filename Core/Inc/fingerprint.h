#ifndef __FINGERPRINT_
#define __FINGERPRINT_
#include "as608.h"


#define  FP_POWER_ON()          HAL_GPIO_WritePin(FP_POWER_GPIO_Port ,FP_POWER_Pin,GPIO_PIN_SET)//  (PORTAbits.RA0 =0)
#define  FP_POWER_OFF()         HAL_GPIO_WritePin(FP_POWER_GPIO_Port ,FP_POWER_Pin,GPIO_PIN_RESET)//  (PORTAbits.RA0 =0)

#define FP_INPUT_KEY()         HAL_GPIO_ReadPin(FP_INT_INPUT_GPIO_Port  ,FP_INT_INPUT_Pin)// 



typedef enum _fp_login{

	  FP_THE_FIRST_LOGIN,
	  	
	  FP_AUTO_REGISTER_CMD,
	  FP_THE_SECOND_LOGIN,
	  FP_THE_THIRD_LOGIN,
	  FP_THE_FOURTH_LGOIN,
	  FP_MERGE_TEMPLATE_ONE,
	  FP_MERGE_TEMPLATE_TWO,
	  FP_MERGE_TEMPLATE_THREE,
	  FP_AUTO_REGISTER_SUCCESS,
	  FP_AUTO_REGISTER_FAIL,
	  FP_AUTO_REGISTER_END


}fp_login_state;

typedef enum _fp_check{

      fp_check_confirm_code,
	  fp_check_ref_one,
	  fp_check_ref_two,
	  fp_check_ref_three


}fp_check_instruct;


typedef enum _fp_read_{

    FP_SUCCESS,
	FP_FAIL,

   fp_read_valid_template,
   

}fp_read_state;
   
void RunCommand_Unlock_Fingerprint(void);

void FP_AutoRegister_Handler(void);

void FP_AutoRegister_Handler(void);



void Fingerprint_Unlock_Handler(void);


#endif 

