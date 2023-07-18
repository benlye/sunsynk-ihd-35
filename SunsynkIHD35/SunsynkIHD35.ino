#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <lvgl.h>
#include <Time.h>
#include <TimeLib.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <WiFiMulti.h>

#include "Config.h"
#include "FreeSans8pt7b.h"
#include "FreeSansBold10pt7b.h"
#include "Graphics.h"
#include "SunsynkApi.h"
//#include "Touch.h"
#include "ui.h"

#ifndef __CONFIG_H
#error Configuration file is missing!
#endif

// Variables for looping
long lastUpdateTime = 0;
long lastClockUpdateTime = 0;
int loopDelaySec = 30;

// Global instance of Wi-Fi client
WiFiMulti WiFiMulti;

// Connect to WiFi
void connectWifI()
{
    WiFi.mode(WIFI_STA);
    WiFiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);

    Serial.print(" - Waiting for WiFi to connect ...");
    while ((WiFiMulti.run() != WL_CONNECTED))
    {
        // Serial.print(".");
    }
    Serial.println(" connected");
}

// Use NTP to set the clock
void setClock()
{
    configTzTime(TZ_INFO, "0.pool.ntp.org", "1.pool.ntp.org", "2.pool.ntp.org");

    Serial.print(F(" - Waiting for NTP time sync ..."));
    time_t nowSecs = time(nullptr);
    while (nowSecs < 8 * 3600 * 2)
    {
        delay(500);
        Serial.print(F("."));
        yield();
        nowSecs = time(nullptr);
    }
    Serial.println(" synced");
}

// Get current epoch time
unsigned long getTime()
{
    time_t now;
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo))
    {
        return (0);
    }
    time(&now);
    return now;
}

// Return a datetime string of the current localized time
String getDateTimeString()
{
    struct tm timeinfo;
    char timeString[32];
    if (!getLocalTime(&timeinfo))
    {
        Serial.println("Failed to obtain time");
        return "1970-01-01 00:00:00";
    }
    sprintf(timeString, "%04d-%02d-%02d %02d:%02d:%02d", timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
    return String(timeString);
}

// Converts an epoch time to a datetime string
String getDateTimeString(unsigned long val)
{
    char timeString[20];
    sprintf(timeString, "%4d-%02d-%02d %02d:%02d:%02d", year(val), month(val), day(val), hour(val), minute(val), second(val));
    return String(timeString);
}

// Gets a 24hr clock time string
String getTimeString()
{
    struct tm timeinfo;
    char timeString[20];
    if (!getLocalTime(&timeinfo))
    {
        Serial.println("Failed to obtain time");
        return "00:00";
    }
    sprintf(timeString, "%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);
    return String(timeString);
}

/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

#if (LV_COLOR_16_SWAP != 0)
  gfx->draw16bitBeRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
#else
  gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
#endif

  lv_disp_flush_ready(disp);
}

/*Read the touchpad*/
void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
    uint16_t touchX = 0, touchY = 0;
    bool touched = false; //tft.getTouch(&touchX, &touchY, 600);

    if (!touched)
    {
        data->state = LV_INDEV_STATE_REL;
    }
    else
    {
        data->state = LV_INDEV_STATE_PR;
        data->point.x = touchX;
        data->point.y = touchY;
    }
}

void getVersion(char const *date, char const *time, char *buff)
{
    int month, day, year, hour, min, sec;
    static const char month_names[] = "JanFebMarAprMayJunJulAugSepOctNovDec";
    sscanf(date, "%s %d %d", buff, &day, &year);
    month = (strstr(month_names, buff) - month_names) / 3 + 1;
    year = year % 100U;
    sscanf(time, "%d:%d:%d", &hour, &min, &sec);
    sprintf(buff, "v%d.%02d%02d-%02d%02d", year, month, day, hour, min);
}

void setup()
{
    char version[24];
    getVersion(__DATE__, __TIME__, version);

    // Initialize the serial port
    Serial.begin(115200);
    Serial.printf("\n%s\n", version);
    Serial.println("\nBooting ...");

    // Init Display
    if (!gfx->begin())
    {
        Serial.println("gfx->begin() failed!");
    }
    gfx->fillScreen(BLACK);

    #ifdef GFX_BL
    pinMode(GFX_BL, OUTPUT);
    digitalWrite(GFX_BL, HIGH);
    #endif

    // Init touch device
    //touch_init(gfx->width(), gfx->height(), gfx->getRotation());

    // Initialize LVGL
    lv_init();

    // Allocate the LVGL display buffer
    disp_draw_buf = (lv_color_t *)heap_caps_malloc(sizeof(lv_color_t) * screenWidth * 40, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    if (!disp_draw_buf)
    {
        Serial.println("LVGL disp_draw_buf allocate failed!");
    }
    else
    {
        lv_disp_draw_buf_init(&draw_buf, disp_draw_buf, NULL, screenWidth * 40);

        /* Initialize the display */
        lv_disp_drv_init(&disp_drv);
        disp_drv.hor_res = screenWidth;
        disp_drv.ver_res = screenHeight;
        disp_drv.flush_cb = my_disp_flush;
        disp_drv.draw_buf = &draw_buf;
        lv_disp_drv_register(&disp_drv);

        /* Initialize the (dummy) input device driver */
        static lv_indev_drv_t indev_drv;
        lv_indev_drv_init(&indev_drv);
        indev_drv.type = LV_INDEV_TYPE_POINTER;
        lv_indev_drv_register(&indev_drv);
    }

    // Print the heading
    gfx->setFont(&FreeSansBold10pt7b);
    gfx->setTextColor(CYAN, true);
    gfx->getTextBounds("3.5\" IHD for Sunsynk", 0, 0, &gfx_x1, &gfx_y1, &gfx_w, &gfx_h);
    gfx->setCursor((gfx->width() - gfx_w) / 2, 28);
    gfx->print("3.5\" IHD for Sunsynk");
    
    // Print the version
    gfx->setFont(&FreeSans8pt7b);
    gfx->setTextColor(LIGHTGREY);
    gfx->getTextBounds(version, 0, 0, &gfx_x1, &gfx_y1, &gfx_w, &gfx_h);
    gfx->setCursor((gfx->width() - gfx_w) / 2, 55);
    gfx->print(version);

    // Print the Git URL
    gfx->setTextColor(LIGHTGREY);
    gfx->getTextBounds("https://github.com/benlye/sunsynk-ihd-35", 0, 0, &gfx_x1, &gfx_y1, &gfx_w, &gfx_h);
    gfx->setCursor((gfx->width() - gfx_w) / 2, 80);
    gfx->print("https://github.com/benlye/sunsynk-ihd-35");
    
    // Connect to WiFi
    gfx->setFont(&FreeSans8pt7b);
    gfx->getTextBounds("Connecting to wireless network ...", 0, 0, &gfx_x1, &gfx_y1, &gfx_w, &gfx_h);
    gfx->setCursor((gfx->width() - gfx_w) / 2, (gfx->height() / 2));
    gfx->setTextColor(YELLOW);
    gfx->print("Connecting to wireless network ...");
    connectWifI();

    // Print the IP address
    gfx->getTextBounds(WiFi.localIP().toString().c_str(), 0, 0, &gfx_x1, &gfx_y1, &gfx_w, &gfx_h);
    gfx->setCursor((gfx->width() - gfx_w) - 20, gfx->height() - 10);
    gfx->setTextColor(GREEN);
    gfx->print(WiFi.localIP().toString().c_str());

    // Set the clock
    gfx->setFont(&FreeSans8pt7b);
    gfx->fillRect(0, (gfx->height() / 2) - 35, gfx->width(), 50, BLACK);
    gfx->getTextBounds("Synchronizing the clock ...", 0, 0, &gfx_x1, &gfx_y1, &gfx_w, &gfx_h);
    gfx->setCursor((gfx->width() - gfx_w) / 2, (gfx->height() / 2));
    gfx->setTextColor(YELLOW);
    gfx->print("Synchronizing the clock ...");
    setClock();

    // Print the time
    gfx->setCursor(20, gfx->height() - 10);
    gfx->setTextColor(GREEN);
    gfx->print(getDateTimeString().c_str());

    // Get an API access token
    gfx->setFont(&FreeSans8pt7b);
    gfx->fillRect(0, (gfx->height() / 2) - 35, gfx->width(), 50, BLACK);
    gfx->getTextBounds("Authenticating with Sunsynk ...", 0, 0, &gfx_x1, &gfx_y1, &gfx_w, &gfx_h);
    gfx->setCursor((gfx->width() - gfx_w) / 2, (gfx->height() / 2));
    gfx->setTextColor(YELLOW);
    gfx->print("Authenticating with Sunsynk ...");
    GetSunsynkAuthToken();

    // Last status update
    gfx->setFont(&FreeSans8pt7b);
    gfx->fillRect(0, (gfx->height() / 2) - 35, gfx->width(), 50, BLACK);
    gfx->getTextBounds("Getting data from Sunsynk ...", 0, 0, &gfx_x1, &gfx_y1, &gfx_w, &gfx_h);
    gfx->setCursor((gfx->width() - gfx_w) / 2, (gfx->height() / 2));
    gfx->setTextColor(YELLOW);
    gfx->print("Getting data from Sunsynk ...");
    
    // Send status update to serial
    Serial.printf("\nNetwork SSID: %s\n", WIFI_SSID);
    Serial.printf("IP Address:   %s\n", WiFi.localIP().toString().c_str());
    Serial.printf("\nUTC Time:     %s\n", getDateTimeString(getTime()).c_str());
    Serial.println("\nReady.\n");
    
    // Initialize the IHD UI
    ui_init();
}

void loop()
{
    // Update the clock at startup or after the interval has elapsed
    if ((lastClockUpdateTime == 0) || (millis() - lastClockUpdateTime >= 1000L))
    {
        String timeNow = getTimeString();
        lv_label_set_text(ui_time, timeNow.c_str());
        lastClockUpdateTime = millis();
    }

    // Poll the API at startup or after the interval has elapsed
    if ((lastUpdateTime == 0) || (millis() - lastUpdateTime >= (loopDelaySec * 1000L)))
    {
        // Get a new token if one is needed
        if (!CheckSunsynkAuthToken())
        {
            GetSunsynkAuthToken();
        }

        // Get the plant realtime data
        GetPlantRealtime();

        // Get the plant flow data
        GetPlantFlow();

        // Get the grid import and export totals
        GetGridTotals();

        // Get the battery charge and discharge totals
        GetBatteryTotals();

        // Get the load total
        GetLoadTotal();

        // Store the time
        lastUpdateTime = millis();

        // Wait for a bit
        Serial.printf("Waiting %d seconds before next API poll ...\n\n", loopDelaySec);
    }

    // Update the GUI
    lv_timer_handler();

    // Sleep for a bit
    delay(5);
}
