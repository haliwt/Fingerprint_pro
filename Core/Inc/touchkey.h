#ifndef   __TOUCHKEY_H_
#define   __TOUCHKEY_H_
#include "main.h"
#include "gpio.h"
//IO Direction setup
//#define I2C_SDA_IO_IN()      {GPIOB->MODER&=0XFFFF3FFF;GPIOB->MODER|=0<<14;}  //0x00 input mode
//#define I2C_SDA_IO_OUT()     {GPIOB->MODER&=0XFFFF3FFF;GPIOB->MODER|=1<<14;}   //0x01 output  mode 


#define  SPECIAL_APP	1



#define I2C_SDA_SetHigh()            HAL_GPIO_WritePin(IIC_SDA_GPIO_Port,IIC_SDA_Pin,GPIO_PIN_SET)    // output high level
#define I2C_SDA_SetLow()             HAL_GPIO_WritePin(IIC_SDA_GPIO_Port,IIC_SDA_Pin,GPIO_PIN_RESET)    // output low level

#define I2C_SCL_SetHigh()            HAL_GPIO_WritePin(IIC_SCL_GPIO_Port,IIC_SCL_Pin,GPIO_PIN_SET)    // output high level
#define I2C_SCL_SetLow()             HAL_GPIO_WritePin(IIC_SCL_GPIO_Port,IIC_SCL_Pin,GPIO_PIN_RESET)    // output low level

#define I2C_SDA_ReadData()           HAL_GPIO_ReadPin(IIC_SDA_GPIO_Port,IIC_SDA_Pin)




#define SC12B_ADDR    0x40         //ASEL 引脚悬空，设备的地址 ->write address

#define SC12B_READ_ADDR        0X41

#define CTRL0_REG                    0x02    //CTRL0 control register setup of address


#define RTM0 								0 									//3 个采样周期有效， 1 个采样周期判断无效
#define RTM1 								1 									//4 个采样周期有效， 2 个采样周期判断无效
#define RTM2 								2 									//5 个采样周期有效， 3 个采样周期判断无效
#define RTM3								3 									//6 个采样周期有效， 4 个采样周期判断无效
#define KVF_STOP_CORREC 		    (1u<<2)							// 按键有效，触摸不校准
#define KVF_50S_CORREC 			    (0u<<2) 						// 按下有效后， 50S 开始校准
#define HOLD 						(1u<<3) 						//基线保持不校准
#define NOTHOLD 					(0u<<3) 						//基线持续校准
#define SLPCYC_LGT 					(0u<<5) 						//无穷大
#define SLPCYC_0R5T 				(1u<<5) 						//休眠后采样间隔 60MS
#define SLPCYC_1R5T 				(2u<<5) 						//休眠后采样间隔 180MS
#define SLPCYC_2R5T 				(3u<<5) 						//休眠后采样间隔 300MS
#define SLPCYC_3R5T 				(4u<<5) 						//休眠后采样间隔 420MS
#define SLPCYC_4R5T 				(5u<<5) 						//休眠后采样间隔 540MS
#define SLPCYC_5R5T 				(6u<<5) 						//休眠后采样间隔 660MS
#define SLPCYC_6R5T				 	(7u<<5) 						//休眠后采样间隔 780MS
#define FAST_TO_SLEEP 				(1u<<4) 						//快速进入休眠
#define SLOW_TO_SLEEP 				(0u<<4) 						// 75S 进入休眠








typedef enum
{
    UNDONE = 0x00,
    DONE   = 0x01
}Complete_Status; 

extern uint8_t  SC_Data[2];

void I2C_SDA_IO_IN(void) ;	//PB11配置成输入 
//void I2C_SDA_IO_OUT(void)//PB11配置成开漏输出

void I2C_SCL_IO_IN(void); 	//PB11配置成输入 








void ICman_Init_SET(unsigned char SC_ADDR);  //IC->SC12B Initialize funciton
Complete_Status I2C_Simple_Read_From_Device(unsigned char deviceAddr,unsigned char* target,unsigned char len);
Complete_Status I2C_Read_From_Device(unsigned char deviceAddr,unsigned char REG,unsigned char* target,unsigned char len);





#endif 

