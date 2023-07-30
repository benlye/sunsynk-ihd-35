#ifndef __GRAPHICS_H
#define __GRAPHICS_H

#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include <lvgl.h>
#include "Config.h"

#define UI_GREEN    0x00E05A    // Color used for generating, charging, exporting
#define UI_RED      0xE00000    // Color used for consuming, discharging, importing
#define UI_GREY     0xA2A2A2    // Color used for no activity
#define UI_WHITE    0x000000    // White

#ifdef ESP32_ILI9488_SPI_TFT

//extern Arduino_GFX *gfx;
#define SCREEN_WIDTH    480
#define SCREEN_HEIGHT   320
#define TFT_BL          LCD_BL

#include <lgfx/v1/panel/Panel_ILI948x.hpp>
#include <lgfx/v1/platforms/esp32/Bus_SPI.hpp>

class LGFX : public lgfx::LGFX_Device
{
public:
  lgfx::Bus_SPI       _bus_instance;
  lgfx::Panel_ILI9488 _panel_instance;
  lgfx::Light_PWM     _light_instance;
  lgfx::ITouch*  _touch_instance_ptr = nullptr;

  LGFX(void)
  {
    {
      auto cfg = _bus_instance.config();
      cfg.spi_host = VSPI_HOST;
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

    setPanel(&_panel_instance);
  }
};

#endif // ESP32_ILI9488_SPI_TFT

#ifdef ESP32S3_ELECROW_HMI_35
#include <lgfx/v1/panel/Panel_ILI948x.hpp>
#include <lgfx/v1/platforms/esp32s3/Bus_Parallel16.hpp>
#define SCREEN_WIDTH    480
#define SCREEN_HEIGHT   320
#define TFT_BL          46
#define BUZZER_PIN      20

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
      cfg.x_max = 480;
      cfg.y_min = 0;
      cfg.y_max = 320;

      _touch_instance.config(cfg);
      _panel_instance.setTouch(&_touch_instance);
    }

    setPanel(&_panel_instance);
  }

};

#endif // ESP32S3_ELECROW_HMI_35

#ifdef ESP32S3_ELECROW_HMI_70
#include <lgfx/v1/platforms/esp32s3/Panel_RGB.hpp>
#include <lgfx/v1/platforms/esp32s3/Bus_RGB.hpp>
#define SCREEN_WIDTH    800
#define SCREEN_HEIGHT   480
#define TFT_BL          2

class LGFX : public lgfx::LGFX_Device
{
public:
    lgfx::Bus_RGB _bus_instance;
    lgfx::Panel_RGB _panel_instance;
    lgfx::Light_PWM _light_instance;
    lgfx::Touch_GT911 _touch_instance;
    LGFX(void)
    {
        {
            auto cfg = _panel_instance.config();
            cfg.memory_width = 800;
            cfg.memory_height = 480;
            cfg.panel_width = 800;
            cfg.panel_height = 480;
            cfg.offset_x = 0;
            cfg.offset_y = 0;
            _panel_instance.config(cfg);
        }

        {
            auto cfg = _bus_instance.config();
            cfg.panel = &_panel_instance;

            cfg.pin_d0 = GPIO_NUM_15; // B0
            cfg.pin_d1 = GPIO_NUM_7;  // B1
            cfg.pin_d2 = GPIO_NUM_6;  // B2
            cfg.pin_d3 = GPIO_NUM_5;  // B3
            cfg.pin_d4 = GPIO_NUM_4;  // B4

            cfg.pin_d5 = GPIO_NUM_9;  // G0
            cfg.pin_d6 = GPIO_NUM_46; // G1
            cfg.pin_d7 = GPIO_NUM_3;  // G2
            cfg.pin_d8 = GPIO_NUM_8;  // G3
            cfg.pin_d9 = GPIO_NUM_16; // G4
            cfg.pin_d10 = GPIO_NUM_1; // G5

            cfg.pin_d11 = GPIO_NUM_14; // R0
            cfg.pin_d12 = GPIO_NUM_21; // R1
            cfg.pin_d13 = GPIO_NUM_47; // R2
            cfg.pin_d14 = GPIO_NUM_48; // R3
            cfg.pin_d15 = GPIO_NUM_45; // R4

            cfg.pin_henable = GPIO_NUM_41;
            cfg.pin_vsync = GPIO_NUM_40;
            cfg.pin_hsync = GPIO_NUM_39;
            cfg.pin_pclk = GPIO_NUM_0;
            cfg.freq_write = 16000000;

            cfg.hsync_polarity = 0;
            cfg.hsync_front_porch = 40;
            cfg.hsync_pulse_width = 48;
            cfg.hsync_back_porch = 40;

            cfg.vsync_polarity = 0;
            cfg.vsync_front_porch = 1;
            cfg.vsync_pulse_width = 31;
            cfg.vsync_back_porch = 13;

            cfg.pclk_active_neg = 1;
            cfg.de_idle_high = 0;
            cfg.pclk_idle_high = 0;

            _bus_instance.config(cfg);
        }
        _panel_instance.setBus(&_bus_instance);

        {
            auto cfg = _light_instance.config();
            cfg.pin_bl = GPIO_NUM_2;
            _light_instance.config(cfg);
        }
        _panel_instance.light(&_light_instance);

        /*
            {
              auto cfg = _touch_instance.config();
              cfg.x_min      = 0;
              cfg.x_max      = 800;
              cfg.y_min      = 0;
              cfg.y_max      = 480;
              cfg.pin_int    = GPIO_NUM_18;
              cfg.bus_shared = false;
              cfg.offset_rotation = 0;
              // I2C接続
              cfg.i2c_port   = I2C_NUM_1;
              cfg.pin_sda    = GPIO_NUM_19;
              cfg.pin_scl    = GPIO_NUM_20;
              cfg.freq       = 400000;
              cfg.i2c_addr   = 0x14;        // 0x5D , 0x14
              _touch_instance.config(cfg);
              _panel_instance.setTouch(&_touch_instance);
            }
        */
        setPanel(&_panel_instance);
    }
};

#endif // ESP32S3_ELECROW_HMI_70

extern LGFX *gfx;

void UpdateDisplayFields(void);

boolean IsNightMode(void);

void SetNightMode(void);

void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p);

static lv_disp_draw_buf_t draw_buf;
static lv_color_t *disp_draw_buf;
static lv_disp_drv_t disp_drv;

static uint32_t screenWidth = SCREEN_WIDTH;
static uint32_t screenHeight = SCREEN_HEIGHT;

#endif // __GRAPHICS_H
