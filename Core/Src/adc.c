/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    adc.c
  * @brief   This file provides code for the configuration
  *          of the ADC instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "adc.h"

/* USER CODE BEGIN 0 */
#include "run.h"
#include "led.h"
/* USER CODE END 0 */

ADC_HandleTypeDef hadc1;

uint16_t adcVale;
uint16_t adcx;


/* ADC1 init function */
void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc1.Init.LowPowerAutoWait = DISABLE;
  hadc1.Init.LowPowerAutoPowerOff = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc1.Init.SamplingTimeCommon1 = ADC_SAMPLETIME_1CYCLE_5;
  hadc1.Init.SamplingTimeCommon2 = ADC_SAMPLETIME_1CYCLE_5;
  hadc1.Init.OversamplingMode = DISABLE;
  hadc1.Init.TriggerFrequencyMode = ADC_TRIGGER_FREQ_HIGH;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLINGTIME_COMMON_1;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

void HAL_ADC_MspInit(ADC_HandleTypeDef* adcHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
  if(adcHandle->Instance==ADC1)
  {
  /* USER CODE BEGIN ADC1_MspInit 0 */

  /* USER CODE END ADC1_MspInit 0 */

  /** Initializes the peripherals clocks
  */
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
    PeriphClkInit.AdcClockSelection = RCC_ADCCLKSOURCE_SYSCLK;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
      Error_Handler();
    }

    /* ADC1 clock enable */
    __HAL_RCC_ADC_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**ADC1 GPIO Configuration
    PA0     ------> ADC1_IN0
    */
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN ADC1_MspInit 1 */

  /* USER CODE END ADC1_MspInit 1 */
  }
}

void HAL_ADC_MspDeInit(ADC_HandleTypeDef* adcHandle)
{

  if(adcHandle->Instance==ADC1)
  {
  /* USER CODE BEGIN ADC1_MspDeInit 0 */

  /* USER CODE END ADC1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_ADC_CLK_DISABLE();

    /**ADC1 GPIO Configuration
    PA0     ------> ADC1_IN0
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_0);

  /* USER CODE BEGIN ADC1_MspDeInit 1 */

  /* USER CODE END ADC1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
/* USER CODE BEGIN 1 */
//»ñµÃADCÖµ
//ch: channel 0~16
//return value: ADC trasmit defulat 
uint16_t Get_Adc(void)   
{
    uint16_t adc_temp;
    ADC_ChannelConfTypeDef ADC0_ChanConf;
    
    ADC0_ChanConf.Channel=0;                                   // channel :0
    ADC0_ChanConf.Rank=ADC_RANK_CHANNEL_NUMBER  ;                                       //the first sequency 1:
  //  ADC1_ChanConf.SamplingTime=ADC_SAMPLETIME_239CYCLES_5;      //sample timing              
    HAL_ADC_ConfigChannel(&hadc1,&ADC0_ChanConf);        //ADC channel Config
	
    HAL_ADC_Start(&hadc1);                               //¿ªÆôADC
	
    HAL_ADC_PollForConversion(&hadc1,20);     //ÂÖÑ¯×ª»»
   
    adc_temp = HAL_ADC_GetValue(&hadc1);	  
	
    // 清除通道
	ADC0_ChanConf.Rank = ADC_RANK_NONE;	// 清除通道
	HAL_ADC_ConfigChannel(&hadc1, &ADC0_ChanConf); //ADC_RANK_CHANNEL_NUMBER;	// 设置通道
 
	return adc_temp;//(uint16_t)HAL_ADC_GetValue(&hadc);	        	//·µ»Ø×î½üÒ»´ÎADC1¹æÔò×éµÄ×ª»»½á¹û
}


uint16_t Get_Adc_Average(uint8_t times)
{
	uint32_t temp_val=0;
	uint8_t t;
	for(t=0;t<times;t++)
	{
		temp_val+=Get_Adc();
		HAL_Delay(10);
	}
	return temp_val/times;
} 	 


void ADC_Detected_LowVoltage(void)
{
if(run_t.gTimer_ADC >6){
	   
	   run_t.gTimer_ADC=0;

	   POWER_ON();
	   adcx=Get_Adc_Average(10);
	
	   adcVale=(uint16_t)((adcx*3300)/4096) +100; //3111 ampliiction * 1000

	   if(adcVale < 1800 ){ // low 3.8V is alarm
		   run_t.ADC_times++; //WT.EDIT 2022.09.09
		   if(run_t.ADC_times > 2 ){
			   run_t.ADC_times = 0;
			   BAT_LED_ON();
		   }

	   }
	   else{
		   run_t.ADC_times = 0;
		   BAT_LED_OFF();
		   HAL_ADC_Stop(&hadc1);  

	   }
			   
	 }
}

/* USER CODE END 1 */
