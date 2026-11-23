#ifndef __JD6628_DRIVER_INTERFACE_H
#define __JD6628_DRIVER_INTERFACE_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "iic_soft.h"

#pragma pack(1)

typedef struct {
	uint8_t insert_c1;
	uint8_t insert_c2;
	uint8_t insert_a1;
	uint16_t vbus_c1;
	uint16_t vbus_c2;
	uint16_t vbus_a1;
	uint16_t ibus_c1;
	uint16_t ibus_c2;
	uint16_t ibus_a1;
	uint16_t power_c1;
	uint16_t power_c2;
	uint16_t power_a1;
	uint8_t temperature;
	uint16_t cap_now1;
	uint16_t cap_design1;
	uint16_t cap_full1;
	uint16_t vid1;
	uint16_t pid1;
	uint16_t cap_now2;
	uint16_t cap_design2;
	uint16_t cap_full2;
	uint16_t vid2;
	uint16_t pid2;
} JD6628_Stat_t;
#pragma pack()

#define IIC_REG_SIZE 24
#define DSI_REG_INFO JD6628_Stat_t

uint8_t jd6628_read_status(JD6628_Stat_t *stat);
uint8_t jd6628_read_model(JD6628_Stat_t *stat);
uint8_t jd6628_read_temperature(JD6628_Stat_t *stat);
void jd6628_init(void);

#endif
