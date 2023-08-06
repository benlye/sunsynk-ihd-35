#ifndef __ESP32S3_ELECROW_HMI_35_H
#define __ESP32S3_ELECROW_HMI_35_H

#ifdef ESP32S3_ELECROW_HMI_35

#define LGFX_USE_V1
#include "Config.h"
#include <LovyanGFX.hpp>
#include <lgfx/v1/panel/Panel_ILI948x.hpp>
#include <lgfx/v1/platforms/esp32s3/Bus_Parallel16.hpp>

#define SCREEN_WIDTH    480
#define SCREEN_HEIGHT   320
#define TFT_BL          46
#define BUZZER_PIN      20

#define TOUCH_SWAP_XY
#define TOUCH_MAP_X1 SCREEN_WIDTH
#define TOUCH_MAP_X2 0
#define TOUCH_MAP_Y1 0
#define TOUCH_MAP_Y2 SCREEN_HEIGHT

class LGFX : public lgfx::LGFX_Device
{
public:
  lgfx::Bus_Parallel16 _bus_instance;
  lgfx::Panel_ILI9488 _panel_instance;
  lgfx::Light_PWM     _light_instance;
  lgfx::Touch_FT5x06  _touch_instance;

  LGFX(void)
  {
    {
      auto cfg = _bus_instance.config();

      cfg.freq_write = 40000000;
      cfg.pin_wr = 18;
      cfg.pin_rd = 48;
      cfg.pin_rs = 45;

      cfg.pin_d0 = 47;
      cfg.pin_d1 = 21;
      cfg.pin_d2 = 14;
      cfg.pin_d3 = 13;
      cfg.pin_d4 = 12;
      cfg.pin_d5 = 11;
      cfg.pin_d6 = 10;
      cfg.pin_d7 = 9;
      cfg.pin_d8 = 3;
      cfg.pin_d9 = 8;
      cfg.pin_d10 = 16;
      cfg.pin_d11 = 15;
      cfg.pin_d12 = 7;
      cfg.pin_d13 = 6;
      cfg.pin_d14 = 5;
      cfg.pin_d15 = 4;
      _bus_instance.config(cfg);
      _panel_instance.bus(&_bus_instance);
    }

    {
      auto cfg = _panel_instance.config();
      cfg.panel_width     =   480;
      cfg.panel_width     =   320;
      cfg.offset_x        =     0;
      cfg.offset_y        =     0;
      cfg.pin_cs          =    -1;
      cfg.pin_rst         =    -1;
      cfg.pin_busy        =    -1;
      cfg.offset_rotation =     3;
      cfg.readable        =  true;
      cfg.invert          = false;
      cfg.rgb_order       = false;
      cfg.dlen_16bit      =  true;
      cfg.bus_shared      = false;

      _panel_instance.config(cfg);
    }

    {
      auto cfg = _light_instance.config();

      cfg.pin_bl = GPIO_NUM_46;

      _light_instance.config(cfg);
      _panel_instance.light(&_light_instance);
    }

    {
      auto cfg = _touch_instance.config();
      cfg.i2c_port = 0;
      cfg.i2c_addr = 0x38;
      cfg.pin_sda  = 38;
      cfg.pin_scl  = 39;
      cfg.pin_int  = -1;
      cfg.freq = 400000;
      
      cfg.x_min = 0;
      cfg.x_max = 479;
      cfg.y_min = 0;
      cfg.y_max = 319;

      _touch_instance.config(cfg);
      _panel_instance.setTouch(&_touch_instance);
    }

    setPanel(&_panel_instance);
  }

};

#endif // ESP32S3_ELECROW_HMI_35

#endif // __ESP32S3_ELECROW_HMI_35_H
