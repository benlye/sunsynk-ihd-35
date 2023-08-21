// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.3.1
// LVGL version: 8.3.6
// Project name: Solar_UI_480x320

#include "ui.h"
#include "ui_helpers.h"

///////////////////// VARIABLES ////////////////////

// SCREEN: ui_home
void ui_home_screen_init(void);
lv_obj_t * ui_home;
lv_obj_t * ui_pvIcon;
lv_obj_t * ui_pvLine;
lv_obj_t * ui_pvWatts;
lv_obj_t * ui_pvWattsUnit;
lv_obj_t * ui_pvTotal;
lv_obj_t * ui_pvTotalUnits;
lv_obj_t * ui_loadWatts;
lv_obj_t * ui_loadWattsUnit;
lv_obj_t * ui_battIcon;
lv_obj_t * ui_battLine;
lv_obj_t * ui_battSoc;
lv_obj_t * ui_battWatts;
lv_obj_t * ui_battWattsUnit;
lv_obj_t * ui_battDailyCharge;
lv_obj_t * ui_battDailyDischargeUnit;
lv_obj_t * ui_battDailyDischarge;
lv_obj_t * ui_battDailyChargeUnit;
lv_obj_t * ui_battDischargeIcon;
lv_obj_t * ui_battChargeIcon;
lv_obj_t * ui_gridIcon;
lv_obj_t * ui_gridLine;
lv_obj_t * ui_gridWatts;
lv_obj_t * ui_pvWattsUnit1;
lv_obj_t * ui_gridDailyExport;
lv_obj_t * ui_gridDailyExportUnit;
lv_obj_t * ui_gridDailyImport;
lv_obj_t * ui_gridDailyImportUnit;
lv_obj_t * ui_gridExportArrow;
lv_obj_t * ui_gridImportArrow;
lv_obj_t * ui_loadIcon;
lv_obj_t * ui_loadLine;
lv_obj_t * ui_loadTotal;
lv_obj_t * ui_loadTotalUnits;
void ui_event_Inverter(lv_event_t * e);
lv_obj_t * ui_Inverter;
lv_obj_t * ui_time;
lv_obj_t * ui_syncing;
lv_obj_t * ui_wifiHigh;
lv_obj_t * ui_wifiMed;
lv_obj_t * ui_wifiLow;
lv_obj_t * ui_infoPanel;
lv_obj_t * ui_infoBox;
lv_obj_t * ui_infoText;
lv_obj_t * ui_settingsIcon;

// SCREEN: ui_plot
void ui_plot_screen_init(void);
void ui_event_plot(lv_event_t * e);
lv_obj_t * ui_plot;
lv_obj_t * ui_dailyFlow;
lv_obj_t * ui____initial_actions0;
const lv_img_dsc_t * ui_imgset_867608463[1] = {&ui_img_705529016};
const lv_img_dsc_t * ui_imgset_1070557679[1] = {&ui_img_670870020};
const lv_img_dsc_t * ui_imgset_1885365100[1] = {&ui_img_1076282785};
const lv_img_dsc_t * ui_imgset_59742990[1] = {&ui_img_226794803};
const lv_img_dsc_t * ui_imgset_1375169470[1] = {&ui_img_1549501229};
const lv_img_dsc_t * ui_imgset_1399112042[1] = {&ui_img_264836261};
const lv_img_dsc_t * ui_imgset_535857063[1] = {&ui_img_1683162281};
const lv_img_dsc_t * ui_imgset_1178831554[1] = {&ui_img_1933564998};

///////////////////// TEST LVGL SETTINGS ////////////////////
#if LV_COLOR_DEPTH != 16
    #error "LV_COLOR_DEPTH should be 16bit to match SquareLine Studio's settings"
#endif
#if LV_COLOR_16_SWAP !=0
    #error "LV_COLOR_16_SWAP should be 0 to match SquareLine Studio's settings"
#endif

///////////////////// ANIMATIONS ////////////////////

///////////////////// FUNCTIONS ////////////////////
void ui_event_Inverter(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        _ui_screen_change(&ui_plot, LV_SCR_LOAD_ANIM_NONE, 500, 0, &ui_plot_screen_init);
    }
}
void ui_event_plot(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        _ui_screen_change(&ui_home, LV_SCR_LOAD_ANIM_NONE, 500, 0, &ui_home_screen_init);
    }
    if(event_code == LV_EVENT_SCREEN_LOAD_START) {
        updatePlotData(e);
    }
}

///////////////////// SCREENS ////////////////////

void ui_init(void)
{
    lv_disp_t * dispp = lv_disp_get_default();
    lv_theme_t * theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED),
                                               false, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
    ui_home_screen_init();
    ui_plot_screen_init();
    ui____initial_actions0 = lv_obj_create(NULL);
    lv_disp_load_scr(ui_home);
}
