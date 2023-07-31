#ifndef __ESP32_ILI9488_SPI_H
#define __ESP32_ILI9488_SPI_H

#ifdef ESP32_ILI9488_SPI_TFT

#define LGFX_USE_V1
#include "Config.h"
#include <LovyanGFX.hpp>
#include <lgfx/v1/panel/Panel_ILI948x.hpp>
#include <lgfx/v1/platforms/esp32/Bus_SPI.hpp>

#define SCREEN_WIDTH    480
#define SCREEN_HEIGHT   320
#define TFT_BL          LCD_BL

#define TOUCH_SWAP_XY

class LGFX : public lgfx::LGFX_Device
{
public:
  lgfx::Bus_SPI       _bus_instance;
  lgfx::Panel_ILI9488 _panel_instance;
  lgfx::Light_PWM     _light_instance;
  lgfx::Touch_XPT2046 _touch_instance;

  LGFX(void)
  {
    {
      auto cfg = _bus_instance.config();
      cfg.spi_host = SPI3_HOST;
      cfg.spi_mode = 0;
      cfg.freq_write = 27000000;
      cfg.freq_read  = 20000000;
      cfg.spi_3wire  = true;
      cfg.use_lock   = true;
      cfg.dma_channel = SPI_DMA_CH_AUTO;
      cfg.pin_sclk = LCD_SCK;
      cfg.pin_mosi = LCD_MOSI;
      cfg.pin_miso = LCD_MISO;
      cfg.pin_dc = LCD_DC;

      _bus_instance.config(cfg);
      _panel_instance.setBus(&_bus_instance);

    }

    {
      auto cfg = _panel_instance.config();

      cfg.pin_cs           = LCD_CS;
      cfg.pin_rst          = LCD_RST;
      cfg.pin_busy         =    -1;

      cfg.panel_width      =   320;
      cfg.panel_height     =   480;
      cfg.offset_x         =     0;
      cfg.offset_y         =     0;
      cfg.offset_rotation  = LCD_ROTATION;
      cfg.dummy_read_pixel =     8;
      cfg.dummy_read_bits  =     1;
      cfg.readable         =  true;
      cfg.invert           = false;
      cfg.rgb_order        = false;
      cfg.dlen_16bit       = false;
      cfg.bus_shared       = false;

      _panel_instance.config(cfg);
    }

    {
      auto cfg = _light_instance.config();

      cfg.pin_bl = TFT_BL;

      _light_instance.config(cfg);
      _panel_instance.light(&_light_instance);
    }

    {
      auto cfg = _touch_instance.config();

      cfg.x_min      = 300;
      cfg.x_max      = 4000;
      cfg.y_min      = 300;
      cfg.y_max      = 4000;
      cfg.pin_int    = TOUCH_INT;
      cfg.bus_shared = true;
      cfg.offset_rotation = 0;

      cfg.spi_host = SPI3_HOST;
      cfg.freq = 1000000;
      cfg.pin_sclk = TOUCH_SCK;
      cfg.pin_mosi = TOUCH_MOSI;
      cfg.pin_miso = TOUCH_MISO;
      cfg.pin_cs   =  TOUCH_CS;

      _touch_instance.config(cfg);
      _panel_instance.setTouch(&_touch_instance);
    }

    setPanel(&_panel_instance);
  }
};

#endif // ESP32_ILI9488_SPI_TFT

#endif // __ESP32_ILI9488_SPI_H
