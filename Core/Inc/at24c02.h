#ifndef __AT24C02_H_
#define __AT24C02_H_
#include "main.h"

#define AT24C01		127
#define AT24C02		255
#define AT24C04		511
#define AT24C08		1023
#define AT24C16		2047
#define AT24C32		4095
#define AT24C64	    8191
#define AT24C128	16383
#define AT24C256	32767  
//STM32F429开发板使用的是24c02，所以定义EE_TYPE为AT24C02
#define EE_TYPE AT24C02
					  

void AT24CXX_Init(void); //初始化IIC

/*********************************************************/
uint8_t  AT24CXX_ReadOneByte(uint16_t  ReadAddr);		
void AT24CXX_WriteOneByte(uint8_t  WriteAddr,uint8_t  DataToWrite);

void EEPROM_Read_Byte(uint16_t Addr,uint8_t *Buffer,uint16_t Length);


void EEPROM_Write_Byte(uint16_t WriteAddr,uint8_t *pBuffer,uint16_t NumToWrite) ;



//void EEPROM_EraseData(void);

void ClearEEPRO_Data(void);


#endif 


