#ifndef __CONFIG_H
#define __CONFIG_H

/*
 * Config.h
 * Stores user-specific configuration and settings.
 * Changes to this file will be excluded from Git.
 */

/*
 * Wireless network
 */
#define WIFI_SSID           "<Your wireless network SSID>"         // SSID of the wireless network to connect to
#define WIFI_PASSWORD       "<Your wireless network password>"     // Wireless network password

/*
 * Sunsynk API
 */
#define SUNSYNK_USERNAME    "<Your username for the Sunsynk app>"   // Username for Sunsynk API
#define SUNSYNK_PASSWORD    "<Your password for the Sunsynk app>"   // Password for Sunsynk API
#define SUNSYNK_PLANT_ID    "<The ID of your Sunsynk plant>"        // Numeric ID of the Sunsynk plant
#define API_READ_TIMEOUT    5000                                    // API read timeout in milliseconds (default is 5000)

/*
 * Timezone and NTP server settings
 */
#define TZ_INFO             "GMTGMT-1,M3.4.0/01,M10.4.0/02"         // Timezone string to use for local time. Default is for the UK.
#define NTP_SERVER_1        "0.pool.ntp.org"                        // First NTP server
#define NTP_SERVER_2        "1.pool.ntp.org"                        // Second NTP server
#define NTP_SERVER_3        "time.nist.gov"                         // Third NTP server

/*
 * Night mode times and timeout
 */
#define SCREEN_OFF_TIME     "23:00"                                 // Time when the LCD backlight will be turned off
#define SCREEN_ON_TIME      "06:00"                                 // Time when the LCD backlight will be turned back on
#define SCREEN_OFF_TIMEOUT  30                                      // Number of seconds to keep LCD backlight on after it has been touched (default is 30)

/*
 * Screen brightness settings for day and night modes
*/
#define LCD_BRIGHTNESS_DAY      255                                 // Day setting (0-255)
#define LCD_BRIGHTNESS_NIGHT    0                                   // Night setting (0-255). At 0 the API polling will be suspended while night mode is enabled.

/*
 * Colours for PV, grid, battery, and load Watts
 */
#define UI_GREEN    0x00E05A    // Color used for generating, charging, exporting
#define UI_RED      0xE00000    // Color used for consuming, discharging, importing
#define UI_GREY     0xA2A2A2    // Color used for no activity
#define UI_WHITE    0x000000    // White


/*
 * Settings for the DIY ESP32S3 DevKit with ILI9488 3.5" touchscreen
 * If you are have built your own DIY IHF then adapt to suit your own implementation.
 * If you are using an Elecrow display you can ignore the remainder of this file.
*/
#ifdef ESP32_ILI9488_SPI_TFT

/*
 * Touchpad calibration data
 */
#define TOUCH_MAP_X1         100
#define TOUCH_MAP_X2        4000
#define TOUCH_MAP_Y1         100
#define TOUCH_MAP_Y2        4000

// LCD Rotation
#define LCD_ROTATION        3

// LCD pins
#define LCD_DC              0
#define LCD_CS              15
#define LCD_SCK             18
#define LCD_MOSI            23
#define LCD_MISO            GFX_NOT_DEFINED
#define LCD_BL              32
#define LCD_RST             4

// Touch pins
#define TOUCH_SCK           18
#define TOUCH_MISO          19
#define TOUCH_MOSI          23
#define TOUCH_CS            21
#define TOUCH_INT           27
#endif // ESP32_ILI9488_SPI_TFT

#endif // __CONFIG_H
