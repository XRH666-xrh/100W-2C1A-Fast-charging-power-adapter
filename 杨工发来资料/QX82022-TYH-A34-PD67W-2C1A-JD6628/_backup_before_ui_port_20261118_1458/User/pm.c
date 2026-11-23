#include "pm.h"
#include "iic_soft.h"

#if DEVICE_TYPE == SP8612 || DEVICE_TYPE == SP8630
uart_recive_stat_t sp =
{
	.rb = &rb2,
	.hinit = &initUart
};
#elif DEVICE_TYPE == HUSB385A
HUSB385A_Stat_t reg;
#elif DEVICE_TYPE == JD6628
JD6628_Stat_t reg;
#endif

port_stat_t ports[PORT_NUM] =
{
	{.type = TYPE_C, .idx = '1', .enabled = 0},
#if PORT_TYPE == PORT_1C1A
	{.type = TYPE_A, .idx = '1', .enabled = 0},
#else
	{.type = TYPE_C, .idx = '2', .enabled = 0},
#endif
#if PORT_TYPE == PORT_3C1A || PORT_TYPE == PORT_4C || PORT_TYPE == PORT_3C
	{.type = TYPE_C, .idx = '3', .enabled = 0},
#endif
#if PORT_TYPE == PORT_2C2A || PORT_TYPE == PORT_2C1A || PORT_TYPE == PORT_3C1A || PORT_TYPE == PORT_1C1AC
	{.type = TYPE_A, .idx = '1', .enabled = 0},
#endif
#if PORT_TYPE == PORT_4C
	{.type = TYPE_C, .idx = '4', .enabled = 0},
#elif PORT_TYPE == PORT_2C2A
	{.type = TYPE_A, .idx = '2', .enabled = 0},
#endif
};

void update_port_status(void)
{
#if SP8610 == DEVICE_TYPE
	ports[0].enabled = SP8610_getPortStatus();
	ports[0].current = SP8610_get_port_cur();
	ports[0].voltage = SP8610_get_port_vol();
	ports[0].power = ports[0].current * ports[0].voltage / 100;
#elif SP8612 == DEVICE_TYPE || SP8630 == DEVICE_TYPE || SP8650 == DEVICE_TYPE
	static uint8_t idle_cnt = 0;
	#define IDLE_TIMEOUT 20  //20*100MS = 2S TIMEOUT
	if (sp.is_receive_newdata)
	{
		idle_cnt = 0;
		sp.is_receive_newdata = 0;
		ports[0].enabled = device.reg.Port1_Mode;
		ports[0].protocol = (ENUM_PROTOCOL_MODE)device.reg.Port1_Protocol;
		ports[0].current = (device.reg.Port1_Source_Cur+50)/100; //round to the nearest int
		ports[0].voltage = (device.reg.Bridge0_Voltage+50)/100;
		ports[0].power = ports[0].current * ports[0].voltage / 100;
#if PORT_TYPE == PORT_1C1A
		ports[1].enabled = device.reg.Port3_Mode;
		ports[1].protocol = (ENUM_PROTOCOL_MODE)device.reg.Port3_Protocol;
		ports[1].current = (device.reg.Bridge2_Current+50)/100;
		ports[1].voltage = (device.reg.Bridge2_Voltage+50)/100;
		ports[1].power = ports[1].current * ports[1].voltage / 100;
#elif (PORT_1C1AC == PORT_TYPE)
		ports[1].enabled = device.reg.Port2_Mode;
		ports[2].enabled = device.reg.Port3_Mode;
		if (ports[1].enabled)
		{
			if (ports[2].enabled)
			{
				ports[1].protocol = (ENUM_PROTOCOL_MODE)device.reg.Port2_Protocol;
				ports[1].current = (device.reg.Bridge1_Current+50)/100/2;
				ports[1].voltage = (device.reg.Bridge1_Voltage+50)/100;
				ports[1].power = ports[1].current * ports[1].voltage / 100;
				ports[2].protocol = (ENUM_PROTOCOL_MODE)device.reg.Port2_Protocol;
				ports[2].current = (device.reg.Bridge1_Current+50)/100/2;
				ports[2].voltage = (device.reg.Bridge1_Voltage+50)/100;
				ports[2].power = ports[2].current * ports[2].voltage / 100;
			}
			else
			{
				ports[1].protocol = (ENUM_PROTOCOL_MODE)device.reg.Port2_Protocol;
				ports[1].current = (device.reg.Bridge1_Current+50)/100;
				ports[1].voltage = (device.reg.Bridge1_Voltage+50)/100;
				ports[1].power = ports[1].current * ports[1].voltage / 100;
				ports[2].current = 0;
			}
		}
		else if (ports[2].enabled)
		{
			ports[2].protocol = (ENUM_PROTOCOL_MODE)device.reg.Port2_Protocol;
			ports[2].current = (device.reg.Bridge1_Current+(device.reg.Bridge1_Current>100?50:0))/100;
			ports[2].voltage = (device.reg.Bridge1_Voltage+50)/100;
			ports[2].power = ports[2].current * ports[2].voltage / 100;
			ports[1].current = 0;
		}
		else
		{
			ports[1].current = 0;
			ports[2].current = 0;
		}
#elif SP8650!= DEVICE_TYPE
		ports[1].enabled = device.reg.Port2_Mode;
		ports[1].protocol = (ENUM_PROTOCOL_MODE)device.reg.Port2_Protocol;
		ports[1].current = (device.reg.Port2_Source_Cur+50)/100;
		ports[1].voltage = (ports[0].enabled?(device.reg.Bridge1_Voltage+50):(device.reg.Bridge0_Voltage+50))/100;
		ports[1].power = ports[1].current * ports[1].voltage / 100;
#else
		ports[1].enabled = device.reg.Port3_Mode;
		ports[1].protocol = (ENUM_PROTOCOL_MODE)device.reg.Port2_Protocol;
		ports[1].current = (
			(
				((device.reg.Port3_Source_Cur>>8) & 0xff)
				|((device.reg.Port3_Source_Cur<<8) & 0xff00)
			)+50
		)/100;
		ports[1].voltage = (device.reg.Bridge1_Voltage+50)/100;
		ports[1].power = ports[1].current * ports[1].voltage / 100;
#endif
#if PORT_NUM >=3
#if SP8650!= DEVICE_TYPE
		ports[2].enabled = device.reg.Port3_Mode;
		ports[2].protocol = (ENUM_PROTOCOL_MODE)device.reg.Port3_Protocol;
		ports[2].current = (device.reg.Port3_Source_Cur+50)/100;
		ports[2].voltage = (device.reg.Bridge1_Voltage+50)/100;
		ports[2].power = ports[2].current * ports[2].voltage / 100;
#else
		ports[2].enabled = device.reg.Port4_Mode;
		ports[2].protocol = (ENUM_PROTOCOL_MODE)device.reg.Port2_Protocol;
		ports[2].current = (
			(
				((device.reg.Port4_Source_Cur>>8) & 0xff)
				|((device.reg.Port4_Source_Cur<<8) & 0xff00)
			)+50
		)/100;
		ports[2].voltage = (device.reg.Bridge1_Voltage+50)/100;
		ports[2].power = ports[2].current * ports[2].voltage / 100;
#endif
#endif
#if PORT_NUM == 4
		ports[3].enabled = device.reg.Port4_Mode;
		ports[3].protocol = (ENUM_PROTOCOL_MODE)device.reg.Port4_Protocol;
		ports[3].current = (device.reg.Port4_Source_Cur+50)/100;
		ports[3].voltage = (device.reg.Bridge3_Voltage+50)/100;
		ports[3].power = ports[3].current * ports[3].voltage / 100;
#endif
	}
	else
	{
		idle_cnt++;
		if (idle_cnt >= IDLE_TIMEOUT)
		{
			LOG("ERROR: NO POWER DATA RECIVED IN TIME \r\n");
			idle_cnt = 0;
			ports[0].enabled = 0;
			ports[1].enabled = 0;
#if PORT_NUM >=3
			ports[2].enabled = 0;
#endif
#if PORT_NUM == 4
			ports[3].enabled = 0;
#endif
		}
	}
#elif SW2302 == DEVICE_TYPE
	ports[0].enabled = sw2302_IsOnline();
	ports[0].voltage = sw2302_readVbus(),
	ports[0].current = sw2302_readIOUT();
	ports[0].power = ports[0].current * ports[0].voltage / 10000;
	ports[0].protocol = sw2302_readProtocol();
	//LOG("ports: %d %d %d\n", ports[0].enabled, ports[0].voltage, ports[0].current);
#elif HUSB385A == DEVICE_TYPE
#define FILTER_NUM 8
	static uint16_t filters[PORT_NUM][FILTER_NUM] = {0};
	uint16_t current, sum, i;
	uint8_t ret = husb385a_read_status(&reg);
	if (ret == 0)
	{
		ports[0].enabled = reg.insert_c1;
		ports[0].voltage = reg.vbus * 10;
		sum = current = (reg.ibus_c1 * 10);
		for (i = 0; i < FILTER_NUM-1; i++)
		{
			filters[0][i] = filters[0][i+1];
			sum += filters[0][i];
		}
		filters[0][i] = current;
		ports[0].current = sum >> 5;
		ports[0].power = ports[0].current * ports[0].voltage / 10000;
		ports[0].protocol = reg.protocol;
		ports[1].enabled = reg.insert_c2;
		ports[1].voltage = reg.vbus * 10;
		sum = current = (reg.ibus_c2_a2 * 10);
		for (uint8_t i = 0; i < FILTER_NUM-1; i++)
		{
			filters[1][i] = filters[1][i+1];
			sum += filters[1][i];
		}
		filters[1][i] = current;
		ports[1].current = sum >> 5;
		ports[1].power = ports[1].current * ports[1].voltage / 10000;
		ports[1].protocol = reg.protocol;
	}
	else
	{
		ports[0].enabled = 0;
		ports[0].voltage = 0;
		ports[0].current = 0;
		ports[0].power = 0;
		ports[0].protocol = 0;
		ports[1].enabled = 0;
		ports[1].voltage = 0;
		ports[1].current = 0;
		ports[1].power = 0;
		ports[1].protocol = 0;
	}
	//LOG("ports: %d %d %d %d\n", ret, ports[0].enabled, ports[0].voltage, ports[0].current);
#elif JD6628 == DEVICE_TYPE
	uint8_t ret = jd6628_read_status(&reg);
	if (ret == 0)
	{
		ports[0].enabled = reg.insert_c1;
		ports[1].enabled = reg.insert_c2;
		ports[2].enabled = reg.insert_a1;
		ports[0].power = reg.power_c1;
		ports[1].power = reg.power_c2;
		ports[2].power = reg.power_a1;
	}
	jd6628_read_model(&reg);
	//ret = jd6628_read_temperature(&reg);
#else
	uint8_t i;
	for (i = 0; i<PORT_NUM; i++)
	{
		ports[i].enabled = 1;
		ports[i].current++;
		if (ports[i].current >= 50) ports[i].current = 0;
		ports[i].voltage++;
		if (ports[i].voltage >= 280) ports[i].voltage = 0;
		ports[i].power = ports[i].current * ports[i].voltage / 100;
	}
#endif
}

// run in main process
inline void pm_process(void)
{
#if DEVICE_TYPE == SP8612 || DEVICE_TYPE == SP8630
	sp86xx_recive_process(&sp);
#endif
}

void pm_init(void)
{
#if SW2302 == DEVICE_TYPE
	sw2302_init();
#elif SP8610 == DEVICE_TYPE
#elif DEVICE_TYPE == SP8612 || DEVICE_TYPE == SP8630	
	sp86xx_init(&sp);
#elif DEVICE_TYPE == HUSB385A
	husb385a_init();
#elif DEVICE_TYPE == JD6628
	jd6628_init();
#endif
}
