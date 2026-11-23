#ifndef __APP_ADC_H__
#define __APP_ADC_H__
#include "main.h"

typedef enum
{
	ADC_CH_NTC,
	ADC_CH_QI,
	ADC_CH_PORT_A,
	ADC_CH_KEY,
	ADC_CH_BAT,
	ADC_CH_VREF,
} ADC_CH_E;

uint32_t AdcGetValue(ADC_CH_E ch);
uint32_t AdcGetFiltedValue(ADC_CH_E ch);
uint32_t AdcGetVoltage(ADC_CH_E ch);
void AdcLoop(void);
void AdcInit(void);
void AdcDeInit(void);

#endif
