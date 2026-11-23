#include "logic/temp.h"
#include "ui_page.h"
#include "ui_res_image.h"
#include "pm.h"
#include "ntc.h"
#include "display.h"

#define EXIT_TIME 20*1000

static uint32_t uptime = 0;

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
	widget_set_bg_img((widget_t*)UI_WIDGET_TEMP_r1, UI_RES_IMAGE_RING_A01+i);
	widget_set_bg_img((widget_t*)UI_WIDGET_TEMP_r2, UI_RES_IMAGE_RING_B01+i);
	widget_set_bg_img((widget_t*)UI_WIDGET_TEMP_r3, UI_RES_IMAGE_RING_C01+i);
	widget_set_bg_img((widget_t*)UI_WIDGET_TEMP_r4, UI_RES_IMAGE_RING_D01+i);
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

	// total
	if (total >=10) number2w(total, (char*)str);
	else
	{
		str[0] = 'a';
		str[1] = total % 10 + '0';
		str[2] = 'b';
		str[3] = 0;
	}
	imagetext_set_text(UI_WIDGET_TEMP_pt, (const char*)str);
	uint8_t temp = ntcStat.temperature;
	if (temp >= 99) temp = 99;
	str[0] = '0' + (temp / 10) % 10;
	str[1] = '0' + temp % 10;
	str[2] = 'c';
	str[3] = 0;
	imagetext_set_text(UI_WIDGET_TEMP_C, (char*)str);
	if (temp > 55)
	{
		widget_set_bg_img((widget_t*)UI_WIDGET_TEMP_logo, UI_RES_IMAGE_TEMP_LHIGH);
		widget_set_bg_img((widget_t*)UI_WIDGET_TEMP_label, UI_RES_IMAGE_TEMP_HIGH);
	}
	else
	{
		widget_set_bg_img((widget_t*)UI_WIDGET_TEMP_logo, UI_RES_IMAGE_TEMP_LLOW);
		widget_set_bg_img((widget_t*)UI_WIDGET_TEMP_label, UI_RES_IMAGE_TEMP_LOW);
	}
}
void ui_temp_on_page_open(uint8_t page) {

	update_ring();
	update_p();
	uptime = 0;
}

/**
 * @brief 定时器回调接口
 * @param millisecond   定时器周期，单位 ms
 * @param count  累计次数
 */
void ui_temp_on_timer(uint32_t millisecond, uint32_t count) {

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
	uptime += millisecond;
	if (uptime >= EXIT_TIME)
	{
		gui_context_open_page(UI_PAGE_POWER);
	}
}

/**
 * @brief 按钮点击事件回调接口
 * @param button   按钮控件指针
 * @param type     触摸事件类型
 */
void ui_temp_on_button_touch_event(button_t *button, touch_type_e type) {

}

/**
 * @brief 进度条回调接口
 * @param progress  进度条控件指针
 * @param val       进度值
 */
void ui_temp_on_progress_changed(progress_t *progress, uint16_t val) {

}

/**
 * @brief 动画播放结束回调接口
 * @param animation  动画控件指针
 */
void ui_temp_on_animation_play_end(animation_t *animation) {

}
