#ifndef __UI_DISPLAY_H__
#define __UI_DISPLAY_H__
#include "main.h"

typedef enum
{
	POWER_UP,
	POWER_IDLE,
	POWER_FORCE_OFF,
} powerstat_t;

#define PORT_REMOVE_DELAY_CNT 3

#pragma anon_unions
typedef struct
{
	uint8_t eng;
	uint8_t upside;
} config_t;

extern config_t config;

extern powerstat_t powerstat;

void update_page(uint8_t pageid);
void update_display(uint32_t millisecond);
void display_init(void);

#endif
