/* vim: set ai et ts=4 sw=4: */
#include "device_model.h"
#include "debug.h"

/* Private macro -------------------------------------------------------------*/
//#define SPLOG
#ifdef SPLOG
#define LOGSP LOG
#define LOGSPHEX LOG_HEX
#else
#define LOGSP(...)
#define LOGSPHEX(...)
#endif
/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/* Private user code ---------------------------------------------------------*/

MODEL_E get_apple_model(PID_E pid, BAT_CAP_E bat)
{
	switch(pid)
	{
		case PID_IPHONE1_DUO:
		{
			return IPHONE_DUO;
		}
		case PID_IPHONE18_P_M:
		{
			if (bat <= CAP_IPHONE18PRO) return IPHONE18_PRO;
			else return IPHONE18_PROMAX;
		}
		case PID_IPHONE17_P_M_H:
		{
			return IPHONE17_PROMAX;
		}
		case PID_IPHONE17_P_M:
		{
			if (bat <= CAP_IPHONE17PRO) return IPHONE17_PRO;
			else return IPHONE17_PROMAX;
		}
		case PID_IPHONE17_AIR:return IPHONE17_AIR;
		case PID_IPHONE17:return IPHONE17;
		case PID_IPHONE16_P_M:
		{
			if (bat <= CAP_IPHONE16PRO) return IPHONE16_PRO;
			else return IPHONE16_PROMAX;
		}
		case PID_IPHONE16_PLUS:
		{
			if (bat <= CAP_IPHONE16) return IPHONE16;
			else return IPHONE16_PLUS;
		}
		case PID_IPHONE16_E:return IPHONE16_E;
		case PID_IPHONE15_PM:return IPHONE15_PROMAX;
		case PID_IPHONE15_PRO:return IPHONE15_PRO;
		case PID_IPHONE15_PLUS:return IPHONE15_PLUS;
		case PID_IPHONE15:return IPHONE15;
		case PID_IPHONE14_PM:return IPHONE14_PROMAX;
		case PID_IPHONE14:return IPHONE14;
		case PID_IPHONE14_PLUS:return IPHONE14_PLUS;
		case PID_IPHONE13_PM:return IPHONE13_PROMAX;
		case PID_IPHONE13:return IPHONE13;
		case PID_IPHONE13_MINI:return IPHONE13_MINI;
		case PID_IPHONE12:return IPHONE12;

		case PID_A2836_iPad_Air_6:
		case PID_A2588_iPad_Air_5:
		case PID_A2316_iPad_Air_4:
		case PID_A2316_iPad_Air_4x:
			return IPAD_AIR;
		case PID_IPAD_PRO_11_M5:
		case PID_A2836_iPad_Pro_M4:
		case PID_A2588_iPad_Pro_4:
		case PID_A2588_iPad_Pro_3:
		case PID_A2759_iPad_Pro:
			return IPAD_PRO;
		case PID_A2567_iPad_mini_6:
			return IPAD_MINI;
		case PID_A2696_iPad_10_WiFi:
		case PID_A2696_iPad_9:
			return IPAD;

		case PID_MacBook_Air:
		case PID_A3240_MacBook_Air_M4:
		case PID_A3240_MacBook_Air_M3:
		case PID_A2941_MacBook_Air_M2:
		case PID_A2941_MacBook_Air_M2_2022:
			return MACKBOOK_AIR;
		case PID_A3240_MacBook_PRO_M4:
		case PID_A2485_MacBook_Pro:
		case PID_A2485_MacBook_Pro_2023:
		case PID_A2485_MacBook_Pro_M2:
		case PID_A2485_MacBook_Pro_2021:
		case PID_A2485_MacBook_Pro14_2021:
		case PID_MacBook_Pro_M5:
			return MACKBOOK_PRO;
		case PID_MacBook_Neo_A18:
			return MACKBOOK;
	}
	if (pid > 0x7100 && pid < 0x7200)
	{
		return IPAD;
	}
	else if (pid > 0x7300 && pid < 0x7400)
	{
		return MACKBOOK;
	}
	else if (pid > 0x7500 && pid < 0x7600)
	{
		return IPHONE;
	}
	else
	{
		return APPLE;
	}
}

MODEL_E get_model(uint16_t vid, uint16_t pid, uint16_t bat)
{
	VID_E v = (VID_E)vid;
	PID_E p = (PID_E)pid;
	BAT_CAP_E c = (BAT_CAP_E)bat;
	switch(v)
	{
		case VID_APPLE:
			return get_apple_model(p, c);
		case VID_XIAOMI:
			return XIAOMI;
		case VID_SUNSAM:
			return SUNSAM;
		default:
				break;
	}
	return UNKNOW;
}

