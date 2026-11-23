/* vim: set ai et ts=4 sw=4: */
#include "main.h"
#include "debug.h"
#include "jd6628.h"

/* Private macro -------------------------------------------------------------*/
#define IIC_DEVICE_ADDR 0x5E>>1
//#define DEBUG_JD
#ifdef DEBUG_JD
#define LOGJD LOG
#define LOGJD_HEX LOG_HEX
#else
#define LOGJD(...)
#define LOGJD_HEX(...)
#endif
/* Private define ------------------------------------------------------------*/
#define REG_VOLTAGE 0x10
#define REG_CURRENT 0x11
#define REG_POWER   0x30
#define REG_TEMPERATURE 0x20
#define REG_CAP_NOW 0x12
#define REG_MODEL  0x13
#define REG_CAP_DESIGN 0x14
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/


/* Private user code ---------------------------------------------------------*/
/**
  * @brief  Main program.
  * @retval int
  */
void jd6628_init(void)
{
	I2c_Init();
	//i2c_hal_init();
}

extern I2C_HandleTypeDef I2cHandle;

uint8_t jd6628_read_reg(uint8_t reg, uint8_t *dat, uint8_t len)
{
	uint8_t ret = SI2C_Read_Bytes(IIC_DEVICE_ADDR, reg, dat, len);
	if (ret == 0)
	{
		uint8_t crc = reg;
		for (uint8_t i = 0; i < len-1; i++) crc += dat[i];
		if (crc == dat[len-1])
		{
			//LOGJD("jd6628 ok: %02x %02x\n", reg, crc);
		}
		else
		{
			LOGJD("WRONG CRC: %02x %d %02x %02x\n", reg, len, crc, dat[len-1]);
			LOGJD_HEX(dat, len);
			ret = 2;
		}
	}
	else
	{
		LOGJD("jd6628 ret err: %02x %02x\n", reg, ret);
	}
	return ret;
}

uint8_t jd6628_read_status(JD6628_Stat_t *stat)
{
	uint8_t buff[5];
	uint8_t ret;
#if 1
	ret = jd6628_read_reg(REG_POWER, buff, 5);
	if (ret != 0) return ret;
	stat->power_c1 = buff[1];
	stat->power_c2 = buff[2];
	stat->power_a1 = buff[3];
	stat->insert_c1 = buff[0] & 0x01;
	stat->insert_c2 = buff[0] & 0x02;
	stat->insert_a1 = buff[0] & 0x04;
	LOGJD("jd6628:");
	LOGJD_HEX(buff, 5);
#else
	ret = jd6628_read_reg(REG_POWER, buff, 4);
	if (ret != 0) return ret;
	stat->insert_c1 = buff[0] & 0x01;
	stat->insert_c2 = buff[0] & 0x02;
	LOGJD("JD6628: c1: %d, c2: %d\n", stat->insert_c1, stat->insert_c2);
	LOGJD_HEX(buff, 6);
	ret = jd6628_read_reg(REG_VOLTAGE, buff, 6);
	if (ret != 0) return ret;
	stat->vbus_c1 = (buff[0] | (buff[1]<<8))/100;
	stat->vbus_c2 = (buff[2] | (buff[3]<<8))/100;
	LOGJD("JD6628: v1: %d, v2: %d\n", stat->vbus_c1, stat->vbus_c2);
	ret = jd6628_read_reg(REG_CURRENT, buff, 5);
	if (ret != 0) return ret;
	stat->ibus_c1 = (buff[0] | (buff[1]<<8))/100;
	stat->ibus_c2 = (buff[2] | (buff[3]<<8))/100;
	LOGJD("JD6628: i1: %d, i2: %d\n", stat->ibus_c1, stat->ibus_c2);
	stat->power_c1 = stat->ibus_c1 * stat->vbus_c1 / 10;
	stat->power_c2 = stat->ibus_c2 * stat->vbus_c2 / 10;
	LOGJD("JD6628: p1: %d, p2: %d\n", stat->ibus_c1, stat->ibus_c2);
#endif
	return ret;
}

uint8_t jd6628_read_model(JD6628_Stat_t *stat)
{
	uint8_t buff[12];
	uint8_t ret;
	ret	= jd6628_read_reg(REG_CAP_NOW, buff, 5);
	if (ret == 0)
	{
		stat->cap_now1 = (buff[1] << 8) | buff[0];
		stat->cap_now2 = (buff[3] << 8) | buff[2];
		LOGJD("jd6628: cap now: %04x %04x\n", stat->cap_now1, stat->cap_now2);
	}
	ret = jd6628_read_reg(REG_MODEL, buff, 9);
	if (ret == 0)
	{
		stat->vid1 = (buff[1] << 8) | buff[0];
		stat->pid1 = (buff[3] << 8) | buff[2];
		stat->vid2 = (buff[5] << 8) | buff[4];
		stat->pid2 = (buff[7] << 8) | buff[6];
		LOGJD("jd6628: vid1: %04x pid1: %04x, vid2: %04x pid2: %04x\n", stat->pid1, stat->vid1, stat->vid2, stat->pid2);
	}
	ret = jd6628_read_reg(REG_CAP_DESIGN, buff, 5);
	if (ret == 0)
	{
		stat->cap_full1 = (buff[1] << 8) | buff[0];
		stat->cap_full2 = (buff[3] << 8) | buff[2];
		LOGJD("jd6628: cap design: %04x %04x\n", stat->cap_full1, stat->cap_full2);
	}
	return ret;
}

uint8_t jd6628_read_temperature(JD6628_Stat_t *stat)
{
	uint8_t buff[3];
	uint8_t ret = jd6628_read_reg(REG_TEMPERATURE, buff, 2);
	if (ret == 0)
	{
		stat->temperature = buff[0];
		LOGJD("jd6628 temperature: 0x20 %d\n", stat->temperature);
	}
	return ret;
}
