#ifndef __TOUCH_H
#define __TOUCH_H

#include "Config.h"

#ifdef ESP32_ILI9488_SPI_TFT
#define HAS_TOUCH
#define TOUCH_XPT2046
#endif // ESP32_ILI9488_SPI_TFT

#ifdef TOUCH_XPT2046

#include <XPT2046_Touchscreen.h>
#include <SPI.h>

#define TOUCH_XPT2046_SCK TOUCH_SCK
#define TOUCH_XPT2046_MISO TOUCH_MISO
#define TOUCH_XPT2046_MOSI TOUCH_MOSI
#define TOUCH_XPT2046_CS TOUCH_CS
#define TOUCH_XPT2046_INT TOUCH_INT
#define TOUCH_XPT2046_ROTATION 0
#define TOUCH_XPT2046_SAMPLES 50

#endif // TOUCH_XPT2046

void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data);
void touch_init(int16_t w, int16_t h, uint8_t r);
bool touch_has_signal();
bool touch_touched();
bool touch_released();
void translate_touch_raw();

extern unsigned long lastTouchTime;

#endif // __TOUCH_H
