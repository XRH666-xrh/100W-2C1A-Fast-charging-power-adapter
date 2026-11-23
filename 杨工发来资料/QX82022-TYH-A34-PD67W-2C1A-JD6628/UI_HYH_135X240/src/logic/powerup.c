#include "logic/powerup.h"
#include "ui_page.h"
#include "ui_res_image.h"
#include "pm.h"
#include "display.h"

#define EXIT_TIME 30*1000
#define UPDATE_TIME 100

static uint32_t uptime = 0;
static uint32_t updatetime = 0;

static void update(void)
{
	static uint8_t dir = 0;
	static uint8_t p = 0;
	widget_set_bg_img((widget_t*)UI_WIDGET_POWERUP_logo, p+UI_RES_IMAGE_LOGO_LOGO01);
	if (dir == 0)
	{
		p+=2;
		if (p>=UI_RES_IMAGE_LOGO_LOGO22)
		{
			dir = 1;
			p = UI_RES_IMAGE_LOGO_LOGO20;
		}
	}
	else
	{
		p-=2;
		if (p<UI_RES_IMAGE_LOGO_LOGO01)
		{
			dir = 0;
			p = UI_RES_IMAGE_LOGO_LOGO03;
		}
	}
	uint8_t idleing = 1;
	for (uint8_t i=0; i<3; i++)
	{
		if (ports[i].enabled && ports[i].power)
		{
			idleing = 0;
			if (!ports[i].lastenabled && powerstat != POWER_UP)
			{
				video_init();
				powerstat = POWER_UP;
			}
			if (powerstat == POWER_UP) gui_context_open_page(UI_PAGE_POWER);
			ports[i].lastenabled = PORT_REMOVE_DELAY_CNT;
		}
		else if (ports[i].lastenabled)
		{
			ports[i].lastenabled--;
		}
	}
	if (idleing)
	{
		if (powerstat == POWER_FORCE_OFF)
		{
			powerstat = POWER_IDLE;
		}
	}
}

void ui_powerup_on_page_open(uint8_t page) {

	widget_set_bg_img((widget_t*)UI_WIDGET_POWERUP_Image1, UI_RES_IMAGE_LOGO_LABEL);
	uptime = 0;
	updatetime = 0;
	update();
}

/**
 * @brief 定时器回调接口
 * @param millisecond   定时器周期，单位 ms
 * @param count  累计次数
 */
void ui_powerup_on_timer(uint32_t millisecond, uint32_t count) {

	uptime += millisecond;
	if (uptime >= EXIT_TIME)
	{
		powerstat = POWER_IDLE;
		video_deinit();
	}
	updatetime += millisecond;
	if (updatetime >= UPDATE_TIME)
	{
		update();
		updatetime = 0;
	}
}

/**
 * @brief 按钮点击事件回调接口
 * @param button   按钮控件指针
 * @param type     触摸事件类型
 */
void ui_powerup_on_button_touch_event(button_t *button, touch_type_e type) {

}

/**
 * @brief 进度条回调接口
 * @param progress  进度条控件指针
 * @param val       进度值
 */
void ui_powerup_on_progress_changed(progress_t *progress, uint16_t val) {

}

/**
 * @brief 动画播放结束回调接口
 * @param animation  动画控件指针
 */
void ui_powerup_on_animation_play_end(animation_t *animation) {

}
