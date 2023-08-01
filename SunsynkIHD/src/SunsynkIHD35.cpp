#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <lvgl.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <WiFiMulti.h>

//#include <fonts.h>
#include <ui.h>
#include "Config.h"
#include "DateTime.h"
#include "Graphics.h"
#include "SunsynkApi.h"
#include "Touch.h"

#ifndef __CONFIG_H
#error Configuration file is missing!
#endif

void TaskNtp(void *pvParameters);
void TaskClock(void *pvParameters);
void TaskSunsynkApi(void *pvParameters);
void TaskOutput(void *pvParameters);
void TaskStatus(void *pvParameters);

TaskHandle_t TaskSunsynkApi_h;

// Global instance of Wi-Fi client
WiFiMulti wiFiMulti;

IhdData ihdData;
boolean ihdDataReady = false;
boolean ihdScreenRefreshed = false;
unsigned long lastTouchTime = 0;
boolean backlightOn = true;

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

// Check if night mode should be enabled.
boolean IsNightMode()
{
    String timeNow = getTimeString();
    uint16_t timeNowInt = (timeNow.substring(0, 2).toInt() * 60) + timeNow.substring(3, 5).toInt();
    uint16_t timeOffInt = (String(SCREEN_OFF_TIME).substring(0, 2).toInt() * 60) + String(SCREEN_OFF_TIME).substring(3, 5).toInt();
    uint16_t timeOnInt = (String(SCREEN_ON_TIME).substring(0, 2).toInt() * 60) + String(SCREEN_ON_TIME).substring(3, 5).toInt();

    if (lastTouchTime + SCREEN_OFF_TIMEOUT < getTime())
    {
        if (timeOffInt > timeOnInt)
        {
            if ((timeNowInt >= timeOffInt) || (timeNowInt < timeOnInt))
            {
                return true;
            }
        }
        else
        {
            if ((timeNowInt >= timeOffInt) && (timeNowInt < timeOnInt))
            {
                return true;
            }
        }
    }

    return false;
}

// Enable or disable night mode.
void SetNightMode()
{   
    if (IsNightMode())
    {
        // Get the backlight state, turn it off if it's on.
        if (backlightOn)
        {
            Serial.println("Turning LCD backlight off.");
            gfx->setBrightness(LCD_BRIGHTNESS_NIGHT);
            backlightOn = false;
        }

        if (LCD_BRIGHTNESS_NIGHT == 0)
        {
            // Get the API task state, disable it if it's enabled
            eTaskState state = eTaskGetState(TaskSunsynkApi_h);
            if (state != eSuspended)
            {
                Serial.println("Suspending API polling task.");
                vTaskSuspend(TaskSunsynkApi_h);
            }
        }
    } else {
        // Get the API task state, enable it if it's disabled
        eTaskState state = eTaskGetState(TaskSunsynkApi_h);
        if (state == eSuspended)
        {
            Serial.println("Resuming API polling task.");
            vTaskResume(TaskSunsynkApi_h);
        }

        // Get the backlight state, turn it on if it's off
        if (!backlightOn)
        {
            Serial.println("Turning LCD backlight on.");
            gfx->setBrightness(LCD_BRIGHTNESS_DAY);
            backlightOn = true;
        }
    }
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
#ifdef TFT_BL
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
#ifdef BUZZER_PIN
    // Turn off the buzzer
    pinMode(BUZZER_PIN, OUTPUT);
    ledcSetup(4, 5000, 8);
    ledcAttachPin(BUZZER_PIN, 4);
    ledcWrite(4, 0);
#endif

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
    gfx->fillScreen(TFT_BLACK);

    // Turn on the LCD backlight
    gfx->setBrightness(LCD_BRIGHTNESS_DAY);

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
    gfx->setTextColor(TFT_CYAN);
    gfx->drawCentreString("3.5\" IHD for Sunsynk", gfx->width() / 2, 15, &fonts::Font4);

    // Print the version
    gfx->setTextColor(TFT_LIGHTGREY);
    gfx->drawCentreString(version, gfx->width() / 2, 55, &fonts::Font0);

    // Print the Git URL
    gfx->drawCentreString("https://github.com/benlye/sunsynk-ihd-35", gfx->width() / 2, 80, &fonts::Font0);

    // Connect to WiFi
    gfx->setTextColor(TFT_YELLOW);
    gfx->drawCentreString("Connecting to wireless network ...", gfx->width() / 2, gfx->height() / 2, &fonts::Font0);
    connectWifI();

    // Print the IP address
    gfx->setTextColor(TFT_GREEN);
    gfx->setTextDatum(textdatum_t::bottom_right);
    gfx->drawString(WiFi.localIP().toString().c_str(), gfx->width() - 20, gfx->height() - 10, &fonts::Font0);

    // Set the clock
    gfx->fillRect(0, (gfx->height() / 2) - 35, gfx->width(), 50, TFT_BLACK);
    gfx->setTextColor(TFT_YELLOW);
    gfx->drawCentreString("Synchronizing the clock ...", gfx->width() / 2, gfx->height() / 2, &fonts::Font0);
    setClock();

    // Print the time
    gfx->setTextColor(TFT_GREEN);
    gfx->setTextDatum(textdatum_t::bottom_left);
    gfx->drawString(getDateTimeString().c_str(), 20, gfx->height() - 10, &fonts::Font0);

    // Get an API access token
    gfx->fillRect(0, (gfx->height() / 2) - 35, gfx->width(), 50, TFT_BLACK);
    gfx->setTextColor(TFT_YELLOW);
    gfx->drawCentreString("Authenticating with Sunsynk ...", gfx->width() / 2, gfx->height() / 2, &fonts::Font0);
    GetSunsynkAuthToken();

    // Get the initial data
    gfx->fillRect(0, (gfx->height() / 2) - 35, gfx->width(), 50, TFT_BLACK);
    gfx->setTextColor(TFT_YELLOW);
    gfx->drawCentreString("Getting data from Sunsynk ...", gfx->width() / 2, gfx->height() / 2, &fonts::Font0);

    // Create the task to call the API to get the data
    uint32_t api_delay = 30000; // 30 seconds
    xTaskCreate(TaskSunsynkApi, "Task API", 20480, (void *)&api_delay, 2, &TaskSunsynkApi_h);

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
    uint32_t output_delay = 100;
    xTaskCreate(TaskOutput, "Task Output", 5120, (void *)&output_delay, 2, NULL);
}

void loop()
{
    // Nothing to do here as all the work is done by the tasks
}
