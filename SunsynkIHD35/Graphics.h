#ifndef __GRAPHICS_H
#define __GRAPHICS_H

#include <Arduino_GFX_Library.h>
#include <lvgl.h>
#include "Config.h"

#ifdef ESP32_ILI9488_SPI_TFT
#define HAS_DISPLAY

#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 320

#define GFX_BL LCD_BL

Arduino_DataBus *bus = new Arduino_ESP32SPI(LCD_DC /* DC */, LCD_CS /* CS */, LCD_SCK /* SCK */, LCD_MOSI /* MOSI */, GFX_NOT_DEFINED /* MISO */, VSPI /* spi_num */);
Arduino_GFX *gfx = new Arduino_ILI9488_18bit(bus, LCD_RST /* RST */, LCD_ROTATION /* rotation */, false /* IPS */);

#endif // ESP32_ILI9488_SPI_TFT

#ifdef HAS_DISPLAY

void UpdateDisplayFields();

void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p);

void printCenterString(char* string, int y_pos);
void printCenterString(char* string, const GFXfont* font, int color, int y_pos);

static lv_disp_draw_buf_t draw_buf;
static lv_color_t *disp_draw_buf;
static lv_disp_drv_t disp_drv;

int16_t gfx_x1, gfx_y1;
uint16_t gfx_w, gfx_h;

static uint32_t screenWidth = SCREEN_WIDTH;
static uint32_t screenHeight = SCREEN_HEIGHT;

#endif // HAS_DISPLAY

#endif // __GRAPHICS_H
