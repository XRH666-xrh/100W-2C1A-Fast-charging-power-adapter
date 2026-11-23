#include "logic/P2.h"
#include "ui_page.h"
#include "ui_res_image.h"
#include "pm.h"

/* Grid slots inside ui.bin:
 *   0 = C1 (top-left)   1 = C2 (top-right)
 *   2 = C3 (bottom-left) 3 = A (bottom-right)
 * This board is 2C1A: ports[0]=C1, ports[1]=C2, ports[2]=A1
 * so slot 2 (C3) has no hardware and is always drawn as "not inserted".
 * For a 3C1A board set slot_port[] = {0,1,2,3} instead. */
static const int8_t slot_port[4] = {0, 1, -1, 2};

/* glyph string shown when a port is empty (special font characters) */
static const char IDLE_AV[] = "zzyzvwzyza";

static uint8_t bgs[4] = {0};

static void number2w(uint16_t total, char* str)
{
	uint8_t idx = 0;
	if (total == 0)
	{
		str[idx++] = 'x';
		str[idx++] = 'x';
		str[idx++] = 'x';
		str[idx++] = 'y';
		str[idx++] = 0;
	}
	else
	{
		if (total >=100)
		{
			str[0] = '0' + (total / 100) % 10;
			str[1] = '0' + (total / 10) % 10;
			str[2] = '0' + total % 10;
			str[3] = 'w';
			str[4] = 0;
		}
		else if (total >= 10)
		{
			str[0] = 'h';
			str[1] = '0' + (total / 10) % 10;
			str[2] = '0' + total % 10;
			str[3] = 'w';
			str[4] = 'h';
			str[5] = 0;
		}
		else
		{
			str[0] = ' ';
			str[1] = '0' + total;
			str[2] = 'w';
			str[3] = ' ';
			str[4] = 0;
		}
	}
}

static void port2av(port_stat_t port, char* str)
{
	uint8_t idx = 0;
	if (port.power == 0)
	{
		str[idx++] = 'z';
		str[idx++] = 'z';
		str[idx++] = 'y';
		str[idx++] = 'z';
		str[idx++] = 'v';
		str[idx++] = 'w';
		str[idx++] = 'z';
		str[idx++] = 'y';
		str[idx++] = 'z';
		str[idx++] = 'a';
		str[idx++] = 0;
		return;
	}
	str[idx++] = port.voltage >= 1000? '0'+((port.voltage/1000)%10) : ' ';
	str[idx++] = (port.voltage / 100) %10 + '0';
	str[idx++] = '.';
	str[idx++] = (port.voltage / 10) %10 + '0';
	str[idx++] = 'V';
	str[idx++] = '/';
	str[idx++] = (port.current / 100) %10 + '0';
	str[idx++] = '.';
	str[idx++] = (port.current / 10) %10 + '0';
	str[idx++] = 'A';
	str[idx++] = 0;
}

/* need_power: the A slot only appears when it really delivers power */
static void upd_slot(uint8_t slot, image_t* box, imagetext_t* wt, imagetext_t* at,
                     uint16_t bg_on, uint16_t bg_off, uint8_t need_power)
{
	uint8_t str[7] = {0};
	int8_t src = slot_port[slot];
	uint8_t on = 0;

	if (src >= 0 && src < PORT_NUM && ports[src].enabled)
	{
		on = need_power ? (ports[src].power != 0) : 1;
	}

	if (on)
	{
		if (!bgs[slot]) widget_set_bg_img((widget_t*)box, bg_on);
		bgs[slot] = 1;
		number2w(ports[src].power, (char*)str);
		imagetext_set_text(wt, (const char*)str);
		port2av(ports[src], str);
		imagetext_set_text(at, (const char*)str);
	}
	else if (bgs[slot])
	{
		bgs[slot] = 0;
		widget_set_bg_img((widget_t*)box, bg_off);
		number2w(0, (char*)str);
		imagetext_set_text(wt, (const char*)str);
		imagetext_set_text(at, IDLE_AV);
	}
}

static void update_p(void)
{
	upd_slot(0, UI_WIDGET_P2_c1, UI_WIDGET_P2_w1, UI_WIDGET_P2_a1,
	         UI_RES_IMAGE_P2_C1, UI_RES_IMAGE_P2_C1G1, 0);
	upd_slot(1, UI_WIDGET_P2_c2, UI_WIDGET_P2_w2, UI_WIDGET_P2_a2,
	         UI_RES_IMAGE_P2_C2, UI_RES_IMAGE_P2_C2G2, 0);
	upd_slot(2, UI_WIDGET_P2_c3, UI_WIDGET_P2_w3, UI_WIDGET_P2_a3,
	         UI_RES_IMAGE_P2_C3, UI_RES_IMAGE_P2_C3G3, 0);
	upd_slot(3, UI_WIDGET_P2_c4, UI_WIDGET_P2_w4, UI_WIDGET_P2_a4,
	         UI_RES_IMAGE_P2_A,  UI_RES_IMAGE_P2_AG1, 1);
}

void ui_P2_on_page_open(uint8_t page) {
	bgs[0] = bgs[1] = bgs[2] = bgs[3] = 0;
	update_p();
}

/**
 * @brief timer callback
 * @param millisecond   timer period in ms
 * @param count         accumulated count
 */
void ui_P2_on_timer(uint32_t millisecond, uint32_t count) {
	static uint8_t time = 0;
	if (time++<80) return;
	time = 0;
	update_p();
}

void ui_P2_on_button_touch_event(button_t *button, touch_type_e type) {

}

void ui_P2_on_progress_changed(progress_t *progress, uint16_t val) {

}

void ui_P2_on_animation_play_end(animation_t *animation) {

}
