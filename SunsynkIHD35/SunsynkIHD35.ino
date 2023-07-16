#include <lvgl.h>
#include <TFT_eSPI.h>
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <WiFiMulti.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <Time.h>
#include <TimeLib.h>

#include "Config.h"
#include "SunsynkApi.h"
#include "ui.h"

#ifndef __CONFIG_H
#error Configuration file is missing!
#endif

/* Screen resolution */
static const uint16_t screenWidth = 480;
static const uint16_t screenHeight = 320;

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[screenWidth * screenHeight / 10];

// Variables for looping
long lastUpdateTime = 0;
long lastClockUpdateTime = 0;
int loopDelaySec = 30;

const char compileDate[] = __DATE__ " " __TIME__;

// Global TFT instance
TFT_eSPI tft = TFT_eSPI(screenWidth, screenHeight);

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
    configTzTime(TZ_INFO, "pool.ntp.org");

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

    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, w, h);
    tft.pushColors((uint16_t *)&color_p->full, w * h, true);
    tft.endWrite();

    lv_disp_flush_ready(disp);
}

/*Read the touchpad*/
void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
    uint16_t touchX = 0, touchY = 0;
    bool touched = tft.getTouch(&touchX, &touchY, 600);

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

    // Initialize LVGL
    lv_init();

    // Initialize the screen and set the rotation
    tft.begin();
    tft.setRotation(TFT_ROTATION);

// Apply the screen calibration data
#ifdef USE_TOUCH_CAL
    tft.setTouch(calData);
#endif

    // Initialize the display buffer
    lv_disp_draw_buf_init(&draw_buf, buf, NULL, screenWidth * screenHeight / 10);

    // Initialize the display
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    // Initialize the (dummy) input device driver
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_touchpad_read;
    lv_indev_drv_register(&indev_drv);

    // Initialize the TFT
    ui_init();
    tft.fillScreen(TFT_BLACK);
    tft.setTextPadding(300);
    tft.setTextDatum(TC_DATUM);
    tft.setTextColor(TFT_CYAN, true);
    tft.drawString("3.5\" IHD for Sunsynk", tft.width() / 2, 10, 4);
    tft.setTextColor(TFT_LIGHTGREY);
    tft.drawString("https://github.com/benlye/sunsynk-ihd-35", tft.width() / 2, 40, 2);
    tft.setTextDatum(BC_DATUM);
    tft.drawString(version, tft.width() / 2, tft.height() - 10, 2);

    // Connect to WiFi
    tft.setTextDatum(MC_DATUM);
    tft.setTextColor(TFT_YELLOW, true);
    tft.drawString(" Connecting to wireless network ...", tft.width() / 2, tft.height() / 2, 2);
    connectWifI();
    tft.setTextDatum(BR_DATUM);
    tft.setTextColor(TFT_GREEN);
    tft.drawString(WiFi.localIP().toString().c_str(), tft.width() - 10, tft.height() - 10, 2);

    // Set the clock
    tft.setTextDatum(MC_DATUM);
    tft.setTextColor(TFT_YELLOW, true);
    tft.drawString("Synchronizing the clock ...", tft.width() / 2, tft.height() / 2, 2);
    setClock();
    tft.setTextDatum(BL_DATUM);
    tft.setTextColor(TFT_GREEN);
    tft.drawString(getDateTimeString().c_str(), 10, tft.height() - 10, 2);

    // Get an API access token
    tft.setTextDatum(MC_DATUM);
    tft.setTextColor(TFT_YELLOW, true);
    tft.drawString("Authenticating with Sunsynk ...", tft.width() / 2, tft.height() / 2, 2);
    GetSunsynkAuthToken();

    // Last status update
    tft.setTextDatum(MC_DATUM);
    tft.setTextColor(TFT_YELLOW, true);
    tft.drawString("Getting data from Sunsynk ...", tft.width() / 2, tft.height() / 2, 2);

    // Send status update to serial
    Serial.printf("\nNetwork SSID: %s\n", WIFI_SSID);
    Serial.printf("IP Address:   %s\n", WiFi.localIP().toString().c_str());
    Serial.printf("\nUTC Time:     %s\n", getDateTimeString(getTime()).c_str());
    Serial.println("\nReady.\n");
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
