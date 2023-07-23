#ifndef __CONFIG_H
#define __CONFIG_H

/*
 * Config.h
 * Stores user-specific configuration and settings.
 * Changes to this file will be excluded from Git.
 */

/*
 * Screen type selection - uncomment one screen type
 */
#define ESP32_ILI9488_SPI_TFT       // 'DIY' hardware using ESP32 and 3.5" SPI TFT LCD with ILI9488 and XPT2046
//#define ESP32S3_ELECROW_HMI_35    // Elecrow 3.5" HMI with 16-bit parallel RGB interface
//#define ESP32S3_ELECROW_HMI_70    // Elecrow 7.0" HMI with 16-bit parallel RGB interface

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
#define SCREEN_OFF_TIMEOUT  30                                      // Number of seconds to keep LCD backlight on after it has been touched

/*
 * Touchpad calibration data
 */
#define TOUCH_CAL_X1        -1
#define TOUCH_CAL_X2        -1
#define TOUCH_CAL_Y1        -1
#define TOUCH_CAL_Y2        -1

#ifdef ESP32_ILI9488_SPI_TFT
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

#ifdef ESP32S3_ELECROW_HMI_35
#endif // ESP32S3_ELECROW_HMI_35

#ifdef ESP32S3_ELECROW_HMI_70
#endif // ESP32S3_ELECROW_HMI_35

#endif // __CONFIG_H
