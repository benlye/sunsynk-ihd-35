#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <lvgl.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <WiFiMulti.h>

#include "Config.h"
#include "DateTime.h"
#include "FreeSans8pt7b.h"
#include "FreeSansBold10pt7b.h"
#include "Graphics.h"
#include "SunsynkApi.h"
#include "Touch.h"
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
    delay(500);
    gfx->fillScreen(BLACK);

    // Turn on the LCD backlight
#ifdef GFX_BL
    pinMode(GFX_BL, OUTPUT);
    digitalWrite(GFX_BL, HIGH);
#endif

    // Init touch device
    touch_init(gfx->width(), gfx->height(), gfx->getRotation());

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
        indev_drv.read_cb = my_touchpad_read;
        lv_indev_drv_register(&indev_drv);
    }

    // Print the heading
    printCenterString("3.5\" IHD for Sunsynk", &FreeSansBold10pt7b, CYAN, 28);
    
    // Print the version
    printCenterString(version, &FreeSans8pt7b, LIGHTGREY, 55);

    // Print the Git URL
    printCenterString("https://github.com/benlye/sunsynk-ihd-35", &FreeSans8pt7b, LIGHTGREY, 80);
    
    // Connect to WiFi
    printCenterString("Connecting to wireless network ...", &FreeSans8pt7b, YELLOW, (gfx->height() / 2));
    connectWifI();

    // Print the IP address
    gfx->getTextBounds(WiFi.localIP().toString().c_str(), 0, 0, &gfx_x1, &gfx_y1, &gfx_w, &gfx_h);
    gfx->setCursor((gfx->width() - gfx_w) - 20, gfx->height() - 10);
    gfx->setTextColor(GREEN);
    gfx->print(WiFi.localIP().toString().c_str());

    // Set the clock
    gfx->fillRect(0, (gfx->height() / 2) - 35, gfx->width(), 50, BLACK);
    printCenterString("Synchronizing the clock ...", &FreeSans8pt7b, YELLOW, (gfx->height() / 2));
    setClock();

    // Print the time
    gfx->setCursor(20, gfx->height() - 10);
    gfx->setTextColor(GREEN);
    gfx->print(getDateTimeString().c_str());

    // Get an API access token
    gfx->fillRect(0, (gfx->height() / 2) - 35, gfx->width(), 50, BLACK);
    printCenterString("Authenticating with Sunsynk ...", &FreeSans8pt7b, YELLOW, (gfx->height() / 2));
    GetSunsynkAuthToken();

    // Last status update
    gfx->fillRect(0, (gfx->height() / 2) - 35, gfx->width(), 50, BLACK);
    printCenterString("Getting data from Sunsynk ...", &FreeSans8pt7b, YELLOW, (gfx->height() / 2));
    
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
