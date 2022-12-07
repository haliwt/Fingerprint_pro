#include "at24c02.h"
#include "i2c.h"

uint8_t pinitBuffer[1]={0};

//uint8_t  AT24CXX_ReadOneByte(uint16_t  ReadAddr);							//指定地址读取一个字节
//void AT24CXX_WriteOneByte(uint16_t  WriteAddr,uint8_t  DataToWrite);		//指定地址写入一个字节
void AT24CXX_WriteLenByte(uint16_t  WriteAddr,uint32_t  DataToWrite,uint8_t  Len);//指定地址开始写入指定长度的数据
uint32_t  AT24CXX_ReadLenByte(uint16_t  ReadAddr,uint8_t  Len);					//指定地址开始读取指定长度数据
void AT24CXX_Write(uint16_t  WriteAddr,uint8_t  *pBuffer,uint16_t  NumToWrite);	//从指定地址开始写入指定长度的数据
void AT24CXX_Read(uint16_t  ReadAddr,uint8_t  *pBuffer,uint16_t  NumToRead);   	//从指定地址开始读出指定长度的数据

uint8_t  AT24CXX_Check(void);  //检查器件


//初始化IIC接口
void AT24CXX_Init(void)
{
	AT_IIC_Init();//IIC初始化
}
//在AT24CXX指定地址读出一个数据
//ReadAddr:开始读数的地址  
//返回值  :读到的数据
uint8_t  AT24CXX_ReadOneByte(uint16_t  ReadAddr)
{				  
	uint8_t  temp=0;		  	    																 
    AT_IIC_Start();  
	if(EE_TYPE>AT24C16)
	{
		AT_IIC_Send_Byte(0XA0);	   //发送写命令
		AT_IIC_Wait_Ack();
		AT_IIC_Send_Byte(ReadAddr>>8);//发送高地址	    
	}else AT_IIC_Send_Byte(0XA0+((ReadAddr/256)<<1));   //发送器件地址0XA0,写数据 	   
	AT_IIC_Wait_Ack(); 
    AT_IIC_Send_Byte(ReadAddr%256);   //发送低地址
	AT_IIC_Wait_Ack();	    
	AT_IIC_Start();  	 	   
	AT_IIC_Send_Byte(0XA1);           //进入接收模式			   
	AT_IIC_Wait_Ack();	 
    temp=AT_IIC_Read_Byte(0);		   
    AT_IIC_Stop();//产生一个停止条件	    
	return temp;
}
//在AT24CXX指定地址写入一个数据
//WriteAddr  :写入数据的目的地址    
//DataToWrite:要写入的数据
void AT24CXX_WriteOneByte(uint16_t  WriteAddr,uint8_t  DataToWrite)
{				   	  	    																 
    AT_IIC_Start();  
	if(EE_TYPE>AT24C16)
	{
		AT_IIC_Send_Byte(0XA0);	    //发送写命令
		AT_IIC_Wait_Ack();
		AT_IIC_Send_Byte(WriteAddr>>8);//发送高地址	  
	}else AT_IIC_Send_Byte(0XA0+((WriteAddr/256)<<1));   //发送器件地址0XA0,写数据 	 
	AT_IIC_Wait_Ack();	   
    AT_IIC_Send_Byte(WriteAddr%256);   //发送低地址
	AT_IIC_Wait_Ack(); 	 										  		   
	AT_IIC_Send_Byte(DataToWrite);     //发送字节							   
	AT_IIC_Wait_Ack();  		    	   
    AT_IIC_Stop();//产生一个停止条件 
	EE_IIC_Delay(10);	 
}
//在AT24CXX里面的指定地址开始写入长度为Len的数据
//该函数用于写入16bit或者32bit的数据.
//WriteAddr  :开始写入的地址  
//DataToWrite:数据数组首地址
//Len        :要写入数据的长度2,4
void AT24CXX_WriteLenByte(uint16_t  WriteAddr,uint32_t  DataToWrite,uint8_t  Len)
{  	
	uint8_t  t;
	for(t=0;t<Len;t++)
	{
		AT24CXX_WriteOneByte(WriteAddr+t,(DataToWrite>>(8*t))&0xff);
	}												    
}

//在AT24CXX里面的指定地址开始读出长度为Len的数据
//该函数用于读出16bit或者32bit的数据.
//ReadAddr   :开始读出的地址 
//返回值     :数据
//Len        :要读出数据的长度2,4
uint32_t  AT24CXX_ReadLenByte(uint16_t  ReadAddr,uint8_t  Len)
{  	
	uint8_t  t;
	uint32_t  temp=0;
	for(t=0;t<Len;t++)
	{
		temp<<=8;
		temp+=AT24CXX_ReadOneByte(ReadAddr+Len-t-1); 	 				   
	}
	return temp;												    
}
//检查AT24CXX是否正常
//这里用了24XX的最后一个地址(255)来存储标志字.
//如果用其他24C系列,这个地址要修改
//返回1:检测失败
//返回0:检测成功
uint8_t  AT24CXX_Check(void)
{
	uint8_t  temp;
	temp=AT24CXX_ReadOneByte(255);//避免每次开机都写AT24CXX			   
	if(temp==0X55)return 0;		   
	else//排除第一次初始化的情况
	{
		AT24CXX_WriteOneByte(255,0X55);
	    temp=AT24CXX_ReadOneByte(255);	  
		if(temp==0X55)return 0;
	}
	return 1;											  
}

//在AT24CXX里面的指定地址开始读出指定个数的数据
//ReadAddr :开始读出的地址 对24c02为0~255
//pBuffer  :数据数组首地址
//NumToRead:要读出数据的个数
void AT24CXX_Read(uint16_t  ReadAddr,uint8_t  *pBuffer,uint16_t  NumToRead)
{
	while(NumToRead)
	{
		*pBuffer++=AT24CXX_ReadOneByte(ReadAddr++);	
		NumToRead--;
		HAL_Delay(5);
	}
}  
//在AT24CXX里面的指定地址开始写入指定个数的数据
//WriteAddr :开始写入的地址 对24c02为0~255
//pBuffer   :数据数组首地址
//NumToWrite:要写入数据的个数
void AT24CXX_Write(uint16_t  WriteAddr,uint8_t  *pBuffer,uint16_t  NumToWrite)
{
	while(NumToWrite--)
	{
		AT24CXX_WriteOneByte(WriteAddr,*pBuffer);
		WriteAddr++;
		pBuffer++;
		HAL_Delay(5);
	}
}


void EEPROM_Read_Byte(uint16_t  ReadAddr,uint8_t  *pBuffer,uint16_t  NumToRead)
{


    AT24CXX_Read(ReadAddr,pBuffer,NumToRead);



}

void EEPROM_Write_Byte(uint16_t  WriteAddr,uint8_t  *pBuffer,uint16_t  NumToWrite)
{

  AT24CXX_Write( WriteAddr,pBuffer,  NumToWrite);

}


//void EEPROM_EraseData(void);

void ClearEEPRO_Data(void)
{
    uint8_t i=255;

	while(i--){

	 AT24CXX_WriteOneByte(i,0);

   }

     AT24CXX_WriteOneByte(0,0);


}



