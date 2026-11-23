#ifndef __DEVICE_NTC_H
#define __DEVICE_NTC_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"

typedef struct
{
	uint32_t adc_read;
	uint32_t adc_filtered;
	enum
	{
		NTC_NORMAL,
		NTC_OVT,
	}	stat;
	uint16_t ntc_time;
	uint32_t ohm;
	uint8_t temperature;
} Ntc_StatT;

extern Ntc_StatT ntcStat;
int ntc_init(void);

#endif
