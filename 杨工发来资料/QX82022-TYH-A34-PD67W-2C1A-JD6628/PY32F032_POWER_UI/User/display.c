#include "display.h"
#include "context/guicontext.h"
#include "ui_page.h"
#include "ui_res_image.h"
#include "pm.h"
#include "key.h"
#include "flash.h"

#define PAGE_FLASH_INTERVAL  100
#define PAGE_DISPLAY_INTERVAL 500

enum
{
	PORT_IDLE,
	PORT_WORKING,
	PORT_INSERT,
};

static uint32_t idle_time = 0;
static uint32_t run_time = 0;
config_t config;

powerstat_t powerstat = POWER_UP;

void save_config(void)
{
//	uint8_t dat = 0x80 | config.eng | (config.upside <<1);
//	FlashUpdateByteConfig(dat);
}

void key_handler(void)
{
	get_key_event();
	if (keys[0].key_value)
	{
		//LOG("key: %d\n", keys[0].key_value);
	}
	if (keys[0].key_value == KEY_SHORT_PRES_RELESE)
	{
		if (powerstat == POWER_UP)
		{
			uint8_t page = gui_context_get_current_page();
			if (page == UI_PAGE_POWERUP || page == UI_PAGE_TEMP) gui_context_open_page(UI_PAGE_POWER);
			else gui_context_open_page(UI_PAGE_TEMP);
			powerstat = POWER_UP;
		}
		else
		{
			video_init();
			gui_context_open_page(UI_PAGE_POWERUP);
			powerstat = POWER_UP;
		}
	}
	else if (keys[0].key_value == KEY_DOUBLE_CLICK)
	{

	}
	else if (keys[0].key_value == KEY_LONG_PRES)
	{
		gui_context_open_page(UI_PAGE_POWERUP);
		video_deinit();
		powerstat = POWER_FORCE_OFF;
//		config.upside = !config.upside;
//		video_rotate(config.upside? 2: 0);
//		save_config();
//		//rotate_ui(r);
//		uint8_t page = gui_context_get_current_page();
//		gui_context_open_page(page);
	}
}

void display_init(void)
{
//	uint8_t dat = FlashReadByteConfig();
//	if (dat == 0xff)
//	{
//		config.eng = 0;
//		config.upside = 0;
//	}
//	else
//	{
//		config.eng = (dat & 1) ? 1: 0;
//		config.upside = (dat & 2) ? 1: 0;
//	}
//	if (config.upside)
//	{
//		video_rotate(config.upside? 2: 0);
//	}
}

void update_display(uint32_t millisecond)
{
	static uint32_t on_time = 0;
	static uint32_t displaytime = 0;

	//LOG("%d %d, %d\n", millisecond, count, keys[0].key_value);
	on_time += millisecond;
	displaytime += millisecond;
	key_handler();
	if (on_time < PAGE_FLASH_INTERVAL) return;
	on_time = 0;
	update_port_status();
}
