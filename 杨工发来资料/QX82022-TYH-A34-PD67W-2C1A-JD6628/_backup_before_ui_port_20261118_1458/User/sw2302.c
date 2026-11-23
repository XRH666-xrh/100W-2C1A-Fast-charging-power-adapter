/* vim: set ai et ts=4 sw=4: */
#include "main.h"
#include "iic_soft.h"
#include "sw2302.h"
#include "debug.h"

//#define DEBUG_SW2302
#ifdef DEBUG_SW2302
#define log LOG
#else
#define log(...)
#endif

uint8_t sw2302_IsOnline(void)
{
	uint8_t buff[2] = {2};
	uint8_t ret = SI2C_Read_Bytes(IIC_ADDR, 0x07, buff, 1);
	//log("online %d %x\n", ret, buff[0]);
	if (ret)
	{
		return 0;
	}
	return buff[0] & 0x01;
}

uint8_t sw2302_readProtocol(void)
{
	uint8_t buff[2] = {2};
	uint8_t ret = SI2C_Read_Bytes(IIC_ADDR, 0x06, buff, 1);
	//log("Protocol %d %x\n", ret, buff[0]);
	if (ret)
	{
		return 0;
	}
	return buff[0];
}

uint16_t sw2302_readVbus(void)
{
	uint8_t buff[2] = {2};
	uint8_t ret = SI2C_Read_Bytes(IIC_ADDR, 0x31, buff, 1);
	//log("VBUS %d %x\n", ret, buff[0]);
	if (ret)
	{
		return 0;
	}
	return 12 * buff[0];
}

uint16_t sw2302_readVIN(void)
{
	uint8_t buff[2] = {2};
	uint8_t ret = SI2C_Read_Bytes(IIC_ADDR, 0x30, buff, 1);
	log("VIN %d %x\n", ret, buff[0]);
	if (ret)
	{
		return 0;
	}
	return 12 * buff[0];
}

uint16_t sw2302_readIOUT(void)
{
	uint8_t buff[2] = {2};
	uint8_t ret = SI2C_Read_Bytes(IIC_ADDR, 0x33, buff, 1);
	//log("IOUT %d %x\n", ret, buff[0]);
	if (ret)
	{
		return 0;
	}
	return 5 * buff[0];
}

uint8_t sw2302_enableConfig(void)
{
	uint8_t conf = 0x20;
	uint8_t ret;
	ret = SI2C_Read_Bytes(IIC_ADDR, 0x01, &conf, 1);
	log("ver %d %x\n", ret, conf);
	conf = 0x20;
	ret = SI2C_Write_Bytes(IIC_ADDR, 0x12, &conf, 1);
	log("conf1 %d\n", ret);
	conf = 0x40;
	ret = SI2C_Write_Bytes(IIC_ADDR, 0x12, &conf, 1);
	log("conf2 %d\n", ret);
	conf = 0x80;
	ret = SI2C_Write_Bytes(IIC_ADDR, 0x12, &conf, 1);
	log("conf3 %d\n", ret);
	return ret;
}


uint8_t sw2302_setpower(uint8_t power)
{
	uint8_t pwr = 0x80 | power;
	sw2302_enableConfig();
	uint8_t ret = SI2C_Write_Bytes(IIC_ADDR, 0xAF, &pwr, 1);
	log("pwr %d %x\n", ret, pwr);
	pwr = 0xf0;
	ret = SI2C_Write_Bytes(IIC_ADDR, 0xAE, &pwr, 1); // minimal 5v
	pwr = 0x80;
	ret = SI2C_Write_Bytes(IIC_ADDR, 0x73, &pwr, 1); // rebroadcast
	return ret;
}

void sw2302_init(void)
{
	I2c_Init();
	sw2302_setpower(60);
}
