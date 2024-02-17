// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.3.4
// LVGL version: 8.3.6
// Project name: Solar_UI_480x320

#include "../ui.h"

void ui_inverterScreen_screen_init(void)
{
    ui_inverterScreen = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_inverterScreen, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_bg_color(ui_inverterScreen, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_inverterScreen, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_invScreenHeading = lv_label_create(ui_inverterScreen);
    lv_obj_set_width(ui_invScreenHeading, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_invScreenHeading, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_invScreenHeading, 0);
    lv_obj_set_y(ui_invScreenHeading, -140);
    lv_obj_set_align(ui_invScreenHeading, LV_ALIGN_CENTER);
    lv_label_set_text(ui_invScreenHeading, "INVERTER");
    lv_obj_set_style_text_color(ui_invScreenHeading, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_invScreenHeading, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_invScreenHeading, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_add_event_cb(ui_inverterScreen, ui_event_inverterScreen, LV_EVENT_ALL, NULL);

}