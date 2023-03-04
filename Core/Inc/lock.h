#ifndef __LOCK_H_
#define __LOCK_H_
#include "main.h"


void Read_Administrator_Password(void);
void ReadPassword_EEPROM_SaveData(void);

void SavePassword_To_EEPROM(void);
void Lock_Open_Order(void);
void Open_Lock_Success_Fun(void);
void Open_Lock_Fail_Fun(void);



#endif 
