#ifndef __DEVICE_SLAVE_INTERFACE_H
#define __DEVICE_SLAVE_INTERFACE_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"

#define IIC_ADDR 0x3C
#pragma pack(1)
typedef struct
{
	uint8_t 		PortStatus 				    ;/* 0x00 */
}DSI_REG_INFO;

typedef enum
{
	ADDR_PortStatus 				    ,/* 0x00 */
	IIC_REG_SIZE,
} I2C_REG_ADDRS;
#pragma pack()

void sw2302_init(void);
uint16_t sw2302_readIOUT(void);
uint16_t sw2302_readVbus(void);
uint8_t sw2302_IsOnline(void);
uint8_t sw2302_readProtocol(void);
uint8_t sw2302_setpower(uint8_t power);

#endif
