#include "kmp.h"
#include "run.h"




/**************************************************************************
	*
	*Function Name :unsigned char BF_Search(unsigned char *virtual,unsigned char *pattern)
	*Function : serach long string inside if has son sort string 
	*Input Ref:NO
	*Return Ref:NO
	*
**************************************************************************/
uint8_t  BF_Search(uint8_t *virtual,uint8_t *pattern,uint8_t length)
{
  
	unsigned char i=0,j=0;
	//while(i < run_t.input_digital_key_number_counter){

	for(i=0;i<run_t.input_digital_key_number_counter;i++){

          if(*(virtual+i)== *(pattern+j)){ //virtual password equal real password
				 j++;
		         
				 if(j == length) return 1;

		  }
		  else{
 			 j=0;
		  }
    }

	return 0;
	 

}

