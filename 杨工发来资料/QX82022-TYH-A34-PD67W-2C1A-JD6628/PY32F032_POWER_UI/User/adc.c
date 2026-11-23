/**
  ******************************************************************************
  * @file    adc.c
  * @author  adc Application Team
  * @brief   
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "adc.h"

/* Private define ------------------------------------------------------------*/
#define ADC_CALIBRATION_TIMEOUT_MS       ((uint32_t) 1)
#define ADC_CHANNEL_NUMS     5
#define ADC_SAMPLE_INTERVAL_MS   20
#define ADC_SAMPLE_TIMEOUT_MS  20
typedef enum
{
	ADC_DISABLED,
	ADC_ENABLED,
	ADC_STARTED,
	ADC_FINISHED,
} ADC_STATUS;
typedef struct
{
	ADC_STATUS status;
	uint8_t readIdx;
	uint32_t adcValues[ADC_CHANNEL_NUMS];
	uint32_t filtedValues[ADC_CHANNEL_NUMS];
	uint32_t lastStartTime;
} adc_value_t;

/* Private variables ---------------------------------------------------------*/

adc_value_t adcValue = {
	.status = ADC_DISABLED,
	.readIdx = ADC_CHANNEL_NUMS,
	.adcValues = {0},
	.filtedValues = {0},
};

uint32_t chs[] = {
	LL_ADC_CHANNEL_1, //NTC
	LL_ADC_CHANNEL_3, //QI
	LL_ADC_CHANNEL_4, //PORTA
	LL_ADC_CHANNEL_5, //KEY
	LL_ADC_CHANNEL_7  //BAT
};

/* Private user code ---------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static void AdcConfig(void);
static void AdcCalibrate(void);
static void QuickSort(int32_t arr[], int32_t length);

/**
  * @brief  adcInit
  * @retval int
  */
void AdcInit(void)
{

  LL_ADC_Reset(ADC1);
  
  /* Enable ADC1 clock */
  LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_ADC1);
  
  /* ADC calibration */
  AdcCalibrate();

  /* Configure ADC parameters */
  AdcConfig();
}

void AdcDeInit(void)
{
  LL_ADC_DisableIT_EOC(ADC1);
  LL_ADC_DisableIT_EOS(ADC1);
	LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_1, LL_GPIO_PULL_NO);
	LL_ADC_CommonDeInit(__LL_ADC_COMMON_INSTANCE(ADC1));
  LL_APB1_GRP2_DisableClock(LL_APB1_GRP2_PERIPH_ADC1);
}

uint32_t AdcGetValue(ADC_CH_E ch)
{
	return adcValue.adcValues[ch];
}

uint32_t AdcGetFiltedValue(ADC_CH_E ch)
{
	return adcValue.filtedValues[ch];
}

#define VOLTAGE_VCC 5000
uint32_t AdcGetVoltage(ADC_CH_E ch)
{
	return adcValue.filtedValues[ch] * VOLTAGE_VCC / 4095;
}

/**
  * @brief  ADC Service Loop
  * @retval None
  */
void AdcLoop(void)
{
	switch (adcValue.status)
	{
		case ADC_DISABLED:
			if (g_systick_1ms - adcValue.lastStartTime >= ADC_SAMPLE_INTERVAL_MS)
			{
				/* Enable ADC */
				LL_ADC_Enable(ADC1);
				adcValue.lastStartTime = g_systick_1ms;
				adcValue.status = ADC_ENABLED;
				adcValue.readIdx = 0;
			}
			break;
		case ADC_ENABLED:
			/* ADC stabilization time, minimum 8 ADC Clock cycles */	
			if (g_systick_1ms - adcValue.lastStartTime >= 1)
			{
				/* Start ADC conversion */
				LL_ADC_REG_StartConversion(ADC1);
				adcValue.status = ADC_STARTED;
			}
			break;
		case ADC_STARTED:
			if (g_systick_1ms - adcValue.lastStartTime >= ADC_SAMPLE_TIMEOUT_MS)
			{
				// should not reach here
				LOG("ERROR: ADC CONVERSION TIMETOUT: %d\r\n", g_systick_1ms, adcValue.lastStartTime);
				LL_ADC_REG_StopConversion(ADC1);
				LL_ADC_Disable(ADC1);
				adcValue.status = ADC_DISABLED;
			}
			break;
		case ADC_FINISHED:
			//LOG("ADC SAMPLE FINSH %d %d\r\n", g_systick_1ms, adcValue.lastStartTime);
			//LOG("adc: ");
			for (adcValue.readIdx = 0; adcValue.readIdx < ADC_CHANNEL_NUMS; adcValue.readIdx++)
			{
				// IIR FILTER
				adcValue.filtedValues[adcValue.readIdx] = (
					adcValue.adcValues[adcValue.readIdx]
					+ (adcValue.filtedValues[adcValue.readIdx]<<4) - adcValue.filtedValues[adcValue.readIdx]
				) >> 4;
				//LOG(" %d,", adcValue.filtedValues[adcValue.readIdx]);
			}
			//LOG("\r\n");
			adcValue.status = ADC_DISABLED;
			break;
	}
}

/**
  * @brief  ADC Convert the selected channel
  * @param  channel: channel is selected to be converted
  * @note   !take 5ms~6ms at 
  * @retval None
  */
uint32_t AdcConvert(ADC_CH_E ch)
{
  uint32_t AdcConvertValue;

  /* Clear all channels */
  WRITE_REG(ADC1->CHSELR,0);
  
  /* Set channel as conversion channel */
  LL_ADC_REG_SetSequencerChannels(ADC1, chs[ch]);
  
	/* Enable ADC */
	LL_ADC_Enable(ADC1);

	/* ADC stabilization time, minimum 8 ADC Clock cycles */
	LL_mDelay(1);

  /* Start ADC conversion */
  LL_ADC_REG_StartConversion(ADC1);
  
  /* Wait ADC conversion complete */
  while(LL_ADC_IsActiveFlag_EOC(ADC1)==0);
  
  /* Clear eoc flag */
  LL_ADC_ClearFlag_EOC(ADC1);
  
  /* Get conversion value */
  AdcConvertValue = LL_ADC_REG_ReadConversionData12(ADC1);
  
  /* Clear all channels */
  WRITE_REG(ADC1->CHSELR,0);

	//AdcValues[ch] = (AdcConvertValue + (AdcValues[ch]<<4) - AdcValues[ch]) >> 4; // IIR FILTER
	
  return AdcConvertValue; 
}

/******************************************************************************/
/* PY32F0xx Peripheral Interrupt Handlers                                     */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file.                                          */
/******************************************************************************/
/**
  * @brief This function handles ADC_COMP Interrupt .
  */
void ADC_COMP_IRQHandler(void)
{
  /* Check if it is an interrupt triggered by the end of conversion */
  if(LL_ADC_IsActiveFlag_EOC(ADC1) != 0)
  {
    /* Clear ADC EOC interrupt flag */
    LL_ADC_ClearFlag_EOC(ADC1);
		adcValue.adcValues[adcValue.readIdx++] = LL_ADC_REG_ReadConversionData12(ADC1);
  }
  /* Check if it is an interrupt triggered by the end of sequence conversions. */
  if(LL_ADC_IsActiveFlag_EOS(ADC1) != 0)
  {
    /* Clear ADC EOS interrupt flag */
    LL_ADC_ClearFlag_EOS(ADC1);
		adcValue.status = ADC_FINISHED;
    /* Call the interrupt handler function */
  }
}

/**
  * @brief  ADC configuration function
  * @param  None
  * @retval None
  */
static void AdcConfig(void)
{
  /* Enable GPIOA clock */
  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);

  /* Configure pin 1/3/4/5/7 as analog input */
  LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_1|LL_GPIO_PIN_3|LL_GPIO_PIN_4|LL_GPIO_PIN_5|LL_GPIO_PIN_7, LL_GPIO_MODE_ANALOG);

	// pull up PA1 because the NTC doesn't have a extenal PULL UP
	LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_1, LL_GPIO_PULL_UP);
  /* ADC channel and clock source should be configured when ADEN=0, others should be configured when ADSTART=0 */
  /* Configure internal conversion channel */
  LL_ADC_SetCommonPathInternalCh(__LL_ADC_COMMON_INSTANCE(ADC1), LL_ADC_PATH_INTERNAL_NONE);

  /* Set ADC clock */
  LL_ADC_SetClock(ADC1, LL_ADC_CLOCK_ASYNC_HSI_DIV64);

  /* Set 12-bit resolution */
  LL_ADC_SetResolution(ADC1, LL_ADC_RESOLUTION_12B);

  /* Set data alignment to right */
  LL_ADC_SetDataAlignment(ADC1, LL_ADC_DATA_ALIGN_RIGHT);

  /* Set low power mode */
  LL_ADC_SetLowPowerMode(ADC1, LL_ADC_LP_AUTOWAIT);

  /* Set channel conversion time */
  LL_ADC_SetSamplingTimeCommonChannels(ADC1, LL_ADC_SAMPLINGTIME_239CYCLES_5);

  /* ADC regular group conversion trigger by software */
  LL_ADC_REG_SetTriggerSource(ADC1, LL_ADC_REG_TRIG_SOFTWARE);

  /* Set DMA mode as disabled */
  LL_ADC_REG_SetDMATransfer(ADC1, LL_ADC_REG_DMA_TRANSFER_NONE);

  /* Set overrun management mode to data overwritten */
  LL_ADC_REG_SetOverrun(ADC1, LL_ADC_REG_OVR_DATA_OVERWRITTEN);

  /* Set discontinuous mode to disabled */
  LL_ADC_REG_SetSequencerDiscont(ADC1, LL_ADC_REG_SEQ_DISCONT_DISABLE);
  
  /* Set ADC conversion mode to single mode: one conversion per trigger */
  LL_ADC_REG_SetContinuousMode(ADC1, LL_ADC_REG_CONV_SINGLE);

  /* Set channel as conversion channel */
  LL_ADC_REG_SetSequencerChannels(ADC1, 
		LL_ADC_CHANNEL_1| //NTC
		LL_ADC_CHANNEL_3| //QI
		LL_ADC_CHANNEL_4| //PORTA
		LL_ADC_CHANNEL_5| //KEY
		LL_ADC_CHANNEL_7  //BAT
	);

  /* Enable EOC interrupt */
  LL_ADC_EnableIT_EOC(ADC1);
  LL_ADC_EnableIT_EOS(ADC1);

  NVIC_SetPriority(ADC_COMP_IRQn, 0);
  NVIC_EnableIRQ(ADC_COMP_IRQn);
}

/**
  * @brief  Sort the array.
  * @param  arr the array
  * @param  length the array length
  * @retval  None
  */
static void QuickSort(int32_t arr[], int32_t length)
{
  int32_t pos,min;
  for(int32_t i=0;i<length;i++)
  {
    min = arr[i];
    pos = i;
    for(int32_t j=i+1;j<length;j++)
    {
      if(min > arr[j])
      {
        min = arr[j];
        pos = j;
      }
    }
    arr[pos] = arr[i];
    arr[i] = min;
  }
}

/**
  * @brief  ADC calibration function
  * @param  None
  * @retval None
  */
static void AdcCalibrate(void)
{
  __IO uint32_t backup_setting_adc_dma_transfer = 0;
  uint32_t ADC_CALRR1_Buff[5];
  int32_t ADC_CALRR1_BuffInt[5];
#if (USE_TIMEOUT == 1)
  uint32_t Timeout = 0; 
#endif 

  if (LL_ADC_IsEnabled(ADC1) == 0)
  {
    /* Disable ADC DMA transfer during calibration */
    backup_setting_adc_dma_transfer = LL_ADC_REG_GetDMATransfer(ADC1);
    LL_ADC_REG_SetDMATransfer(ADC1, LL_ADC_REG_DMA_TRANSFER_NONE);

    for(int i=0;i<5;i++)
    {
      /* Enable calibration */
      LL_ADC_StartCalibration(ADC1);

#if (USE_TIMEOUT == 1)
    Timeout = ADC_CALIBRATION_TIMEOUT_MS;
#endif 

      while (LL_ADC_IsCalibrationOnGoing(ADC1) != 0)
      {
#if (USE_TIMEOUT == 1)
        /* Check if calibration is timeout */
        if (LL_SYSTICK_IsActiveCounterFlag())
        {
          if(Timeout-- == 0)
          {

          }
        }
#endif
     }
      ADC_CALRR1_Buff[i]= *((__IO uint32_t *)(0x40012448)); 
      ADC_CALRR1_BuffInt[i]=(int32_t)(ADC_CALRR1_Buff[i]<<9);   
   }

    QuickSort(ADC_CALRR1_BuffInt,5); 
    *((__IO uint32_t *)(0x40012450)) = (ADC_CALRR1_BuffInt[2]>>9);             
    *((__IO uint32_t *)(0x40012454)) = *((__IO uint32_t *)(0x4001244C));
    *((__IO uint32_t *)(0x40012444)) |= 0x8000; 
    /* Delay between ADC calibration end and ADC enable: minimum 4 ADC Clock cycles */
    LL_mDelay(1);

    /* Restore ADC DMA configuration */
    LL_ADC_REG_SetDMATransfer(ADC1, backup_setting_adc_dma_transfer);
  }
}
