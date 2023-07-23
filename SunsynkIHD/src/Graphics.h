#ifndef __GRAPHICS_H
#define __GRAPHICS_H

#include <Arduino_GFX_Library.h>
#include <lvgl.h>
#include "Config.h"

//Arduino_DataBus *bus;
extern Arduino_GFX *gfx;

#define UI_GREEN    0x00E05A    // Color used for generating, charging, exporting
#define UI_RED      0xE00000    // Color used for consuming, discharging, importing
#define UI_GREY     0xA2A2A2    // Color used for no activity
#define UI_WHITE    0x000000    // White

#ifdef ESP32_ILI9488_SPI_TFT

#define SCREEN_WIDTH    480
#define SCREEN_HEIGHT   320
#define GFX_BL          LCD_BL

#endif // ESP32_ILI9488_SPI_TFT

#ifdef ESP32S3_ELECROW_HMI_35
#endif // ESP32S3_ELECROW_HMI_35

#ifdef ESP32S3_ELECROW_HMI_70
#endif // ESP32S3_ELECROW_HMI_35

void UpdateDisplayFields(void);

boolean NightModeEnabled(void);

void SetNightMode(void);

void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p);

void printCenterString(const char* string, int y_pos);
void printCenterString(const char* string, const GFXfont* font, int color, int y_pos);
void printRightString(const char* string, const GFXfont* font, int color, int y_pos, int x_indent);

static lv_disp_draw_buf_t draw_buf;
static lv_color_t *disp_draw_buf;
static lv_disp_drv_t disp_drv;

static uint32_t screenWidth = SCREEN_WIDTH;
static uint32_t screenHeight = SCREEN_HEIGHT;

#endif // __GRAPHICS_H
