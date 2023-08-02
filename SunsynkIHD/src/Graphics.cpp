#include <Arduino.h>
#include <lvgl.h>
#include <WiFi.h>
#include "Graphics.h"
#include "SunsynkApi.h"
#include "Touch.h"

#if defined(SCREEN_800_480)
#include <ui_800_480.h>
#else
#include <ui_480_320.h>
#endif

#ifdef ESP32_ILI9488_SPI_TFT
#endif // ESP32_ILI9488_SPI_TFT

#ifdef ESP32S3_ELECROW_HMI_35
#endif // ESP32S3_ELECROW_HMI_35

#ifdef ESP32S3_ELECROW_HMI_70
#endif // ESP32S3_ELECROW_HMI_70

LGFX *gfx = new LGFX;

int16_t gfx_x1, gfx_y1;
uint16_t gfx_w, gfx_h;

int8_t rssi;
int8_t lastRssi = 0;

void UpdateDisplayFields()
{
    // Prevent Wi-Fi symbol hysteresis
    if (WiFi.RSSI() > lastRssi + 2 || WiFi.RSSI() < lastRssi - 2)
    {
        lastRssi = rssi;
        rssi = WiFi.RSSI();
    }

    // Show the appropriate WiFi symbol
    if (rssi < -80) // Poor signal
    {
        lv_obj_clear_flag(ui_wifiLow, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_wifiMed, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_wifiHigh, LV_OBJ_FLAG_HIDDEN);
    }
    else if (rssi < -67) // Moderate signal
    {
        lv_obj_add_flag(ui_wifiLow, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(ui_wifiMed, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_wifiHigh, LV_OBJ_FLAG_HIDDEN);
    }
    else // Good signal
    {
        lv_obj_add_flag(ui_wifiLow, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_wifiMed, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(ui_wifiHigh, LV_OBJ_FLAG_HIDDEN);
    }

    if (ihdDataReady &! ihdScreenRefreshed)
    {
        // Hide the syncing icon
        lv_obj_add_flag(ui_syncing, LV_OBJ_FLAG_HIDDEN);

        // Update the PV energy
        int pvWattsColor = UI_GREY;
        if (ihdData.pvWatts > 0)
        { // Generating
            pvWattsColor = UI_GREEN;
        }
        lv_obj_set_style_text_color(ui_pvWatts, lv_color_hex(pvWattsColor), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_label_set_text_fmt(ui_pvWatts, "%d", ihdData.pvWatts);

        // Update the grid energy
        int gridWattsColor = UI_GREY;
        if (ihdData.gridWatts > 0)
        {
            if (ihdData.toGrid)
            { // Exporting
                gridWattsColor = UI_GREEN;
            }
            else
            { // Importing
                gridWattsColor = UI_RED;
            }
        }
        lv_obj_set_style_text_color(ui_gridWatts, lv_color_hex(gridWattsColor), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_label_set_text_fmt(ui_gridWatts, "%d", ihdData.gridWatts);

        // Update the battery energy
        int battWattsColor = UI_GREY;
        if (ihdData.battWatts != 0)
        { // Neither charging nor discharging
            if (ihdData.toBatt)
            { // Charging
                battWattsColor = UI_GREEN;
            }
            else
            { // Discharging
                battWattsColor = UI_RED;
            }
        }
        lv_obj_set_style_text_color(ui_battWatts, lv_color_hex(battWattsColor), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_label_set_text_fmt(ui_battWatts, "%d", abs(ihdData.battWatts));

        // Update the load energy
        int loadWattsColor = UI_GREY;
        if (ihdData.loadWatts > 0)
        { // Consuming
            loadWattsColor = UI_RED;
        }
        lv_obj_set_style_text_color(ui_loadWatts, lv_color_hex(loadWattsColor), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_label_set_text_fmt(ui_loadWatts, "%d", ihdData.loadWatts);

        // Update the battery SOC
        lv_label_set_text_fmt(ui_battSoc, "%d%%", ihdData.battSoc);

        // Update total export
        double eTodayToDbl = ihdData.gridDailyExport;
        char eTodayToStr[8];
        dtostrf(eTodayToDbl, 3, 1, eTodayToStr);
        lv_label_set_text(ui_gridDailyExport, eTodayToStr);

        // Update total import
        double eTodayFromDbl = ihdData.gridDailyImport;
        char eTodayFromStr[8];
        dtostrf(eTodayFromDbl, 3, 1, eTodayFromStr);
        lv_label_set_text(ui_gridDailyImport, eTodayFromStr);

        // Update total battery charge
        double eTodayChgDbl = ihdData.battDailyCharge;
        char eTodayChgStr[8];
        dtostrf(eTodayChgDbl, 3, 1, eTodayChgStr);
        lv_label_set_text(ui_battDailyCharge, eTodayChgStr);

        // Update total battery discharge
        double eTodayDischgDbl = ihdData.battDailyDischarge;
        char eTodayDischgStr[8];
        dtostrf(eTodayDischgDbl, 3, 1, eTodayDischgStr);
        lv_label_set_text(ui_battDailyDischarge, eTodayDischgStr);

        // Update total load
        double loadTotalDbl = ihdData.loadDailyTotal;
        char eLoadTotalStr[8];
        dtostrf(loadTotalDbl, 3, 1, eLoadTotalStr);
        lv_label_set_text(ui_loadTotal, eLoadTotalStr);

        // Update PV total
        char eTodayStr[8];
        dtostrf(ihdData.pvDailyTotal, 3, 1, eTodayStr);
        lv_label_set_text(ui_pvTotal, eTodayStr);

        ihdScreenRefreshed = true;
    }
    else if (!ihdDataReady)
    {
        // Show the syncing icon
        lv_obj_clear_flag(ui_syncing, LV_OBJ_FLAG_HIDDEN);
    }

    // Update time
    lv_label_set_text(ui_time, ihdData.time.c_str());
}

/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
    int w = (area->x2 - area->x1 + 1);
    int h = (area->y2 - area->y1 + 1);
    gfx->startWrite();
    gfx->setAddrWindow(area->x1, area->y1, w, h);
    gfx->writePixels((lgfx::rgb565_t*)&color_p->full, w*h);
    gfx->endWrite();
    lv_disp_flush_ready(disp);
}
