#include "at24c02.h"
#include "i2c.h"

uint8_t pinitBuffer[1]={0};

//uint8_t  AT24CXX_ReadOneByte(uint16_t  ReadAddr);							//ָ����ַ��ȡһ���ֽ�
//void AT24CXX_WriteOneByte(uint16_t  WriteAddr,uint8_t  DataToWrite);		//ָ����ַд��һ���ֽ�
void AT24CXX_WriteLenByte(uint16_t  WriteAddr,uint32_t  DataToWrite,uint8_t  Len);//ָ����ַ��ʼд��ָ�����ȵ�����
uint32_t  AT24CXX_ReadLenByte(uint16_t  ReadAddr,uint8_t  Len);					//ָ����ַ��ʼ��ȡָ����������
void AT24CXX_Write(uint16_t  WriteAddr,uint8_t  *pBuffer,uint16_t  NumToWrite);	//��ָ����ַ��ʼд��ָ�����ȵ�����
void AT24CXX_Read(uint16_t  ReadAddr,uint8_t  *pBuffer,uint16_t  NumToRead);   	//��ָ����ַ��ʼ����ָ�����ȵ�����

uint8_t  AT24CXX_Check(void);  //�������


//��ʼ��IIC�ӿ�
void AT24CXX_Init(void)
{
	AT_IIC_Init();//IIC��ʼ��
}
//��AT24CXXָ����ַ����һ������
//ReadAddr:��ʼ�����ĵ�ַ  
//����ֵ  :����������
uint8_t  AT24CXX_ReadOneByte(uint16_t  ReadAddr)
{				  
	uint8_t  temp=0;		  	    																 
    AT_IIC_Start();  
	if(EE_TYPE>AT24C16)
	{
		AT_IIC_Send_Byte(0XA0);	   //����д����
		AT_IIC_Wait_Ack();
		AT_IIC_Send_Byte(ReadAddr>>8);//���͸ߵ�ַ	    
	}else AT_IIC_Send_Byte(0XA0+((ReadAddr/256)<<1));   //����������ַ0XA0,д���� 	   
	AT_IIC_Wait_Ack(); 
    AT_IIC_Send_Byte(ReadAddr%256);   //���͵͵�ַ
	AT_IIC_Wait_Ack();	    
	AT_IIC_Start();  	 	   
	AT_IIC_Send_Byte(0XA1);           //�������ģʽ			   
	AT_IIC_Wait_Ack();	 
    temp=AT_IIC_Read_Byte(0);		   
    AT_IIC_Stop();//����һ��ֹͣ����	    
	return temp;
}
//��AT24CXXָ����ַд��һ������
//WriteAddr  :д�����ݵ�Ŀ�ĵ�ַ    
//DataToWrite:Ҫд�������
void AT24CXX_WriteOneByte(uint16_t  WriteAddr,uint8_t  DataToWrite)
{				   	  	    																 
    AT_IIC_Start();  
	if(EE_TYPE>AT24C16)
	{
		AT_IIC_Send_Byte(0XA0);	    //����д����
		AT_IIC_Wait_Ack();
		AT_IIC_Send_Byte(WriteAddr>>8);//���͸ߵ�ַ	  
	}else AT_IIC_Send_Byte(0XA0+((WriteAddr/256)<<1));   //����������ַ0XA0,д���� 	 
	AT_IIC_Wait_Ack();	   
    AT_IIC_Send_Byte(WriteAddr%256);   //���͵͵�ַ
	AT_IIC_Wait_Ack(); 	 										  		   
	AT_IIC_Send_Byte(DataToWrite);     //�����ֽ�							   
	AT_IIC_Wait_Ack();  		    	   
    AT_IIC_Stop();//����һ��ֹͣ���� 
	EE_IIC_Delay(10);	 
}
//��AT24CXX�����ָ����ַ��ʼд�볤��ΪLen������
//�ú�������д��16bit����32bit������.
//WriteAddr  :��ʼд��ĵ�ַ  
//DataToWrite:���������׵�ַ
//Len        :Ҫд�����ݵĳ���2,4
void AT24CXX_WriteLenByte(uint16_t  WriteAddr,uint32_t  DataToWrite,uint8_t  Len)
{  	
	uint8_t  t;
	for(t=0;t<Len;t++)
	{
		AT24CXX_WriteOneByte(WriteAddr+t,(DataToWrite>>(8*t))&0xff);
	}												    
}

//��AT24CXX�����ָ����ַ��ʼ��������ΪLen������
//�ú������ڶ���16bit����32bit������.
//ReadAddr   :��ʼ�����ĵ�ַ 
//����ֵ     :����
//Len        :Ҫ�������ݵĳ���2,4
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
//���AT24CXX�Ƿ�����
//��������24XX�����һ����ַ(255)���洢��־��.
//���������24Cϵ��,�����ַҪ�޸�
//����1:���ʧ��
//����0:���ɹ�
uint8_t  AT24CXX_Check(void)
{
	uint8_t  temp;
	temp=AT24CXX_ReadOneByte(255);//����ÿ�ο�����дAT24CXX			   
	if(temp==0X55)return 0;		   
	else//�ų���һ�γ�ʼ�������
	{
		AT24CXX_WriteOneByte(255,0X55);
	    temp=AT24CXX_ReadOneByte(255);	  
		if(temp==0X55)return 0;
	}
	return 1;											  
}

//��AT24CXX�����ָ����ַ��ʼ����ָ������������
//ReadAddr :��ʼ�����ĵ�ַ ��24c02Ϊ0~255
//pBuffer  :���������׵�ַ
//NumToRead:Ҫ�������ݵĸ���
void AT24CXX_Read(uint16_t  ReadAddr,uint8_t  *pBuffer,uint16_t  NumToRead)
{
	while(NumToRead)
	{
		*pBuffer++=AT24CXX_ReadOneByte(ReadAddr++);	
		NumToRead--;
		HAL_Delay(5);
	}
}  
//��AT24CXX�����ָ����ַ��ʼд��ָ������������
//WriteAddr :��ʼд��ĵ�ַ ��24c02Ϊ0~255
//pBuffer   :���������׵�ַ
//NumToWrite:Ҫд�����ݵĸ���
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



