#include "logic/power.h"
#include "ui_page.h"
#include "ui_res_image.h"
#include "pm.h"
#include "device_model.h"
#include "display.h"

#define EXIT_TIME 60 // * 500ms = 30s

static uint32_t uptime = 0;

extern JD6628_Stat_t reg;
static uint8_t bgs[4] = {0};

static char number2w(int16_t total, char* str)
{
	uint8_t idx = 0;
	if (total == -1)
	{
		str[idx++] = 'D';
		str[idx++] = 'D';
		str[idx++] = 0;
	}
	else
	{
		str[idx++] = '0' + (total / 10) % 10;
		str[idx++] = '0' + total % 10;
	}
	return idx;
}

static void update_ring(void)
{
	static uint8_t dir = 0;
	static uint8_t i = 0;
	widget_set_bg_img((widget_t*)UI_WIDGET_POWER_r1, UI_RES_IMAGE_RING_A01+i);
	widget_set_bg_img((widget_t*)UI_WIDGET_POWER_r2, UI_RES_IMAGE_RING_B01+i);
	widget_set_bg_img((widget_t*)UI_WIDGET_POWER_r3, UI_RES_IMAGE_RING_C01+i);
	widget_set_bg_img((widget_t*)UI_WIDGET_POWER_r4, UI_RES_IMAGE_RING_D01+i);
	if (dir == 0)
	{
		if (i >= 20)
		{
			dir = 1;
			i = 18;
		}
		else
		{
			i+=2;
		}
	}
	else
	{
		if (i == 0)
		{
			dir = 0;
			i = 2;
		}
		else
		{
			i-=2;
		}
	}
	LOG("i=%d\n", i);
}

static void update_m(uint8_t port)
{
	MODEL_E model;
	if (port == 1)
	{
		model = get_model((VID_E)reg.vid2, (PID_E)reg.pid2, reg.cap_full2);
	}
	else
	{
		model = get_model((VID_E)reg.vid1, (PID_E)reg.pid1, reg.cap_full1);
	}
	uint16_t id = UI_RES_IMAGE_MODEL_E;
	switch(model)
	{
		case UNKNOW:
			break;
		case APPLE:
			break;
		case XIAOMI:
			break;
		case SUNSAM:
			break;

		case IPHONE:
			id = UI_RES_IMAGE_MODEL_IP;
			break;
		case IPHONE18_PROMAX:
			id = UI_RES_IMAGE_MODEL_IP18PM;
			break;
		case IPHONE18_PRO:
			id = UI_RES_IMAGE_MODEL_IP18PRO;
			break;
		case IPHONE_DUO:
			id = UI_RES_IMAGE_MODEL_IPDUO;
			break;
		case IPHONE17_PROMAX:
			id = UI_RES_IMAGE_MODEL_IP17PM;
			break;
		case IPHONE17_AIR:
			id = UI_RES_IMAGE_MODEL_IP17A;
			break;
		case IPHONE17_PRO:
			id = UI_RES_IMAGE_MODEL_IP17P;
			break;
		case IPHONE17:
			id = UI_RES_IMAGE_MODEL_IP17;
			break;
		case IPHONE16_PROMAX:
			id = UI_RES_IMAGE_MODEL_IP16PM;
			break;
		case IPHONE16_PRO:
			id = UI_RES_IMAGE_MODEL_IP16P;
			break;
		case IPHONE16_PLUS:
			id = UI_RES_IMAGE_MODEL_IP16PP;
			break;
		case IPHONE16_E:
			id = UI_RES_IMAGE_MODEL_IP16E;
			break;
		case IPHONE16:
			id = UI_RES_IMAGE_MODEL_IP16;
			break;
		case IPHONE15_PROMAX:
			id = UI_RES_IMAGE_MODEL_IP15PM;
			break;
		case IPHONE15_PRO:
			id = UI_RES_IMAGE_MODEL_IP15P;
			break;
		case IPHONE15_PLUS:
			id = UI_RES_IMAGE_MODEL_IP15PP;
			break;
		case IPHONE15:
			id = UI_RES_IMAGE_MODEL_IP15;
			break;
		case IPHONE14_PROMAX:
			id = UI_RES_IMAGE_MODEL_IP14PM;
			break;
		case IPHONE14_PRO:
			id = UI_RES_IMAGE_MODEL_IP14P;
			break;
		case IPHONE14_PLUS:
			id = UI_RES_IMAGE_MODEL_IP14PP;
			break;
		case IPHONE14:
			id = UI_RES_IMAGE_MODEL_IP14;
			break;
		case IPHONE13_PROMAX:
			id = UI_RES_IMAGE_MODEL_IP13PM;
			break;
		case IPHONE13_PRO:
			id = UI_RES_IMAGE_MODEL_IP13P;
			break;
		case IPHONE13_MINI:
			id = UI_RES_IMAGE_MODEL_IP13M;
			break;
		case IPHONE13:
			id = UI_RES_IMAGE_MODEL_IP13;
			break;
		case IPHONE12_PRO:
		case IPHONE12:
			id = UI_RES_IMAGE_MODEL_IP12;
			break;

		case IPAD:
			//id = UI_RES_IMAGE_MODEL_IPAD;
			break;
		case IPAD_PRO:
			//id = UI_RES_IMAGE_MODEL_IPP;
			break;
		case IPAD_MINI:
			//id = UI_RES_IMAGE_MODEL_IPM;
			break;
		case IPAD_AIR:
			//id = UI_RES_IMAGE_MODEL_IPA;
			break;

		case MACKBOOK:
			//id = UI_RES_IMAGE_MODEL_MB;
			break;
		case MACKBOOK_AIR:
			//id = UI_RES_IMAGE_MODEL_MBA;
			break;
		case MACKBOOK_PRO:
			//id = UI_RES_IMAGE_MODEL_MBP;
			break;
	}
	if (port == 0) widget_set_bg_img((widget_t*)UI_WIDGET_POWER_phone1, id);
	else widget_set_bg_img((widget_t*)UI_WIDGET_POWER_phone2, id);
}

static void update_p(void)
{
	int32_t total = 0;
	uint8_t str[7] = {0};
	uint8_t i;
	for (i = 0; i < 3; i++)
	{
		if (ports[i].enabled)
		{
			if (ports[i].power) ports[i].lastenabled = PORT_REMOVE_DELAY_CNT;
			total += ports[i].power;
		}
		//LOG("port %d: %d %d %d\n", i, ports[i].voltage, ports[i].current, ports[i].power);
	}
	update_m(0);
	update_m(1);

	// total
	if (total >=10) number2w(total, (char*)str);
	else
	{
		str[0] = 'a';
		str[1] = total % 10 + '0';
		str[2] = 'b';
		str[3] = 0;
	}
	imagetext_set_text(UI_WIDGET_POWER_pt, (const char*)str);
	if (total == 0)
	{
		uptime++;
		if (uptime >= EXIT_TIME)
		{
			gui_context_open_page(UI_PAGE_POWERUP);
		}
	}
	else
	{
		uptime = 0;
	}

	// C1
	if (ports[0].enabled)
	{
		if (bgs[0] != 1)
		{
			widget_set_bg_img((widget_t*)UI_WIDGET_POWER_PC1, UI_RES_IMAGE_PORT_C1);
		}
		bgs[0] = 1;
		number2w(ports[0].power, (char*)str);
		imagetext_set_text(UI_WIDGET_POWER_C1, (const char*)str);
	}
	else if (bgs[0])
	{
		widget_set_bg_img((widget_t*)UI_WIDGET_POWER_PC1, UI_RES_IMAGE_PORT_C1G);
		bgs[0] = 0;
		number2w(-1, (char*)str);
		imagetext_set_text(UI_WIDGET_POWER_C1, (const char*)str);
	}
	// C2
	if (ports[1].enabled)
	{
		if (bgs[1] != 1)
		{
			widget_set_bg_img((widget_t*)UI_WIDGET_POWER_PC2, UI_RES_IMAGE_PORT_C2);
		}
		bgs[1] = 1;
		number2w(ports[1].power, (char*)str);
		imagetext_set_text(UI_WIDGET_POWER_C2, (const char*)str);
	}
	else if (bgs[1])
	{
		ports[1].power = 0;
		widget_set_bg_img((widget_t*)UI_WIDGET_POWER_PC2, UI_RES_IMAGE_PORT_C2G);
		bgs[1] = 0;
		number2w(-1, (char*)str);
		imagetext_set_text(UI_WIDGET_POWER_C2, (const char*)str);
	}
	// A
	if (ports[2].enabled)
	{
		if (bgs[2] != 1)
		{
			widget_set_bg_img((widget_t*)UI_WIDGET_POWER_PA, UI_RES_IMAGE_PORT_A);
		}
		bgs[2] = 1;
		number2w(ports[2].power, (char*)str);
		imagetext_set_text(UI_WIDGET_POWER_A, (const char*)str);
	}
	else if (bgs[2])
	{
		ports[2].power = 0;
		widget_set_bg_img((widget_t*)UI_WIDGET_POWER_PA, UI_RES_IMAGE_PORT_AG);
		bgs[2] = 0;
		number2w(-1, (char*)str);
		imagetext_set_text(UI_WIDGET_POWER_A, (const char*)str);
	}
}
void ui_power_on_page_open(uint8_t page) {

	uptime = 0;
	bgs[0] = bgs[1] = bgs[2] = bgs[3] = 2;
	update_ring();
	update_p();
}

/**
 * @brief 定时器回调接口
 * @param millisecond   定时器周期，单位 ms
 * @param count  累计次数
 */
void ui_power_on_timer(uint32_t millisecond, uint32_t count) {

	static uint8_t time = 0, flashtime = 0;
	if (flashtime++ >= 10)
	{
		flashtime = 0;
		update_ring();
	}
	if (time++ >= 50)
	{
		time = 0;
		update_p();
	}
}

/**
 * @brief 按钮点击事件回调接口
 * @param button   按钮控件指针
 * @param type     触摸事件类型
 */
void ui_power_on_button_touch_event(button_t *button, touch_type_e type) {

}

/**
 * @brief 进度条回调接口
 * @param progress  进度条控件指针
 * @param val       进度值
 */
void ui_power_on_progress_changed(progress_t *progress, uint16_t val) {

}

/**
 * @brief 动画播放结束回调接口
 * @param animation  动画控件指针
 */
void ui_power_on_animation_play_end(animation_t *animation) {

}
