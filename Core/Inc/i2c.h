#ifndef __I2C_H_
#define __I2C_H_
#include "main.h"


#define  AT24C02_IIC_SCL_Pin   GPIO_PIN_11
#define  AT24C02_IIC_SDA_Pin   GPIO_PIN_10
#define  AT24C02_IIC_GPIO_Port  GPIOB

//IO��������
//#define AT_SDA_IN()  {GPIOB->MODER&=0X0FFFFFFF;GPIOB->MODER&=~(3<<20);}	//PB10����ģʽ:00
//#define AT_SDA_OUT() {GPIOB->MODER&=0X0FFFFFFF;GPIOB->MODER|=1<<20;} 	//PB10:01 :output modeʽ

//IO����
#define AT_IIC_SCL_SetHigh()    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11,GPIO_PIN_SET)                   //SCL      2.�޸����ż����޸�IIC�ӿ�
#define AT_IIC_SCL_SetLow()    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11,GPIO_PIN_RESET)                   //SCL      2.�޸����ż����޸�IIC�ӿ�

#define AT_IIC_SDA_SetHigh()    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10,GPIO_PIN_SET)                   //SCL      2.�޸����ż����޸�IIC�ӿ� 
#define AT_IIC_SDA_SetLow()    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10,GPIO_PIN_RESET)  

//IIC���в�������
void AT_IIC_Init(void);                //��ʼ��IIC��IO��				 
void AT_IIC_Start(void);				//����IIC��ʼ�ź�
void AT_IIC_Stop(void);	  			//����IICֹͣ�ź�
void AT_IIC_Send_Byte(uint8_t txd);			//IIC����һ���ֽ�
uint8_t AT_IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
uint8_t AT_IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void AT_IIC_Ack(void);					//IIC����ACK�ź�
void AT_IIC_NAck(void);				//IIC������ACK�ź�

void AT_IIC_Write_One_Byte(uint8_t daddr,uint8_t addr,uint8_t data);
uint8_t AT_IIC_Read_One_Byte(uint8_t daddr,uint8_t addr);	 


void EE_IIC_Delay(uint8_t us);


#endif 
