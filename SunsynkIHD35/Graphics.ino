#include <Arduino_GFX_Library.h>
#include <lvgl.h>
#include "Graphics.h"

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
