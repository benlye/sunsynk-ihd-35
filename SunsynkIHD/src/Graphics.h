#ifndef __GRAPHICS_H
#define __GRAPHICS_H

#include <lvgl.h>
#include "Config.h"
#include <Displays.h>

#define UI_GREEN    0x00E05A    // Color used for generating, charging, exporting
#define UI_RED      0xE00000    // Color used for consuming, discharging, importing
#define UI_GREY     0xA2A2A2    // Color used for no activity
#define UI_WHITE    0x000000    // White

void UpdateDisplayFields(void);
boolean IsNightMode(void);
void SetNightMode(void);

extern boolean showInfoMessage;
extern const char* infoMessage;

void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p);

static lv_disp_draw_buf_t draw_buf;
static lv_color_t *disp_draw_buf;
static lv_disp_drv_t disp_drv;

static uint32_t screenWidth = SCREEN_WIDTH;
static uint32_t screenHeight = SCREEN_HEIGHT;

#endif // __GRAPHICS_H
