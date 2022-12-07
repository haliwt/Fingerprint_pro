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

//IIC��ʼ��
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
  GPIO_InitStruct.Speed=GPIO_SPEED_FREQ_HIGH;//����//GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  
}

unsigned char READ_SDA(void)
{
    return HAL_GPIO_ReadPin(AT24C02_IIC_GPIO_Port,AT24C02_IIC_SDA_Pin);
}	

/******************************************************************************
*��  ����void EE_IIC_Delay(void)
*�����ܣ�IIC��ʱ
*��  ������
*����ֵ����
*��  ע: ��ֲʱֻ��Ҫ��EE_IIC_Delay()�����Լ�����ʱ����
*******************************************************************************/	
void EE_IIC_Delay(uint8_t us)
{
        for(int i = 0; i < 10; i++) //20   
        {
            __asm("NOP");//core bus 160M  ����´��IIC���� 400K
        }
    
}
//����IIC��ʼ�ź�
void AT_IIC_Start(void)
{
	AT_SDA_OUT();     //sda�����
	AT_IIC_SDA_SetHigh(); //=1;	  	  
	AT_IIC_SCL_SetHigh();//=1;
	EE_IIC_Delay(4);
 	AT_IIC_SDA_SetLow();//=0;//START:when CLK is high,DATA change form high to low 
	EE_IIC_Delay(4);
	AT_IIC_SCL_SetLow();//=0;//ǯסI2C���ߣ�׼�����ͻ�������� 
}	  
//����IICֹͣ�ź�
void AT_IIC_Stop(void)
{
	AT_SDA_OUT();//sda�����
	AT_IIC_SCL_SetLow();//=0;
	AT_IIC_SDA_SetLow();//=0;//STOP:when CLK is high DATA change form low to high
 	EE_IIC_Delay(4);//EE_IIC_Delay(4);
	AT_IIC_SCL_SetHigh();//=1; 
	AT_IIC_SDA_SetHigh();//=1;//����I2C���߽����ź�
	EE_IIC_Delay(4);							   	
}
//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
uint8_t AT_IIC_Wait_Ack(void)
{
	uint8_t ucErrTime=0;
	AT_SDA_IN();      //SDA����Ϊ����  
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
	AT_IIC_SCL_SetLow();//=0;//ʱ�����0 	   
	return 0;  
} 
//����ACKӦ��
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
//������ACKӦ��		    
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
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
void AT_IIC_Send_Byte(uint8_t txd)
{                        
    uint8_t t,temp;   
	AT_SDA_OUT(); 	    
    AT_IIC_SCL_SetLow();//=0;//����ʱ�ӿ�ʼ���ݴ���
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
		EE_IIC_Delay(2);   //��TEA5767��������ʱ���Ǳ����
		AT_IIC_SCL_SetHigh();//=1;
		EE_IIC_Delay(2); 
		AT_IIC_SCL_SetLow();//=0;	
		EE_IIC_Delay(2);
    }	 
} 	    
//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
uint8_t AT_IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	AT_SDA_IN();//SDA����Ϊ����
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
        AT_IIC_NAck();//����nACK
    else
        AT_IIC_Ack(); //����ACK   
    return receive;
}







