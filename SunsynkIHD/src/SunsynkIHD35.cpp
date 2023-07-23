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

void TaskNtp(void *pvParameters);
void TaskClock(void *pvParameters);
void TaskSunsynkApi(void *pvParameters);
void TaskOutput(void *pvParameters);
void TaskStatus(void *pvParameters);

// Global instance of Wi-Fi client
WiFiMulti wiFiMulti;

IhdData ihdData;
boolean ihdDataReady = false;
unsigned long lastTouchTime = 0;

// Connect to WiFi
void connectWifI()
{
    WiFi.mode(WIFI_STA);
    wiFiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);

    Serial.print(" - Waiting for WiFi to connect ...");
    while ((wiFiMulti.run() != WL_CONNECTED))
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

void printStatus()
{
    unsigned long uptimeSecs = millis() / 1000;
    uint16_t hrs = uptimeSecs / 3600;
    uint8_t mins = (uptimeSecs / 60) % 60;
    uint8_t secs = uptimeSecs % 60;
    Serial.println();
    Serial.printf("Time:                %s\n", getDateTimeString(getTime()).c_str());
    Serial.printf("Uptime:              %d:%02d:%02d\n", hrs, mins, secs);
    Serial.printf("Wifi RSSI:           %d\n", WiFi.RSSI());
    Serial.printf("Free memory (bytes): %d\n", esp_get_free_heap_size());
    Serial.println();
}

void TaskNtp(void *pvParameters)
{
    uint32_t ntp_delay = *((uint32_t *)pvParameters);
    for (;;)
    {
        setClock();
        delay(ntp_delay);
    }
}

void TaskClock(void *pvParameters)
{
    uint32_t time_delay = *((uint32_t *)pvParameters);
    for (;;)
    {
        ihdData.time = getTimeString();
        delay(time_delay);
    }
}

void TaskSunsynkApi(void *pvParameters)
{
    uint32_t api_delay = *((uint32_t *)pvParameters);
    for (;;)
    {
        GetIhdData();
        delay(api_delay);
    }
}

void TaskOutput(void *pvParameters)
{
    uint32_t output_delay = *((uint32_t *)pvParameters);
    for (;;)
    {
#ifdef GFX_BL
        SetNightMode();
#endif
        UpdateDisplayFields();
        lv_timer_handler();
        delay(output_delay);
    }
}

void TaskStatus(void *pvParameters)
{
    uint32_t output_delay = *((uint32_t *)pvParameters);
    for (;;)
    {
        printStatus();
        delay(output_delay);
    }
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
    delay(1000);
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
    printRightString((char *)WiFi.localIP().toString().c_str(), &FreeSans8pt7b, GREEN, gfx->height() - 10, 20);

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

    // Get the initial data
    gfx->fillRect(0, (gfx->height() / 2) - 35, gfx->width(), 50, BLACK);
    printCenterString("Getting data from Sunsynk ...", &FreeSans8pt7b, YELLOW, (gfx->height() / 2));

    // Create the task to call the API to get the data
    uint32_t api_delay = 30000; // 30 seconds
    xTaskCreate(TaskSunsynkApi, "Task API", 20480, (void *)&api_delay, 2, NULL);

    // Send status update to serial
    Serial.printf("\nNetwork SSID: %s\n", WIFI_SSID);
    Serial.printf("IP Address:   %s\n", WiFi.localIP().toString().c_str());
    Serial.printf("\nUTC Time:     %s\n", getDateTimeString(getTime()).c_str());
    Serial.println("\nReady.\n");

    // Task to print the status
    uint32_t status_delay = 300 * 1000; // Five minutes
    xTaskCreate(TaskStatus, "Task Status", 5120, (void *)&status_delay, 2, NULL);

    // Create the task to update the clock value
    uint32_t ntp_delay = 86400 * 1000; // One day
    xTaskCreate(TaskNtp, "Task NTP", 2048, (void *)&ntp_delay, 2, NULL);

    // Create the task to update the time on the IHD
    uint32_t time_delay = 1000; // One second
    xTaskCreate(TaskClock, "Task Clock", 2048, (void *)&time_delay, 2, NULL);

    // Wait up to 10s for API data
    int delayEnd = millis() + (10 * 1000);
    while (!ihdDataReady && millis() < delayEnd)
    {
        delay(100);
    }

    // Initialize the LVGL UI
    ui_init();

    // Task to update the display
    uint32_t output_delay = 100; // One tenth of a second
    xTaskCreate(TaskOutput, "Task Output", 5120, (void *)&output_delay, 2, NULL);
}

void loop()
{
    // Nothing to do here as all the work is done by the tasks
}