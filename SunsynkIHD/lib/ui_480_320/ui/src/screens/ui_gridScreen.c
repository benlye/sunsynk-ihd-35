// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.3.4
// LVGL version: 8.3.6
// Project name: Solar_UI_480x320

#include "../ui.h"

void ui_gridScreen_screen_init(void)
{
    ui_gridScreen = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_gridScreen, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_bg_color(ui_gridScreen, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_gridScreen, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_gridScreenHeading = lv_label_create(ui_gridScreen);
    lv_obj_set_width(ui_gridScreenHeading, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_gridScreenHeading, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_gridScreenHeading, 0);
    lv_obj_set_y(ui_gridScreenHeading, -140);
    lv_obj_set_align(ui_gridScreenHeading, LV_ALIGN_CENTER);
    lv_label_set_text(ui_gridScreenHeading, "GRID");
    lv_obj_set_style_text_color(ui_gridScreenHeading, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_gridScreenHeading, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_gridScreenHeading, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_add_event_cb(ui_gridScreen, ui_event_gridScreen, LV_EVENT_ALL, NULL);

}
