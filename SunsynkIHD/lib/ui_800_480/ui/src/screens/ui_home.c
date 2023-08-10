// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.3.1
// LVGL version: 8.3.6
// Project name: Solar_UI_800x480

#include "../ui.h"

void ui_home_screen_init(void)
{
    ui_home = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_home, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_bg_color(ui_home, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_home, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_pvIcon = lv_img_create(ui_home);
    lv_img_set_src(ui_pvIcon, &ui_img_1549501229);
    lv_obj_set_width(ui_pvIcon, LV_SIZE_CONTENT);   /// 100
    lv_obj_set_height(ui_pvIcon, LV_SIZE_CONTENT);    /// 100
    lv_obj_set_x(ui_pvIcon, -200);
    lv_obj_set_y(ui_pvIcon, -100);
    lv_obj_set_align(ui_pvIcon, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_pvIcon, LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    lv_obj_clear_flag(ui_pvIcon, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_pvWatts = lv_label_create(ui_home);
    lv_obj_set_width(ui_pvWatts, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_pvWatts, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_pvWatts, -200);
    lv_obj_set_y(ui_pvWatts, -190);
    lv_obj_set_align(ui_pvWatts, LV_ALIGN_CENTER);
    lv_label_set_text(ui_pvWatts, " - ");
    lv_obj_set_style_text_color(ui_pvWatts, lv_color_hex(0xA2A2A2), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_pvWatts, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui_pvWatts, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_pvWatts, &ui_font_mont_64, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_pvWatts, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_pvWatts, 90, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_pvWatts, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_pvWatts, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_pvWattsUnit = lv_label_create(ui_pvWatts);
    lv_obj_set_width(ui_pvWattsUnit, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_pvWattsUnit, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_pvWattsUnit, 80);
    lv_obj_set_y(ui_pvWattsUnit, 0);
    lv_obj_set_align(ui_pvWattsUnit, LV_ALIGN_RIGHT_MID);
    lv_label_set_text(ui_pvWattsUnit, "W");

    ui_pvTotal = lv_label_create(ui_home);
    lv_obj_set_width(ui_pvTotal, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_pvTotal, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_pvTotal, -320);
    lv_obj_set_y(ui_pvTotal, -90);
    lv_obj_set_align(ui_pvTotal, LV_ALIGN_CENTER);
    lv_label_set_text(ui_pvTotal, " - ");
    lv_obj_set_style_text_color(ui_pvTotal, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_pvTotal, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_pvTotal, &lv_font_montserrat_46, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_pvTotal, 20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_pvTotal, 20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_pvTotal, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_pvTotal, 25, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_pvTotalUnits = lv_label_create(ui_pvTotal);
    lv_obj_set_width(ui_pvTotalUnits, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_pvTotalUnits, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_pvTotalUnits, 0);
    lv_obj_set_y(ui_pvTotalUnits, 32);
    lv_obj_set_align(ui_pvTotalUnits, LV_ALIGN_CENTER);
    lv_label_set_text(ui_pvTotalUnits, "kWh");
    lv_obj_set_style_text_color(ui_pvTotalUnits, lv_color_hex(0xA2A2A2), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_pvTotalUnits, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_pvTotalUnits, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_loadWatts = lv_label_create(ui_home);
    lv_obj_set_width(ui_loadWatts, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_loadWatts, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_loadWatts, 200);
    lv_obj_set_y(ui_loadWatts, 190);
    lv_obj_set_align(ui_loadWatts, LV_ALIGN_CENTER);
    lv_label_set_text(ui_loadWatts, " - ");
    lv_obj_set_style_text_color(ui_loadWatts, lv_color_hex(0xA2A2A2), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_loadWatts, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui_loadWatts, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_loadWatts, &ui_font_mont_64, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_loadWatts, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_loadWatts, 90, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_loadWatts, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_loadWatts, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_loadWattsUnit = lv_label_create(ui_loadWatts);
    lv_obj_set_width(ui_loadWattsUnit, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_loadWattsUnit, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_loadWattsUnit, 80);
    lv_obj_set_y(ui_loadWattsUnit, 0);
    lv_obj_set_align(ui_loadWattsUnit, LV_ALIGN_RIGHT_MID);
    lv_label_set_text(ui_loadWattsUnit, "W");

    ui_battIcon = lv_img_create(ui_home);
    lv_img_set_src(ui_battIcon, &ui_img_670870020);
    lv_obj_set_width(ui_battIcon, LV_SIZE_CONTENT);   /// 90
    lv_obj_set_height(ui_battIcon, LV_SIZE_CONTENT);    /// 90
    lv_obj_set_x(ui_battIcon, -200);
    lv_obj_set_y(ui_battIcon, 100);
    lv_obj_set_align(ui_battIcon, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_battIcon, LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    lv_obj_clear_flag(ui_battIcon, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_img_set_angle(ui_battIcon, -900);

    ui_battSoc = lv_label_create(ui_home);
    lv_obj_set_width(ui_battSoc, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_battSoc, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_battSoc, 235);
    lv_obj_set_y(ui_battSoc, 105);
    lv_obj_set_align(ui_battSoc, LV_ALIGN_LEFT_MID);
    lv_label_set_text(ui_battSoc, " - %");
    lv_obj_set_style_text_color(ui_battSoc, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_battSoc, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui_battSoc, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_battSoc, &lv_font_montserrat_42, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_battWatts = lv_label_create(ui_home);
    lv_obj_set_width(ui_battWatts, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_battWatts, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_battWatts, -200);
    lv_obj_set_y(ui_battWatts, 190);
    lv_obj_set_align(ui_battWatts, LV_ALIGN_CENTER);
    lv_label_set_text(ui_battWatts, " - ");
    lv_obj_set_style_text_color(ui_battWatts, lv_color_hex(0xA2A2A2), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_battWatts, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui_battWatts, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_battWatts, &ui_font_mont_64, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_battWatts, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_battWatts, 90, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_battWatts, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_battWatts, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_battWattsUnit = lv_label_create(ui_battWatts);
    lv_obj_set_width(ui_battWattsUnit, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_battWattsUnit, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_battWattsUnit, 80);
    lv_obj_set_y(ui_battWattsUnit, 0);
    lv_obj_set_align(ui_battWattsUnit, LV_ALIGN_RIGHT_MID);
    lv_label_set_text(ui_battWattsUnit, "W");

    ui_battDailyCharge = lv_label_create(ui_home);
    lv_obj_set_width(ui_battDailyCharge, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_battDailyCharge, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_battDailyCharge, -692);
    lv_obj_set_y(ui_battDailyCharge, 122);
    lv_obj_set_align(ui_battDailyCharge, LV_ALIGN_RIGHT_MID);
    lv_label_set_text(ui_battDailyCharge, " - ");
    lv_obj_set_style_text_color(ui_battDailyCharge, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_battDailyCharge, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_battDailyCharge, &lv_font_montserrat_32, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_battDailyDischargeUnit = lv_label_create(ui_home);
    lv_obj_set_width(ui_battDailyDischargeUnit, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_battDailyDischargeUnit, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_battDailyDischargeUnit, -640);
    lv_obj_set_y(ui_battDailyDischargeUnit, 93);
    lv_obj_set_align(ui_battDailyDischargeUnit, LV_ALIGN_RIGHT_MID);
    lv_label_set_text(ui_battDailyDischargeUnit, "kWh");
    lv_obj_set_style_text_color(ui_battDailyDischargeUnit, lv_color_hex(0xA2A2A2), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_battDailyDischargeUnit, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_battDailyDischargeUnit, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_battDailyDischarge = lv_label_create(ui_home);
    lv_obj_set_width(ui_battDailyDischarge, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_battDailyDischarge, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_battDailyDischarge, -692);
    lv_obj_set_y(ui_battDailyDischarge, 88);
    lv_obj_set_align(ui_battDailyDischarge, LV_ALIGN_RIGHT_MID);
    lv_label_set_text(ui_battDailyDischarge, " - ");
    lv_obj_set_style_text_color(ui_battDailyDischarge, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_battDailyDischarge, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_battDailyDischarge, &lv_font_montserrat_32, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_battDailyChargeUnit = lv_label_create(ui_home);
    lv_obj_set_width(ui_battDailyChargeUnit, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_battDailyChargeUnit, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_battDailyChargeUnit, -640);
    lv_obj_set_y(ui_battDailyChargeUnit, 127);
    lv_obj_set_align(ui_battDailyChargeUnit, LV_ALIGN_RIGHT_MID);
    lv_label_set_text(ui_battDailyChargeUnit, "kWh");
    lv_obj_set_style_text_color(ui_battDailyChargeUnit, lv_color_hex(0xA2A2A2), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_battDailyChargeUnit, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_battDailyChargeUnit, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_battDischargeIcon = lv_img_create(ui_home);
    lv_img_set_src(ui_battDischargeIcon, &ui_img_705529016);
    lv_obj_set_width(ui_battDischargeIcon, LV_SIZE_CONTENT);   /// 30
    lv_obj_set_height(ui_battDischargeIcon, LV_SIZE_CONTENT);    /// 30
    lv_obj_set_x(ui_battDischargeIcon, -373);
    lv_obj_set_y(ui_battDischargeIcon, 86);
    lv_obj_set_align(ui_battDischargeIcon, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_battDischargeIcon, LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    lv_obj_clear_flag(ui_battDischargeIcon, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_img_set_angle(ui_battDischargeIcon, -900);
    lv_obj_set_style_blend_mode(ui_battDischargeIcon, LV_BLEND_MODE_NORMAL, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor(ui_battDischargeIcon, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor_opa(ui_battDischargeIcon, 100, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_battChargeIcon = lv_img_create(ui_home);
    lv_img_set_src(ui_battChargeIcon, &ui_img_705529016);
    lv_obj_set_width(ui_battChargeIcon, LV_SIZE_CONTENT);   /// 30
    lv_obj_set_height(ui_battChargeIcon, LV_SIZE_CONTENT);    /// 30
    lv_obj_set_x(ui_battChargeIcon, -373);
    lv_obj_set_y(ui_battChargeIcon, 130);
    lv_obj_set_align(ui_battChargeIcon, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_battChargeIcon, LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    lv_obj_clear_flag(ui_battChargeIcon, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_img_set_angle(ui_battChargeIcon, 900);
    lv_obj_set_style_img_recolor(ui_battChargeIcon, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor_opa(ui_battChargeIcon, 100, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_gridIcon = lv_img_create(ui_home);
    lv_img_set_src(ui_gridIcon, &ui_img_264836261);
    lv_obj_set_width(ui_gridIcon, LV_SIZE_CONTENT);   /// 100
    lv_obj_set_height(ui_gridIcon, LV_SIZE_CONTENT);    /// 100
    lv_obj_set_x(ui_gridIcon, 200);
    lv_obj_set_y(ui_gridIcon, -100);
    lv_obj_set_align(ui_gridIcon, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_gridIcon, LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    lv_obj_clear_flag(ui_gridIcon, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_gridWatts = lv_label_create(ui_home);
    lv_obj_set_width(ui_gridWatts, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_gridWatts, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_gridWatts, 200);
    lv_obj_set_y(ui_gridWatts, -190);
    lv_obj_set_align(ui_gridWatts, LV_ALIGN_CENTER);
    lv_label_set_text(ui_gridWatts, " - ");
    lv_obj_set_style_text_color(ui_gridWatts, lv_color_hex(0xA2A2A2), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_gridWatts, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui_gridWatts, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_gridWatts, &ui_font_mont_64, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_gridWatts, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_gridWatts, 90, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_gridWatts, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_gridWatts, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_gridWattsUnit = lv_label_create(ui_gridWatts);
    lv_obj_set_width(ui_gridWattsUnit, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_gridWattsUnit, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_gridWattsUnit, 80);
    lv_obj_set_y(ui_gridWattsUnit, 0);
    lv_obj_set_align(ui_gridWattsUnit, LV_ALIGN_RIGHT_MID);
    lv_label_set_text(ui_gridWattsUnit, "W");

    ui_gridDailyExport = lv_label_create(ui_home);
    lv_obj_set_width(ui_gridDailyExport, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_gridDailyExport, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_gridDailyExport, -89);
    lv_obj_set_y(ui_gridDailyExport, -118);
    lv_obj_set_align(ui_gridDailyExport, LV_ALIGN_RIGHT_MID);
    lv_label_set_text(ui_gridDailyExport, " - ");
    lv_obj_set_style_text_color(ui_gridDailyExport, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_gridDailyExport, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_gridDailyExport, &lv_font_montserrat_32, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_gridDailyExportUnit = lv_label_create(ui_home);
    lv_obj_set_width(ui_gridDailyExportUnit, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_gridDailyExportUnit, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_gridDailyExportUnit, -37);
    lv_obj_set_y(ui_gridDailyExportUnit, -113);
    lv_obj_set_align(ui_gridDailyExportUnit, LV_ALIGN_RIGHT_MID);
    lv_label_set_text(ui_gridDailyExportUnit, "kWh");
    lv_obj_set_style_text_color(ui_gridDailyExportUnit, lv_color_hex(0xA2A2A2), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_gridDailyExportUnit, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_gridDailyExportUnit, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_gridDailyImport = lv_label_create(ui_home);
    lv_obj_set_width(ui_gridDailyImport, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_gridDailyImport, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_gridDailyImport, -88);
    lv_obj_set_y(ui_gridDailyImport, -84);
    lv_obj_set_align(ui_gridDailyImport, LV_ALIGN_RIGHT_MID);
    lv_label_set_text(ui_gridDailyImport, " - ");
    lv_obj_set_style_text_color(ui_gridDailyImport, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_gridDailyImport, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_gridDailyImport, &lv_font_montserrat_32, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_gridDailyImportUnit = lv_label_create(ui_home);
    lv_obj_set_width(ui_gridDailyImportUnit, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_gridDailyImportUnit, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_gridDailyImportUnit, -37);
    lv_obj_set_y(ui_gridDailyImportUnit, -79);
    lv_obj_set_align(ui_gridDailyImportUnit, LV_ALIGN_RIGHT_MID);
    lv_label_set_text(ui_gridDailyImportUnit, "kWh");
    lv_obj_set_style_text_color(ui_gridDailyImportUnit, lv_color_hex(0xA2A2A2), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_gridDailyImportUnit, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_gridDailyImportUnit, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_gridExportArrow = lv_img_create(ui_home);
    lv_img_set_src(ui_gridExportArrow, &ui_img_705529016);
    lv_obj_set_width(ui_gridExportArrow, LV_SIZE_CONTENT);   /// 30
    lv_obj_set_height(ui_gridExportArrow, LV_SIZE_CONTENT);    /// 30
    lv_obj_set_x(ui_gridExportArrow, 375);
    lv_obj_set_y(ui_gridExportArrow, -120);
    lv_obj_set_align(ui_gridExportArrow, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_gridExportArrow, LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    lv_obj_clear_flag(ui_gridExportArrow, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_img_set_angle(ui_gridExportArrow, -900);
    lv_obj_set_style_img_recolor(ui_gridExportArrow, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor_opa(ui_gridExportArrow, 100, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_gridImportArrow = lv_img_create(ui_home);
    lv_img_set_src(ui_gridImportArrow, &ui_img_705529016);
    lv_obj_set_width(ui_gridImportArrow, LV_SIZE_CONTENT);   /// 30
    lv_obj_set_height(ui_gridImportArrow, LV_SIZE_CONTENT);    /// 30
    lv_obj_set_x(ui_gridImportArrow, 374);
    lv_obj_set_y(ui_gridImportArrow, -76);
    lv_obj_set_align(ui_gridImportArrow, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_gridImportArrow, LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    lv_obj_clear_flag(ui_gridImportArrow, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_img_set_angle(ui_gridImportArrow, 900);
    lv_obj_set_style_img_recolor(ui_gridImportArrow, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor_opa(ui_gridImportArrow, 100, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_loadIcon = lv_img_create(ui_home);
    lv_img_set_src(ui_loadIcon, &ui_img_1076282785);
    lv_obj_set_width(ui_loadIcon, LV_SIZE_CONTENT);   /// 100
    lv_obj_set_height(ui_loadIcon, LV_SIZE_CONTENT);    /// 100
    lv_obj_set_x(ui_loadIcon, 200);
    lv_obj_set_y(ui_loadIcon, 100);
    lv_obj_set_align(ui_loadIcon, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_loadIcon, LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    lv_obj_clear_flag(ui_loadIcon, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_loadTotal = lv_label_create(ui_home);
    lv_obj_set_width(ui_loadTotal, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_loadTotal, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_loadTotal, 320);
    lv_obj_set_y(ui_loadTotal, 106);
    lv_obj_set_align(ui_loadTotal, LV_ALIGN_CENTER);
    lv_label_set_text(ui_loadTotal, " - ");
    lv_obj_set_style_text_color(ui_loadTotal, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_loadTotal, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_loadTotal, &lv_font_montserrat_46, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_loadTotal, 20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_loadTotal, 20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_loadTotal, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_loadTotal, 25, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_loadTotalUnits = lv_label_create(ui_loadTotal);
    lv_obj_set_width(ui_loadTotalUnits, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_loadTotalUnits, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_loadTotalUnits, 0);
    lv_obj_set_y(ui_loadTotalUnits, 32);
    lv_obj_set_align(ui_loadTotalUnits, LV_ALIGN_CENTER);
    lv_label_set_text(ui_loadTotalUnits, "kWh");
    lv_obj_set_style_text_color(ui_loadTotalUnits, lv_color_hex(0xA3A2A2), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_loadTotalUnits, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_loadTotalUnits, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Inverter = lv_obj_create(ui_home);
    lv_obj_set_width(ui_Inverter, 130);
    lv_obj_set_height(ui_Inverter, 130);
    lv_obj_set_align(ui_Inverter, LV_ALIGN_CENTER);
    lv_obj_clear_flag(ui_Inverter, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_bg_color(ui_Inverter, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Inverter, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_Inverter, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_Inverter, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_Inverter, 6, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_SunsynkLogo = lv_img_create(ui_Inverter);
    lv_img_set_src(ui_SunsynkLogo, &ui_img_1416811391);
    lv_obj_set_width(ui_SunsynkLogo, LV_SIZE_CONTENT);   /// 39
    lv_obj_set_height(ui_SunsynkLogo, LV_SIZE_CONTENT);    /// 42
    lv_obj_set_align(ui_SunsynkLogo, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_SunsynkLogo, LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    lv_obj_clear_flag(ui_SunsynkLogo, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_img_set_zoom(ui_SunsynkLogo, 110);

    ui_time = lv_label_create(ui_home);
    lv_obj_set_width(ui_time, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_time, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_time, 0);
    lv_obj_set_y(ui_time, -5);
    lv_obj_set_align(ui_time, LV_ALIGN_BOTTOM_MID);
    lv_label_set_text(ui_time, "--:--");
    lv_obj_set_style_text_color(ui_time, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_time, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_time, &lv_font_montserrat_42, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_syncing = lv_img_create(ui_home);
    lv_img_set_src(ui_syncing, &ui_img_1765460539);
    lv_obj_set_width(ui_syncing, LV_SIZE_CONTENT);   /// 24
    lv_obj_set_height(ui_syncing, LV_SIZE_CONTENT);    /// 24
    lv_obj_set_x(ui_syncing, 35);
    lv_obj_set_y(ui_syncing, 11);
    lv_obj_add_flag(ui_syncing, LV_OBJ_FLAG_HIDDEN | LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    lv_obj_clear_flag(ui_syncing, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_wifiHigh = lv_img_create(ui_home);
    lv_img_set_src(ui_wifiHigh, &ui_img_1683162281);
    lv_obj_set_width(ui_wifiHigh, LV_SIZE_CONTENT);   /// 24
    lv_obj_set_height(ui_wifiHigh, LV_SIZE_CONTENT);    /// 24
    lv_obj_set_x(ui_wifiHigh, 10);
    lv_obj_set_y(ui_wifiHigh, 10);
    lv_obj_add_flag(ui_wifiHigh, LV_OBJ_FLAG_HIDDEN | LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    lv_obj_clear_flag(ui_wifiHigh, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_wifiMed = lv_img_create(ui_home);
    lv_img_set_src(ui_wifiMed, &ui_img_1288183160);
    lv_obj_set_width(ui_wifiMed, LV_SIZE_CONTENT);   /// 24
    lv_obj_set_height(ui_wifiMed, LV_SIZE_CONTENT);    /// 24
    lv_obj_set_x(ui_wifiMed, 10);
    lv_obj_set_y(ui_wifiMed, 10);
    lv_obj_add_flag(ui_wifiMed, LV_OBJ_FLAG_HIDDEN | LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    lv_obj_clear_flag(ui_wifiMed, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_wifiLow = lv_img_create(ui_home);
    lv_img_set_src(ui_wifiLow, &ui_img_811431924);
    lv_obj_set_width(ui_wifiLow, LV_SIZE_CONTENT);   /// 24
    lv_obj_set_height(ui_wifiLow, LV_SIZE_CONTENT);    /// 24
    lv_obj_set_x(ui_wifiLow, 10);
    lv_obj_set_y(ui_wifiLow, 10);
    lv_obj_add_flag(ui_wifiLow, LV_OBJ_FLAG_HIDDEN | LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    lv_obj_clear_flag(ui_wifiLow, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_settingsIcon = lv_img_create(ui_home);
    lv_img_set_src(ui_settingsIcon, &ui_img_226794803);
    lv_obj_set_width(ui_settingsIcon, LV_SIZE_CONTENT);   /// 24
    lv_obj_set_height(ui_settingsIcon, LV_SIZE_CONTENT);    /// 24
    lv_obj_set_x(ui_settingsIcon, 223);
    lv_obj_set_y(ui_settingsIcon, -143);
    lv_obj_set_align(ui_settingsIcon, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_settingsIcon, LV_OBJ_FLAG_HIDDEN | LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    lv_obj_clear_flag(ui_settingsIcon, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_img_recolor(ui_settingsIcon, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor_opa(ui_settingsIcon, 128, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Image3 = lv_img_create(ui_home);
    lv_img_set_src(ui_Image3, &ui_img_207095971);
    lv_obj_set_width(ui_Image3, LV_SIZE_CONTENT);   /// 150
    lv_obj_set_height(ui_Image3, LV_SIZE_CONTENT);    /// 50
    lv_obj_set_x(ui_Image3, -140);
    lv_obj_set_y(ui_Image3, 25);
    lv_obj_set_align(ui_Image3, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_Image3, LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    lv_obj_clear_flag(ui_Image3, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_img_set_zoom(ui_Image3, 228);

    ui_Image1 = lv_img_create(ui_home);
    lv_img_set_src(ui_Image1, &ui_img_207095971);
    lv_obj_set_width(ui_Image1, LV_SIZE_CONTENT);   /// 150
    lv_obj_set_height(ui_Image1, LV_SIZE_CONTENT);    /// 50
    lv_obj_set_x(ui_Image1, 138);
    lv_obj_set_y(ui_Image1, -25);
    lv_obj_set_align(ui_Image1, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_Image1, LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    lv_obj_clear_flag(ui_Image1, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_img_set_angle(ui_Image1, 1800);
    lv_img_set_zoom(ui_Image1, 228);

    ui_Image4 = lv_img_create(ui_home);
    lv_img_set_src(ui_Image4, &ui_img_1192762099);
    lv_obj_set_width(ui_Image4, LV_SIZE_CONTENT);   /// 150
    lv_obj_set_height(ui_Image4, LV_SIZE_CONTENT);    /// 50
    lv_obj_set_x(ui_Image4, 139);
    lv_obj_set_y(ui_Image4, 25);
    lv_obj_set_align(ui_Image4, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_Image4, LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    lv_obj_clear_flag(ui_Image4, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_img_set_zoom(ui_Image4, 228);

    ui_Image2 = lv_img_create(ui_home);
    lv_img_set_src(ui_Image2, &ui_img_1192762099);
    lv_obj_set_width(ui_Image2, LV_SIZE_CONTENT);   /// 150
    lv_obj_set_height(ui_Image2, LV_SIZE_CONTENT);    /// 50
    lv_obj_set_x(ui_Image2, -140);
    lv_obj_set_y(ui_Image2, -25);
    lv_obj_set_align(ui_Image2, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_Image2, LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    lv_obj_clear_flag(ui_Image2, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_img_set_angle(ui_Image2, 1800);
    lv_img_set_zoom(ui_Image2, 228);

    ui_infoPanel = lv_obj_create(ui_home);
    lv_obj_set_width(ui_infoPanel, 800);
    lv_obj_set_height(ui_infoPanel, 480);
    lv_obj_set_align(ui_infoPanel, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_infoPanel, LV_OBJ_FLAG_HIDDEN);     /// Flags
    lv_obj_clear_flag(ui_infoPanel, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_infoPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_infoPanel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_infoPanel, 200, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_infoPanel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_infoPanel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_infoBox = lv_obj_create(ui_infoPanel);
    lv_obj_set_width(ui_infoBox, 500);
    lv_obj_set_height(ui_infoBox, 100);
    lv_obj_set_align(ui_infoBox, LV_ALIGN_CENTER);
    lv_obj_clear_flag(ui_infoBox, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_bg_color(ui_infoBox, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_infoBox, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_infoBox, 4, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_infoText = lv_label_create(ui_infoBox);
    lv_obj_set_width(ui_infoText, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_infoText, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_infoText, LV_ALIGN_CENTER);
    lv_label_set_text(ui_infoText, "");
    lv_obj_set_style_text_color(ui_infoText, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_infoText, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_infoText, &lv_font_montserrat_34, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui_infoText, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_infoText, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_infoText, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_infoText, 50, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_infoText, 50, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_infoText, 20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_infoText, 20, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_add_event_cb(ui_Inverter, ui_event_Inverter, LV_EVENT_ALL, NULL);

}
