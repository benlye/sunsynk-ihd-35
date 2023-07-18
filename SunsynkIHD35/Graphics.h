#ifndef __GRAPHICS_H
#define __GRAPHICS_H

#include <Arduino_GFX_Library.h>

#include "Config.h"

#ifdef ESP32_ILI9488_SPI_TFT
#define HAS_DISPLAY 1

Arduino_DataBus *bus = new Arduino_ESP32SPI(0 /* DC */, 15 /* CS */, 18 /* SCK */, 23 /* MOSI */, GFX_NOT_DEFINED /* MISO */, VSPI /* spi_num */);
Arduino_GFX *gfx = new Arduino_ILI9488_18bit(bus, 4 /* RST */, 3 /* rotation */, false /* IPS */);

#define GFX_BL 32

/* Screen resolution */
static uint32_t screenWidth = 480;
static uint32_t screenHeight = 320;


#endif // ESP32_ILI9488_SPI_TFT


#ifdef HAS_DISPLAY

static lv_disp_draw_buf_t draw_buf;
static lv_color_t *disp_draw_buf;
static lv_disp_drv_t disp_drv;

int16_t gfx_x1, gfx_y1;
uint16_t gfx_w, gfx_h;

#endif // HAS_DISPLAY

#endif // __GRAPHICS_H
