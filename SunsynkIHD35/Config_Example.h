#ifndef __CONFIG_H
#define __CONFIG_H

/*
 * Config.h - Stores sensitive configuration data.
 * Changes to this file will be excluded from Git.
 */

// SSID of the wireless network to connect to
const char* WIFI_SSID           = "<NETWORK SSID>";

// Wireless network password
const char* WIFI_PASSWORD       = "<NETWORK PASSWORD>";

// Username for Sunsynk
const char* SUNSYNK_USERNAME    = "<SUNSYNK USERNAME>";

// Password for Sunsynk
const char* SUNSYNK_PASSWORD    = "<SUNSYNK PASSWORD>";

// Numeric ID of the Sunsynk plant
const char* SUNSYNK_PLANT_ID    = "<PLANT ID>";

// Numeric ID of the Sunsynk inverter
const char* SUNSYNK_INVERTER_ID = "<INVERTER ID>";

// Timezone string - see https://www.gnu.org/software/libc/manual/html_node/TZ-Variable.html for more information
const char* TZ_INFO             = "GMTGMT-1,M3.4.0/01,M10.4.0/02";

// Touchpad calibration data
//#define USE_TOUCH_CAL
uint16_t calData[5]             = {0, 0, 0, 0, 0};

// TFT rotation setting
const int TFT_ROTATION          = 3;

#endif // __CONFIG_H
