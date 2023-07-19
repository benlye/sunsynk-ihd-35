#include <Arduino_GFX_Library.h>
#include <lvgl.h>
#include "Graphics.h"
#include "SunsynkApi.h"
#include "ui.h"

void UpdateDisplayFields()
{
    if(ihdDataReady) {
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
        dtostrf(ihdData.pvDailyTotal,3,1,eTodayStr);
        lv_label_set_text(ui_pvTotal, eTodayStr);
    }

    // Update time
    lv_label_set_text(ui_time, ihdData.time.c_str());
}

/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

#if (LV_COLOR_16_SWAP != 0)
    gfx->draw16bitBeRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
#else
    gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
#endif

    lv_disp_flush_ready(disp);
}

void printCenterString(char* string, int y_pos)
{
    gfx->getTextBounds(string, 0, 0, &gfx_x1, &gfx_y1, &gfx_w, &gfx_h);
    gfx->setCursor((gfx->width() - gfx_w) / 2, y_pos);
    gfx->print(string);
}

void printCenterString(char* string, const GFXfont* font, int color, int y_pos)
{
    gfx->setFont(font);
    gfx->setTextColor(color);
    gfx->getTextBounds(string, 0, 0, &gfx_x1, &gfx_y1, &gfx_w, &gfx_h);
    gfx->setCursor((gfx->width() - gfx_w) / 2, y_pos);
    gfx->print(string);
}
