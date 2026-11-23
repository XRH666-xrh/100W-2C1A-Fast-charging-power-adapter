#include "sp8610.h"
#include "iic_soft.h"

i2c_reg_t device;

// 1. 8610充放电自动唤醒，唤醒后STAT脚会保持高电平，休眠状态为低电平
// 2. 8610唤醒后WAKE脚必须保持低电平，无论是8610充放电自动唤醒还是单片机通过WAKE脚主动唤醒。唤醒后WAKE脚如果拉高1秒后会断电休眠
// 3. I2C寄存器一次读取最多4字节，写入一次最多2字节

#define write_reg(reg, len) SI2C_Write_Bytes(IIC_ADDR, reg, device.buff+reg, len)
#define read_reg(reg, len) SI2C_Read_Bytes(IIC_ADDR, reg, device.buff+reg, len)

#define WAKE_CLK LL_IOP_GRP1_PERIPH_GPIOB
#define WAKE_PORT GPIOB
#define WAKE_PIN LL_GPIO_PIN_4
#define STAT_CLK LL_IOP_GRP1_PERIPH_GPIOB
#define STAT_PORT GPIOB
#define STAT_PIN LL_GPIO_PIN_5

void SP8610_Init(void)
{
	I2c_Init();
  LL_IOP_GRP1_EnableClock(WAKE_CLK);
  LL_GPIO_SetPinMode(WAKE_PORT, WAKE_PIN, LL_GPIO_MODE_OUTPUT);

  LL_IOP_GRP1_EnableClock(STAT_CLK);
  LL_GPIO_SetPinMode(STAT_PORT, STAT_PIN, LL_GPIO_MODE_INPUT);
  SP8610_Wake(1);
}

void SP8610_DeInit(void)
{
	I2c_DeInit();
	SP8610_Wake(0);
}

void SP8610_Wake(uint8_t stat)
{
	if (!stat)
	{
		device.reg.MODULE_SLEEP = 1;
		write_reg(ADDR_MODULE_SLEEP, 1);
	}
	if (!stat) LL_GPIO_SetOutputPin(WAKE_PORT, WAKE_PIN);
	else LL_GPIO_ResetOutputPin(WAKE_PORT, WAKE_PIN);
}

uint8_t SP8610_IsWake(void)
{
	return LL_GPIO_IsInputPinSet(STAT_PORT, STAT_PIN);
}

PORT_STATUS SP8610_getPortStatus(void)
{
	read_reg(ADDR_PortStatus, 1);
	uint8_t a1 = (device.reg.PortStatus & 0x03) >> 0;
	uint8_t c1 = (device.reg.PortStatus & 0x0c) >> 2;
	uint8_t a2 = (device.reg.PortStatus & 0x30) >> 4;
	uint8_t c2 = (device.reg.PortStatus & 0xc0) >> 6;
	PORT_STATUS flag = PORT_IDLE;
	if (c1 == 2 || c2 == 2) flag |= PORT_INPUT;
	if (c1 == 1 || c2 == 1 || a1 == 1 || a2 == 1)
	{
		flag |= PORT_OUTPUT;
	}
	read_reg(ADDR_BatteryFullStateConfig, 1);
	if (device.reg.BatteryFullStateConfig) flag |= PORT_CHARGE_FULL;
	return flag;
}

uint8_t SP8610_is_bat_ntc_error(void)
{
	read_reg(ADDR_PortAbnormalStatus, 1);
	if (device.reg.PortAbnormalStatus & (1<<4)) return 1;
	return 0;
}

uint32_t SP8610_get_bat_voltage(void)
{
	read_reg(ADDR_Vbat_L, 2);
	return device.reg.Vbat;
}

//桥电压
uint8_t SP8610_get_port_vol(void)
{
	read_reg(ADDR_H1PortVoltage, 1);
	return device.reg.H1PortVoltage;
}

//桥电流
uint8_t SP8610_get_port_cur(void)
{
	read_reg(ADDR_H1PortCurrent, 1);
	return device.reg.H1PortCurrent;
}

// 输出开关
void SP8610_set_output_en(PORT_IDX port, uint8_t en)
{
	if (port == PORT_C1)
	{
		device.reg.TYPEC1_DFP_EN = en;
		write_reg(ADDR_TYPEC1_DFP_EN, 1);
	}
	else if (port == PORT_C2)
	{
		device.reg.TYPEC2_DFP_EN = en;
		write_reg(ADDR_TYPEC2_DFP_EN, 1);
	}
}

// 输入开关
void SP8610_set_input_en(PORT_IDX port, uint8_t en)
{
	if (port == PORT_C1)
	{
		device.reg.TYPEC1_UFP_EN = en;
		write_reg(ADDR_TYPEC1_UFP_EN, 1);
	}
	else if (port == PORT_C2)
	{
		device.reg.TYPEC2_UFP_EN = en;
		write_reg(ADDR_TYPEC2_UFP_EN, 1);
	}
}

// 设置输出PDO
void SP8610_set_output_pdo(PORT_IDX port, PDO_POWER pdo, uint8_t pps_en)
{
	if (port == PORT_C1)
	{
		device.reg.C1_PD_OBJ_SEL = pdo | (pps_en? 0x80: 0);
		LOG("PDO REG: 0x%02x\n", device.reg.C1_PD_OBJ_SEL);
		write_reg(ADDR_C1_PD_OBJ_SEL, 1);
		//写输出功率包需要关端口之后重启才会生效，或者重新插拔设备。设置只要电池不断电可以保持
		device.reg.TYPEC1_DFP_EN = 0;
		write_reg(ADDR_TYPEC1_DFP_EN, 1);
		LL_mDelay(100);
		device.reg.TYPEC1_DFP_EN = 1;
		write_reg(ADDR_TYPEC1_DFP_EN, 1);
	}
	else if (port == PORT_C2)
	{
		device.reg.C2_PD_OBJ_SEL = pdo | (pps_en? 0x80: 0);
		write_reg(ADDR_C2_PD_OBJ_SEL, 1);
		device.reg.TYPEC2_DFP_EN = 0;
		write_reg(ADDR_TYPEC2_DFP_EN, 1);
		LL_mDelay(100);
		device.reg.TYPEC2_DFP_EN = 1;
		write_reg(ADDR_TYPEC2_DFP_EN, 1);
	}
}

// 设置充电功率
void SP8610_set_input_power(PORT_IDX port, uint8_t power)
{
	if (port == PORT_C1)
	{
		device.reg.C1_Charge_Power = power;
		write_reg(ADDR_C1_Charge_Power, 1);
	}
	else if (port == PORT_C2)
	{
		device.reg.C2_Charge_Power = power;
		write_reg(ADDR_C2_Charge_Power, 1);
	}
}
// 设置电池串数和满电电压
void SP8610_set_bat(uint8_t num, uint16_t full_vol)
{
	device.reg.Batterynum = num; //串数
	device.reg.Charge_Voltage = full_vol; //单串电池满电电压
	device.reg.Charge_Vol_W_F = 1; //写1让电池设置生效
	write_reg(ADDR_Batterynum, 1);
	write_reg(ADDR_Charge_Voltage_L, 2);
	write_reg(ADDR_Charge_Vol_W_F, 1);
}
