#ifndef __I2C_H_
#define __I2C_H_
#include "main.h"


#define  AT24C02_IIC_SCL_Pin   GPIO_PIN_11
#define  AT24C02_IIC_SDA_Pin   GPIO_PIN_10
#define  AT24C02_IIC_GPIO_Port  GPIOB

//IO方向设置
//#define AT_SDA_IN()  {GPIOB->MODER&=0X0FFFFFFF;GPIOB->MODER&=~(3<<20);}	//PB10输入模式:00
//#define AT_SDA_OUT() {GPIOB->MODER&=0X0FFFFFFF;GPIOB->MODER|=1<<20;} 	//PB10:01 :output mode式

//IO操作
#define AT_IIC_SCL_SetHigh()    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11,GPIO_PIN_SET)                   //SCL      2.修改引脚即可修改IIC接口
#define AT_IIC_SCL_SetLow()    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11,GPIO_PIN_RESET)                   //SCL      2.修改引脚即可修改IIC接口

#define AT_IIC_SDA_SetHigh()    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10,GPIO_PIN_SET)                   //SCL      2.修改引脚即可修改IIC接口 
#define AT_IIC_SDA_SetLow()    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10,GPIO_PIN_RESET)  

//IIC所有操作函数
void AT_IIC_Init(void);                //初始化IIC的IO口				 
void AT_IIC_Start(void);				//发送IIC开始信号
void AT_IIC_Stop(void);	  			//发送IIC停止信号
void AT_IIC_Send_Byte(uint8_t txd);			//IIC发送一个字节
uint8_t AT_IIC_Read_Byte(unsigned char ack);//IIC读取一个字节
uint8_t AT_IIC_Wait_Ack(void); 				//IIC等待ACK信号
void AT_IIC_Ack(void);					//IIC发送ACK信号
void AT_IIC_NAck(void);				//IIC不发送ACK信号

void AT_IIC_Write_One_Byte(uint8_t daddr,uint8_t addr,uint8_t data);
uint8_t AT_IIC_Read_One_Byte(uint8_t daddr,uint8_t addr);	 


void EE_IIC_Delay(uint8_t us);


#endif 
