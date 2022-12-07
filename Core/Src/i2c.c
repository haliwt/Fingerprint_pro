#include "i2c.h"

static void AT_SDA_IN(void) ;
static void AT_SDA_OUT(void) ;

static void AT_SDA_IN(void) 
{

	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_PIN_10;
	  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	  GPIO_InitStruct.Pull = GPIO_NOPULL;
	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);




}

static void AT_SDA_OUT(void) 
{

	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_PIN_10;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);




}

//IIC初始化
void AT_IIC_Init(void)
{
   GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10|GPIO_PIN_11, GPIO_PIN_SET);

 
  /*Configure GPIO pins : PC13 PC14 */
  GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed=GPIO_SPEED_FREQ_HIGH;//高速//GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  
}

unsigned char READ_SDA(void)
{
    return HAL_GPIO_ReadPin(AT24C02_IIC_GPIO_Port,AT24C02_IIC_SDA_Pin);
}	

/******************************************************************************
*函  数：void EE_IIC_Delay(void)
*功　能：IIC延时
*参  数：无
*返回值：无
*备  注: 移植时只需要将EE_IIC_Delay()换成自己的延时即可
*******************************************************************************/	
void EE_IIC_Delay(uint8_t us)
{
        for(int i = 0; i < 10; i++) //20   
        {
            __asm("NOP");//core bus 160M  情况下大概IIC速率 400K
        }
    
}
//产生IIC起始信号
void AT_IIC_Start(void)
{
	AT_SDA_OUT();     //sda线输出
	AT_IIC_SDA_SetHigh(); //=1;	  	  
	AT_IIC_SCL_SetHigh();//=1;
	EE_IIC_Delay(4);
 	AT_IIC_SDA_SetLow();//=0;//START:when CLK is high,DATA change form high to low 
	EE_IIC_Delay(4);
	AT_IIC_SCL_SetLow();//=0;//钳住I2C总线，准备发送或接收数据 
}	  
//产生IIC停止信号
void AT_IIC_Stop(void)
{
	AT_SDA_OUT();//sda线输出
	AT_IIC_SCL_SetLow();//=0;
	AT_IIC_SDA_SetLow();//=0;//STOP:when CLK is high DATA change form low to high
 	EE_IIC_Delay(4);//EE_IIC_Delay(4);
	AT_IIC_SCL_SetHigh();//=1; 
	AT_IIC_SDA_SetHigh();//=1;//发送I2C总线结束信号
	EE_IIC_Delay(4);							   	
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
uint8_t AT_IIC_Wait_Ack(void)
{
	uint8_t ucErrTime=0;
	AT_SDA_IN();      //SDA设置为输入  
	AT_IIC_SDA_SetHigh();//=1;
    EE_IIC_Delay(1);	   
	AT_IIC_SCL_SetHigh();//=1;
    EE_IIC_Delay(1);	 
	while(READ_SDA())
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			AT_IIC_Stop();
			return 1;
		}
	}
	AT_IIC_SCL_SetLow();//=0;//时钟输出0 	   
	return 0;  
} 
//产生ACK应答
void AT_IIC_Ack(void)
{
	AT_IIC_SCL_SetLow();//=0;
	AT_SDA_OUT();
	AT_IIC_SDA_SetLow();//=0;
	EE_IIC_Delay(2);
	AT_IIC_SCL_SetHigh();//=1;
	EE_IIC_Delay(2);
	AT_IIC_SCL_SetLow();//=0;
}
//不产生ACK应答		    
void AT_IIC_NAck(void)
{
	AT_IIC_SCL_SetLow();//=0;
	AT_SDA_OUT();
	AT_IIC_SDA_SetHigh();//=1;
	EE_IIC_Delay(2);
	AT_IIC_SCL_SetHigh();//=1;
	EE_IIC_Delay(2);
	AT_IIC_SCL_SetLow();//=0;
}					 				     
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void AT_IIC_Send_Byte(uint8_t txd)
{                        
    uint8_t t,temp;   
	AT_SDA_OUT(); 	    
    AT_IIC_SCL_SetLow();//=0;//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
        //AT_IIC_SDA((txd&0x80)>>7);//IIC_SDA=(txd&0x80)>>7;
        temp = (txd >>7)&0x01;  //High bit the first tranmist . low bit in the end transmit
        if(temp & 0x01){
           AT_IIC_SDA_SetHigh();
        }
        else{
           AT_IIC_SDA_SetLow();
        }
        txd<<=1; //	  
		EE_IIC_Delay(2);   //对TEA5767这三个延时都是必须的
		AT_IIC_SCL_SetHigh();//=1;
		EE_IIC_Delay(2); 
		AT_IIC_SCL_SetLow();//=0;	
		EE_IIC_Delay(2);
    }	 
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
uint8_t AT_IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	AT_SDA_IN();//SDA设置为输入
    for(i=0;i<8;i++ )
	{
        AT_IIC_SCL_SetLow();//=0; 
        EE_IIC_Delay(2);
		AT_IIC_SCL_SetHigh();//=1;
        receive<<=1;
        if(READ_SDA())receive++;   
		EE_IIC_Delay(1); 
    }					 
    if (!ack)
        AT_IIC_NAck();//发送nACK
    else
        AT_IIC_Ack(); //发送ACK   
    return receive;
}







