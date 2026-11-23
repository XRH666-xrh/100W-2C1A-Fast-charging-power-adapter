#ifndef _LOGIC_POWER_H_
#define _LOGIC_POWER_H_

#include "context/guicontext.h"

#define UI_WIDGET_POWER_pt                   ((imagetext_t*)gui_context_find_widget(3))
#define UI_WIDGET_POWER_PA                   ((image_t*)gui_context_find_widget(5))
#define UI_WIDGET_POWER_PC1                  ((image_t*)gui_context_find_widget(6))
#define UI_WIDGET_POWER_PC2                  ((image_t*)gui_context_find_widget(7))
#define UI_WIDGET_POWER_A                    ((imagetext_t*)gui_context_find_widget(8))
#define UI_WIDGET_POWER_C1                   ((imagetext_t*)gui_context_find_widget(9))
#define UI_WIDGET_POWER_C2                   ((imagetext_t*)gui_context_find_widget(10))
#define UI_WIDGET_POWER_phone1               ((image_t*)gui_context_find_widget(11))
#define UI_WIDGET_POWER_phone2               ((image_t*)gui_context_find_widget(12))
#define UI_WIDGET_POWER_ring                 ((image_t*)gui_context_find_widget(2))
#define UI_WIDGET_POWER_r1                   ((image_t*)gui_context_find_widget(4))
#define UI_WIDGET_POWER_r2                   ((image_t*)gui_context_find_widget(13))
#define UI_WIDGET_POWER_r3                   ((image_t*)gui_context_find_widget(14))
#define UI_WIDGET_POWER_r4                   ((image_t*)gui_context_find_widget(15))

void ui_power_on_page_open(uint8_t page);

/**
 * @brief 定时器回调接口
 * @param time   定时器周期，单位 ms
 * @param count  累计次数
 */
void ui_power_on_timer(uint32_t millisecond, uint32_t count);

/**
 * @brief 按钮点击事件回调接口
 * @param button   按钮控件指针
 * @param type     触摸事件类型
 */
void ui_power_on_button_touch_event(button_t *button, touch_type_e type);

/**
 * @brief 进度条回调接口
 * @param progress  进度条控件指针
 * @param val       进度值
 */
void ui_power_on_progress_changed(progress_t *progress, uint16_t val);

/**
 * @brief 动画播放结束回调接口
 * @param animation  动画控件指针
 */
void ui_power_on_animation_play_end(animation_t *animation);

#endif /*_LOGIC_POWER_H_*/
