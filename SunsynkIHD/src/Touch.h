#ifndef __TOUCH_H
#define __TOUCH_H

#include "Config.h"

#ifdef ESP32_ILI9488_SPI_TFT
#define HAS_TOUCH
#define TOUCH_XPT2046
#endif // ESP32_ILI9488_SPI_TFT

#ifdef ESP32S3_ELECROW_HMI_35
#define HAS_TOUCH
#define TOUCH_FT6X36
#endif // ESP32S3_ELECROW_HMI_35

#ifdef ESP32S3_ELECROW_HMI_70
#define HAS_TOUCH
#define TOUCH_GT911
#endif // ESP32S3_ELECROW_HMI_70

#ifdef TOUCH_XPT2046
#include <XPT2046_Touchscreen.h>
#include <SPI.h>
#define TOUCH_SWAP_XY
#define TOUCH_XPT2046_SCK TOUCH_SCK
#define TOUCH_XPT2046_MISO TOUCH_MISO
#define TOUCH_XPT2046_MOSI TOUCH_MOSI
#define TOUCH_XPT2046_CS TOUCH_CS
#define TOUCH_XPT2046_INT TOUCH_INT
#define TOUCH_XPT2046_ROTATION 0
#define TOUCH_XPT2046_SAMPLES 50
#endif // TOUCH_XPT2046

#ifdef TOUCH_GT911
#include <Wire.h>
#include <TAMC_GT911.h>
#define TOUCH_GT911_SCL 20 //20
#define TOUCH_GT911_SDA 19 //19
#define TOUCH_GT911_INT -1 //-1
#define TOUCH_GT911_RST -1 //38
#define TOUCH_GT911_ROTATION ROTATION_NORMAL
#define TOUCH_MAP_X1 800
#define TOUCH_MAP_X2 0
#define TOUCH_MAP_Y1 480
#define TOUCH_MAP_Y2 0
#endif // TOUCH_GT911

#ifdef TOUCH_FT6X36
#include <FT6X36.h>
#define TOUCH_SWAP_XY
#define TOUCH_FT6X36_SCL 19
#define TOUCH_FT6X36_SDA 38
#define TOUCH_FT6X36_INT 0
#define TOUCH_MAP_X1 480
#define TOUCH_MAP_X2 0
#define TOUCH_MAP_Y1 0
#define TOUCH_MAP_Y2 320
#endif // TOUCH_FT6X36

void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data);
void touch_init();
bool touch_has_signal();
bool touch_touched();
bool touch_released();

extern unsigned long lastTouchTime;

#endif // __TOUCH_H
