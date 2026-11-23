/* vim: set ai et ts=4 sw=4: */
#include "main.h"
#include "debug.h"
#include "husb385a.h"
#include "ntc.h"

/* Private define ------------------------------------------------------------*/
#define NTC_OVT_THRESHOLD  600   // ABOUT 9Kohm=90degc
#define NTC_RECOVER_THRESHOLD  1024   // ABOUT 18Kohm=70degc
//#define NTC_RECOVER_THRESHOLD  1290   // ABOUT 24Kohm=60degc
/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef        hADC;
TIM_HandleTypeDef        hTIM1;
TIM_MasterConfigTypeDef  sMasterConfig;
Ntc_StatT ntcStat = {.adc_filtered = 2675}; // default 100k

const static uint32_t ntctable100k[] = 
{
	327020, // 0c
	310764,
	295412,
	280908,
	267201,
	254243,
	241988,
	230394,
	219422,
	209036,
	199201,
	189884,
	181056,
	172688,
	164754,
	157229,
	150090,
	143314,
	136883,
	130775,
	124973,
	119461,
	114222,
	109242,
	104505,
	100000,
	95713,
	91633,
	87749,
	84051,
	80527,
	77171,
	73972,
	70922,
	68014,
	65241,
	62595,
	60071,
	57661,
	55360,
	53164,
	51065,
	49060,
	47144,
	45313,
	43562,
	41888,
	40286,
	38754,
	37288,
	35884,
	34541,
	33254,
	32021,
	30841,
	29710,
	28625,
	27586,
	26590,
	25634,
	24717,
	23838,
	22994,
	22184,
	21406,
	20659,
	19942,
	19254,
	18592,
	17956,
	17345,
	16758,
	16193,
	15650,
	15128,
	14625,
	14142,
	13676,
	13229,
	12798,
	12383,
	11983,
	11598,
	11227,
	10870,
	10525,
	10194,
	9874,
	9565,
	9268,
	8981,
	8704,
	8437,
	8180,
	7931,
	7691,
	7460,
	7236,
	7020,
	6812,
	6610,
	6416,
	6227,
	6046,
	5870,
	5700,
	5536,
	5378,
	5224,
	5076,
	4932,
	4793,
	4659,
	4529,
	4403,
	4281,
	4163,
	4048,
	3938,
	3831,
	3727,
	3626,
	3529,
	3435,
	3343,
	3254,
	3168,
	3085,
	3004,
	2926,
};

/* Private user code ---------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void APP_AdcConfig(void);
static void APP_TimConfig(void);

/**
  * @brief  Main program.
  * @retval int
  */
int ntc_init(void)
{

  /* Initialize UART */
  //BSP_UART_Config();

  /* Configure ADC */
  APP_AdcConfig();
   

  __HAL_RCC_GPIOB_CLK_ENABLE();                           /* Enable GPIO clock */

	LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_1, LL_GPIO_PULL_UP);
  /* ADC Calibrate */   
  if (HAL_ADCEx_Calibration_Start(&hADC, ADC_CALIBRATION_NUMBER_1) != HAL_OK)
  {
    APP_ErrorHandler();
  }
  
  /* ADC Start */
  HAL_ADC_Start_IT(&hADC);
  
  /* TIM Cofig */
  APP_TimConfig();
	return 0;
}

/**
  * @brief  ADC conversion complete callback
  * @param  hadc: ADC handle
  * @retval None
  */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
	ntcStat.adc_read = HAL_ADC_GetValue(hadc); 

	if (ntcStat.adc_filtered == 0) ntcStat.adc_filtered = ntcStat.adc_read;
	else
	{
		ntcStat.adc_filtered = (ntcStat.adc_filtered<<2) - ntcStat.adc_filtered + ntcStat.adc_read;
		ntcStat.adc_filtered >>= 2;
	}
	if (ntcStat.stat == NTC_NORMAL)
	{
		if (ntcStat.adc_filtered < NTC_OVT_THRESHOLD)
		{
			ntcStat.ntc_time++;
			if (ntcStat.ntc_time >= 10)
			{
				LOG("protect, set power to 30\n\n\n");
				ntcStat.ntc_time = 0;
				ntcStat.stat = NTC_OVT;
			}
			LOG("OVT: %d %d\n", ntcStat.adc_filtered, ntcStat.ohm);
		}
		else
		{
			ntcStat.ntc_time = 0;
		}
	}
	else
	{
		if (ntcStat.adc_filtered >= NTC_RECOVER_THRESHOLD)
		{
			ntcStat.ntc_time++;
			if (ntcStat.ntc_time >= 10)
			{
				LOG("protect recover, set power to 45\n\n\n");
				ntcStat.ntc_time = 0;
				ntcStat.stat = NTC_NORMAL;
			}
			LOG("OVT: %d %d\n", ntcStat.adc_filtered, ntcStat.ohm);
		}
		else
		{
			ntcStat.ntc_time = 0;
		}
	}
	ntcStat.ohm = ntcStat.adc_filtered*53000/(4095 - ntcStat.adc_filtered);
	uint32_t ohm = ntcStat.adc_read*53300/(4095 - ntcStat.adc_read);
	uint8_t i = 0;
	for (i = 0; i < 120; i++)
	{
		if (ntcStat.ohm >= ntctable100k[i]) break;
	}
	ntcStat.temperature = i;
	//LOG("NTC: %d %d %d %d %dc\n", ntcStat.adc_read, ohm, ntcStat.adc_filtered, ntcStat.ohm, ntcStat.temperature);
}

/**
  * @brief  ADC Configuration
  * @param  None
  * @retval None
  */
static void APP_AdcConfig(void)
{
  ADC_ChannelConfTypeDef   sConfig = {0};
  
  hADC.Instance = ADC1;
  hADC.Init.ClockPrescaler        = ADC_CLOCK_SYNC_PCLK_DIV4;         /* ADC sync clock with prescaler division by 4 */
  hADC.Init.Resolution            = ADC_RESOLUTION_12B;               /* 12-bit resolution for converted data  */
  hADC.Init.DataAlign             = ADC_DATAALIGN_RIGHT;              /* Right-alignment for converted data */
  hADC.Init.ScanConvMode          = ADC_SCAN_DISABLE;                 /* Scan Mode Disable */
  hADC.Init.EOCSelection          = ADC_EOC_SINGLE_CONV;              /* End of unitary conversion flag */ 
  hADC.Init.LowPowerAutoWait      = DISABLE;                          /* Low power mode disable */
  hADC.Init.ContinuousConvMode    = DISABLE;                          /* Single Conversion  */
  hADC.Init.NbrOfConversion       = 1;                                /* Conversion Number 1 */
  hADC.Init.DiscontinuousConvMode = DISABLE;                          /* Discontinuous Mode Disable */
  /* hADC.Init.NbrOfDiscConversion   = 1; */                          /* Discontinuous Conversion Number 1 */
  hADC.Init.Overrun               = ADC_OVR_DATA_PRESERVED;           /* ADC group regular behavior in case of overrun: data preserved */
  hADC.Init.DMAContinuousRequests = DISABLE;                          /* DMA transfer limited */
  hADC.Init.ExternalTrigConv      = ADC_EXTERNALTRIG_T1_TRGO;         /* TIM1 TRGO Trigger */
  hADC.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_RISING;  /* Rising edge trigger */
  
  if (HAL_ADC_Init(&hADC) != HAL_OK)
  {
    APP_ErrorHandler();
  }
  
  sConfig.Channel      = ADC_CHANNEL_9;             
  sConfig.Rank         = ADC_REGULAR_RANK_1;        
  sConfig.SamplingTime = ADC_SAMPLETIME_640CYCLES_5;           
  if (HAL_ADC_ConfigChannel(&hADC, &sConfig) != HAL_OK)
  {
    APP_ErrorHandler();
  }
}

/**
  * @brief  TIM Configure function
  * @param  None
  * @retval None
  */
static void APP_TimConfig(void)
{
  __HAL_RCC_TIM1_CLK_ENABLE();                                    /* Enable TIM1 Clock */
  hTIM1.Instance = TIM1;                                          /* TIM1 */
  hTIM1.Init.Period            = 8000 - 1;                        /* Period = 8000-1  */
  hTIM1.Init.Prescaler         = 1000 - 1;                        /* Prescaler = 1000-1 */
  hTIM1.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;          /* ClockDivision = 0 */
  hTIM1.Init.CounterMode       = TIM_COUNTERMODE_UP;              /* Counter Direction = up */
  hTIM1.Init.RepetitionCounter = 0;                               /* Repetition = 0  */
  hTIM1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;  /* Auto-Reload register not buffered */
  if (HAL_TIM_Base_Init(&hTIM1) != HAL_OK)                        /* Initialize TIM1 */
  {
    APP_ErrorHandler();
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;            /* Select Update Event as Trigger Source */
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;    /* Disable Master/Slave Mode  */
  HAL_TIMEx_MasterConfigSynchronization(&hTIM1, &sMasterConfig);  /* Configure TIM1 */
  
  if (HAL_TIM_Base_Start(&hTIM1) != HAL_OK)                       /* TIM1 Start */
  {
    APP_ErrorHandler();
  }
}
/******************************************************************************/
/* PY32F032 Peripheral Interrupt Handlers                                     */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file.                                          */
/******************************************************************************/
/**
  * @brief This function handles ADC Interrupt .
  */
void ADC_IRQHandler(void)
{
  HAL_ADC_IRQHandler(&hADC);
}

/**
  * @brief Initialize ADC MSP.
  */
void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{
  RCC_PeriphCLKInitTypeDef RCC_PeriphCLKInit = {0};
  
  /* ADC clock source select system clock  */
  RCC_PeriphCLKInit.PeriphClockSelection= RCC_PERIPHCLK_ADC;
  RCC_PeriphCLKInit.AdcClockSelection   = RCC_ADCCLKSOURCE_SYSCLK;
  HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphCLKInit);
  
  /* Enable ADC clock */
  __HAL_RCC_ADC_CLK_ENABLE();
  
  HAL_NVIC_SetPriority(ADC_IRQn, 0, 0); /* Interrupt priority setting */
  HAL_NVIC_EnableIRQ(ADC_IRQn);         /* ADC interrupt enable */
}
