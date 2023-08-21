// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.3.1
// LVGL version: 8.3.6
// Project name: Solar_UI_800x480

#ifndef _SOLAR_UI_800X480_UI_H
#define _SOLAR_UI_800X480_UI_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined __has_include
#if __has_include("lvgl.h")
#include "lvgl.h"
#elif __has_include("lvgl/lvgl.h")
#include "lvgl/lvgl.h"
#else
#include "lvgl.h"
#endif
#else
#include "lvgl.h"
#endif

#include "ui_helpers.h"
#include "ui_events.h"
// SCREEN: ui_home
void ui_home_screen_init(void);
extern lv_obj_t * ui_home;
extern lv_obj_t * ui_pvIcon;
extern lv_obj_t * ui_pvWatts;
extern lv_obj_t * ui_pvWattsUnit;
extern lv_obj_t * ui_pvTotal;
extern lv_obj_t * ui_pvTotalUnits;
extern lv_obj_t * ui_loadWatts;
extern lv_obj_t * ui_loadWattsUnit;
extern lv_obj_t * ui_battIcon;
extern lv_obj_t * ui_battSoc;
extern lv_obj_t * ui_battWatts;
extern lv_obj_t * ui_battWattsUnit;
extern lv_obj_t * ui_battDailyCharge;
extern lv_obj_t * ui_battDailyDischargeUnit;
extern lv_obj_t * ui_battDailyDischarge;
extern lv_obj_t * ui_battDailyChargeUnit;
extern lv_obj_t * ui_battDischargeIcon;
extern lv_obj_t * ui_battChargeIcon;
extern lv_obj_t * ui_gridIcon;
extern lv_obj_t * ui_gridWatts;
extern lv_obj_t * ui_gridWattsUnit;
extern lv_obj_t * ui_gridDailyExport;
extern lv_obj_t * ui_gridDailyExportUnit;
extern lv_obj_t * ui_gridDailyImport;
extern lv_obj_t * ui_gridDailyImportUnit;
extern lv_obj_t * ui_gridExportArrow;
extern lv_obj_t * ui_gridImportArrow;
extern lv_obj_t * ui_loadIcon;
extern lv_obj_t * ui_loadTotal;
extern lv_obj_t * ui_loadTotalUnits;
void ui_event_Inverter(lv_event_t * e);
extern lv_obj_t * ui_Inverter;
extern lv_obj_t * ui_time;
extern lv_obj_t * ui_syncing;
extern lv_obj_t * ui_wifiHigh;
extern lv_obj_t * ui_wifiMed;
extern lv_obj_t * ui_wifiLow;
extern lv_obj_t * ui_settingsIcon;
extern lv_obj_t * ui_Image3;
extern lv_obj_t * ui_Image1;
extern lv_obj_t * ui_Image4;
extern lv_obj_t * ui_Image2;
extern lv_obj_t * ui_infoPanel;
extern lv_obj_t * ui_infoBox;
extern lv_obj_t * ui_infoText;
// SCREEN: ui_plot
void ui_plot_screen_init(void);
void ui_event_plot(lv_event_t * e);
extern lv_obj_t * ui_plot;
extern lv_obj_t * ui_dailyFlow;
extern lv_obj_t * ui____initial_actions0;

LV_IMG_DECLARE(ui_img_1549501229);    // assets\icons8-solar-panel-100.png
LV_IMG_DECLARE(ui_img_670870020);    // assets\icons8-charging-battery-90.png
LV_IMG_DECLARE(ui_img_705529016);    // assets\icons8-arrow-30.png
LV_IMG_DECLARE(ui_img_264836261);    // assets\icons8-transmission-tower-100.png
LV_IMG_DECLARE(ui_img_1076282785);    // assets\icons8-house-100.png
LV_IMG_DECLARE(ui_img_1230882528);    // assets\sunsynk-logo-med.png
LV_IMG_DECLARE(ui_img_1765460539);    // assets\icons8-transfer-24 (1).png
LV_IMG_DECLARE(ui_img_1683162281);    // assets\icons8-wifi-26.png
LV_IMG_DECLARE(ui_img_1288183160);    // assets\icons8-wifi-26-med.png
LV_IMG_DECLARE(ui_img_811431924);    // assets\icons8-wifi-26-low.png
LV_IMG_DECLARE(ui_img_226794803);    // assets\icons8-settings-24-1.png
LV_IMG_DECLARE(ui_img_207095971);    // assets\line-150-5.png
LV_IMG_DECLARE(ui_img_1192762099);    // assets\line-150-5-inv.png

LV_FONT_DECLARE(ui_font_mont_64);

void ui_init(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
