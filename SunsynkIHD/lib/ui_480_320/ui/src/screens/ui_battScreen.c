// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.3.4
// LVGL version: 8.3.6
// Project name: Solar_UI_480x320

#include "../ui.h"

void ui_battScreen_screen_init(void)
{
    ui_battScreen = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_battScreen, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_bg_color(ui_battScreen, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_battScreen, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_battScreenHeading = lv_label_create(ui_battScreen);
    lv_obj_set_width(ui_battScreenHeading, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_battScreenHeading, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_battScreenHeading, 0);
    lv_obj_set_y(ui_battScreenHeading, -140);
    lv_obj_set_align(ui_battScreenHeading, LV_ALIGN_CENTER);
    lv_label_set_text(ui_battScreenHeading, "BATTERY");
    lv_obj_set_style_text_color(ui_battScreenHeading, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_battScreenHeading, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_decor(ui_battScreenHeading, LV_TEXT_DECOR_UNDERLINE, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_battScreenHeading, &lv_font_montserrat_30, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_battIcon2 = lv_img_create(ui_battScreen);
    lv_img_set_src(ui_battIcon2, &ui_img_670870020);
    lv_obj_set_width(ui_battIcon2, LV_SIZE_CONTENT);   /// 90
    lv_obj_set_height(ui_battIcon2, LV_SIZE_CONTENT);    /// 90
    lv_obj_set_x(ui_battIcon2, 183);
    lv_obj_set_y(ui_battIcon2, -104);
    lv_obj_set_align(ui_battIcon2, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_battIcon2, LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    lv_obj_clear_flag(ui_battIcon2, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_img_set_angle(ui_battIcon2, -900);

    ui_battCapLabel = lv_label_create(ui_battScreen);
    lv_obj_set_width(ui_battCapLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_battCapLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_battCapLabel, -300);
    lv_obj_set_y(ui_battCapLabel, -90);
    lv_obj_set_align(ui_battCapLabel, LV_ALIGN_RIGHT_MID);
    lv_label_set_text(ui_battCapLabel, "Capacity:");
    lv_obj_set_style_text_color(ui_battCapLabel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_battCapLabel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_battCapLabel, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_battCapValue = lv_label_create(ui_battScreen);
    lv_obj_set_width(ui_battCapValue, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_battCapValue, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_battCapValue, 190);
    lv_obj_set_y(ui_battCapValue, -90);
    lv_obj_set_align(ui_battCapValue, LV_ALIGN_LEFT_MID);
    lv_label_set_text(ui_battCapValue, "-- Ah");
    lv_obj_set_style_text_color(ui_battCapValue, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_battCapValue, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_battCapValue, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_battSocLabel = lv_label_create(ui_battScreen);
    lv_obj_set_width(ui_battSocLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_battSocLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_battSocLabel, -300);
    lv_obj_set_y(ui_battSocLabel, -60);
    lv_obj_set_align(ui_battSocLabel, LV_ALIGN_RIGHT_MID);
    lv_label_set_text(ui_battSocLabel, "State of charge:");
    lv_obj_set_style_text_color(ui_battSocLabel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_battSocLabel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_battSocLabel, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_battSocValue = lv_label_create(ui_battScreen);
    lv_obj_set_width(ui_battSocValue, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_battSocValue, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_battSocValue, 190);
    lv_obj_set_y(ui_battSocValue, -60);
    lv_obj_set_align(ui_battSocValue, LV_ALIGN_LEFT_MID);
    lv_label_set_text(ui_battSocValue, "-- %");
    lv_obj_set_style_text_color(ui_battSocValue, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_battSocValue, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_battSocValue, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_battPwrLabel = lv_label_create(ui_battScreen);
    lv_obj_set_width(ui_battPwrLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_battPwrLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_battPwrLabel, -300);
    lv_obj_set_y(ui_battPwrLabel, -30);
    lv_obj_set_align(ui_battPwrLabel, LV_ALIGN_RIGHT_MID);
    lv_label_set_text(ui_battPwrLabel, "Power:");
    lv_obj_set_style_text_color(ui_battPwrLabel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_battPwrLabel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_battPwrLabel, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_battPwrValue = lv_label_create(ui_battScreen);
    lv_obj_set_width(ui_battPwrValue, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_battPwrValue, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_battPwrValue, 190);
    lv_obj_set_y(ui_battPwrValue, -30);
    lv_obj_set_align(ui_battPwrValue, LV_ALIGN_LEFT_MID);
    lv_label_set_text(ui_battPwrValue, "-- W");
    lv_obj_set_style_text_color(ui_battPwrValue, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_battPwrValue, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_battPwrValue, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_battCurrLabel = lv_label_create(ui_battScreen);
    lv_obj_set_width(ui_battCurrLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_battCurrLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_battCurrLabel, -300);
    lv_obj_set_y(ui_battCurrLabel, 0);
    lv_obj_set_align(ui_battCurrLabel, LV_ALIGN_RIGHT_MID);
    lv_label_set_text(ui_battCurrLabel, "Current:");
    lv_obj_set_style_text_color(ui_battCurrLabel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_battCurrLabel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_battCurrLabel, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_battCurrValue = lv_label_create(ui_battScreen);
    lv_obj_set_width(ui_battCurrValue, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_battCurrValue, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_battCurrValue, 190);
    lv_obj_set_y(ui_battCurrValue, 0);
    lv_obj_set_align(ui_battCurrValue, LV_ALIGN_LEFT_MID);
    lv_label_set_text(ui_battCurrValue, "-- A");
    lv_obj_set_style_text_color(ui_battCurrValue, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_battCurrValue, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_battCurrValue, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_voltLabel = lv_label_create(ui_battScreen);
    lv_obj_set_width(ui_voltLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_voltLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_voltLabel, -300);
    lv_obj_set_y(ui_voltLabel, 30);
    lv_obj_set_align(ui_voltLabel, LV_ALIGN_RIGHT_MID);
    lv_label_set_text(ui_voltLabel, "Voltage:");
    lv_obj_set_style_text_color(ui_voltLabel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_voltLabel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_voltLabel, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_battVoltValue = lv_label_create(ui_battScreen);
    lv_obj_set_width(ui_battVoltValue, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_battVoltValue, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_battVoltValue, 190);
    lv_obj_set_y(ui_battVoltValue, 30);
    lv_obj_set_align(ui_battVoltValue, LV_ALIGN_LEFT_MID);
    lv_label_set_text(ui_battVoltValue, "-- V");
    lv_obj_set_style_text_color(ui_battVoltValue, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_battVoltValue, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_battVoltValue, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_tempLabel = lv_label_create(ui_battScreen);
    lv_obj_set_width(ui_tempLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_tempLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_tempLabel, -300);
    lv_obj_set_y(ui_tempLabel, 60);
    lv_obj_set_align(ui_tempLabel, LV_ALIGN_RIGHT_MID);
    lv_label_set_text(ui_tempLabel, "Temperature:");
    lv_obj_set_style_text_color(ui_tempLabel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_tempLabel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_tempLabel, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_battTempValue = lv_label_create(ui_battScreen);
    lv_obj_set_width(ui_battTempValue, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_battTempValue, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_battTempValue, 190);
    lv_obj_set_y(ui_battTempValue, 60);
    lv_obj_set_align(ui_battTempValue, LV_ALIGN_LEFT_MID);
    lv_label_set_text(ui_battTempValue, "-- °C");
    lv_obj_set_style_text_color(ui_battTempValue, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_battTempValue, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_battTempValue, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_add_event_cb(ui_battScreen, ui_event_battScreen, LV_EVENT_ALL, NULL);

}