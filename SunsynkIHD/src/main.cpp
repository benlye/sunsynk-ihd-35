#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <lvgl.h>
#include <WiFi.h>
#include <Time.h>
#include <TimeLib.h>
#include "esp_sntp.h"

#include <Sunsynk.h>

#include "Config.h"
#include "esp32_ili9488_spi.h"
#include "esp32s3_elecrow_hmi_35.h"
#include "esp32s3_elecrow_hmi_70.h"

#if defined(SCREEN_800_480)
#include <ui_800_480.h>
#else
#include <ui_480_320.h>
#endif

#ifndef __CONFIG_H
#error Configuration file is missing!
#endif

// Task to make API calls. Should run every 30s as the API will have new data at most every 60s (but could be 300s).
void TaskCallApi(void *pvParameters);

// Task to update the clock time. Should run every 0.5s.
void TaskClock(void *pvParameters);

// Task to update the LVGL UI widget values. Should run every 0.1s.
void TaskUpdateIhd(void *pvParameters);

// Handles for the tasks
TaskHandle_t TaskCallApi_h;     // Api call task handle
TaskHandle_t TaskUpdateIhd_h;   // Api IHD update task handle
TaskHandle_t TaskNightMode_h;   // Api night mode toggle task handle

// Instance of Lovyan panel device for display output
LGFX *gfx = new LGFX;

// Global instance for the Sunsynk API.
Sunsynk sunsynk;

// Struct for storing the realtime flow data.
PlantFlowData_t flowData;

// Struct for storing the daily totals.
PlantTotals_t dailyTotals;

// Flag to indicate that all API calls are complete and the data is ready to be transferred to the display.
boolean ihdDataReady = false;

// Flag to indicate that the most recent API data has been displayed.
boolean ihdScreenRefreshed = false;

// Stores the last clock time that the screen was touched so we can time it out in night mode.
unsigned long lastTouchTime = 0;

// Keep track of whether the backlight is on or off in night mode.
boolean backlightOn = true;

// Flag for whether or not the info text panel should be displayed on the IHD.
boolean showInfoMessage = false;

// Holds the message that should be displayed in the info text panel.
const char* infoMessage = "";

// Holds the current clock time to be displayed on the IHD.
String ihdTime = "--:--";

// Holds the previous Wi-Fi RSSI so we can smooth the Wi-Fi icon display.
int8_t lastRssi = 0;

// LVGL structs
static lv_disp_draw_buf_t draw_buf;
static lv_color_t *disp_draw_buf;
static lv_disp_drv_t disp_drv;

// Connects to the Wi-Fi network.
void connectWiFi(void);

// Configures NTP and syncs the time.
void configureNtp(void);

// Builds a version string from the date and time.
void getVersion(char *buff);

// Determines if night mode should be enabled or not. Returns true (night mode) or false (day mode).
boolean IsNightMode(void);

// Toggles night mode on or off. Night mode dims the screen and disables API polling.
void SetNightMode(void);

// Updates the widget values in the UI with the data pulled from the APIs.
void UpdateDisplayFields(PlantFlowData_t &flow, PlantTotals_t &totals, bool &ready, bool &refreshed);

// Returns the current (non-localized) epoch time
unsigned long getTime(void);

// Returns the current local time as a 24hr clock time string for the IHD UI.
String getTimeString(void);

// Returns the current local time as number of minutes since midnight.
int16_t getTimeMinutes(void);

// Convert a clock time to minutes since midnight.
int16_t timeToMinutes(String time);

// LVGL touchpad read callback.
void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data);

// LVGL display flushing callback.
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p);

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
    getVersion(version);

    // Initialize the serial port
    Serial.begin(115200);
    Serial.printf("\n%s\n", version);
    Serial.println("\nBooting ...");

    // Init Display
    if (!gfx->begin())
    {
        Serial.println("gfx->begin() failed!");
    }
    gfx->fillScreen(TFT_BLACK);

    // Turn on the LCD backlight
    gfx->setBrightness(LCD_BRIGHTNESS_DAY);

    // Initialize LVGL
    lv_init();

    // Allocate the LVGL display buffer
    disp_draw_buf = (lv_color_t *)heap_caps_malloc(sizeof(lv_color_t) * SCREEN_WIDTH * 40, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    if (!disp_draw_buf)
    {
        Serial.println("LVGL disp_draw_buf allocate failed!");
    }
    else
    {
        lv_disp_draw_buf_init(&draw_buf, disp_draw_buf, NULL, SCREEN_WIDTH * 40);

        /* Initialize the display */
        lv_disp_drv_init(&disp_drv);
        disp_drv.hor_res = SCREEN_WIDTH;
        disp_drv.ver_res = SCREEN_HEIGHT;
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

    // Initialize the LVGL UI
    ui_init();
    showInfoMessage = true;

    // Create the task to update the display
    uint32_t ihd_update_delay = 10;
    xTaskCreate(TaskUpdateIhd, "IHD Update Task", 5120, (void *)&ihd_update_delay, 2, &TaskUpdateIhd_h);  

    // Connect to Wi-Fi
    infoMessage = "Connecting to Wi-Fi ...";
    connectWiFi();

    // Connect to WiFi
    infoMessage = "Setting the time ...";
    configureNtp();

    // Authenticate
    infoMessage = "Authenticating ...";
    sunsynk.Authenticate(SUNSYNK_USERNAME, SUNSYNK_PASSWORD);

    // Create the task to get API data
    infoMessage = "Fetching data ...";
    uint32_t api_call_delay = 30000; // 30 seconds
    xTaskCreate(TaskCallApi, "API Call Task", 20480, (void *)&api_call_delay, 2, &TaskCallApi_h);

    // Create the task to update the time on the IHD display
    uint32_t time_delay = 500; // One second
    xTaskCreate(TaskClock, "Task Clock", 2048, (void *)&time_delay, 2, NULL);

    // Wait up to 10s for API data
    int delayEnd = millis() + (10 * 1000);
    while (!ihdDataReady && millis() < delayEnd)
    {
        delay(100);
    }

    infoMessage = "Ready!";
    delay(500);

    // Hide the info panel
    infoMessage = "";
    showInfoMessage = false;
}

void loop()
{
    // Nothing to do here as all the work is done by the tasks
}

// Task to make API calls. Should run every 30s as the API will have new data at most every 60s (but could be 300s).
void TaskCallApi(void *pvParameters)
{
    uint32_t api_delay = *((uint32_t *)pvParameters);
    for (;;)
    {
        // Signal that data isn't ready
        ihdDataReady = false;

        // Check if the access token is still valid, renew it if now
        if (!sunsynk.CheckAccessToken())
        {
            sunsynk.Authenticate(SUNSYNK_USERNAME, SUNSYNK_PASSWORD);
        }

        // Get the plant flow data
        sunsynk.GetPlantFlow(SUNSYNK_PLANT_ID, flowData);

        // Get the date/time so we can pull today's totals
        struct tm timeinfo;
        if (getLocalTime(&timeinfo))
        {
            // Get the daily totals
            sunsynk.GetDailyTotals(SUNSYNK_PLANT_ID, timeinfo, dailyTotals);
        }

        // Signal that data is ready
        ihdDataReady = true;

        // Signal tha the IHD has not been refreshed with this data
        ihdScreenRefreshed = false;

        // Wait until this task should run again
        delay(api_delay);
    }
}

// Task to update the clock time. Should run every 0.5s.
void TaskClock(void *pvParameters)
{
    uint32_t time_delay = *((uint32_t *)pvParameters);
    for (;;)
    {
        // Store the current local time
        ihdTime = getTimeString();

        // Check if we need to toggle night mode
        SetNightMode();

        // Wait until this task should run again
        delay(time_delay);
    }
}

// Task to update the LVGL UI widget values. Should run every 0.1s.
void TaskUpdateIhd(void *pvParameters)
{
    uint32_t output_delay = *((uint32_t *)pvParameters);
    for (;;)
    {
        // Call the function to update the IHD UI widgets
        UpdateDisplayFields(flowData, dailyTotals, ihdDataReady, ihdScreenRefreshed);

        // Call the LVGL timer handler
        lv_timer_handler();

        // Wait until this task should run again
        delay(output_delay);
    }
}

// Connect to WiFi
void connectWiFi()
{
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print(" - Waiting for WiFi to connect ...");
    while ((WiFi.status() != WL_CONNECTED))
    {}
    Serial.println(" connected");
}

// Use NTP to set the clock
void configureNtp()
{
    sntp_set_sync_interval(86400 * 1000U);
    configTzTime(TZ_INFO, NTP_SERVER_1, NTP_SERVER_2, NTP_SERVER_3);

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

// Build a version string
void getVersion(char *buff)
{
    int month, day, year, hour, min, sec;
    static const char month_names[] = "JanFebMarAprMayJunJulAugSepOctNovDec";
    sscanf(__DATE__, "%s %d %d", buff, &day, &year);
    month = (strstr(month_names, buff) - month_names) / 3 + 1;
    year = year % 100U;
    sscanf(__TIME__, "%d:%d:%d", &hour, &min, &sec);
    sprintf(buff, "v%d.%02d%02d-%02d%02d", year, month, day, hour, min);
}

// Check if night mode should be enabled or disabled.
boolean IsNightMode()
{
    int16_t timeNowInt = getTimeMinutes();

    if (timeNowInt == -1)
    {
        return false;
    }

    uint16_t timeOffInt = timeToMinutes(SCREEN_OFF_TIME);
    uint16_t timeOnInt = timeToMinutes(SCREEN_ON_TIME);

    if (timeOffInt == -1 || timeOnInt == -1)
    {
        return false;
    }

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

        if (LCD_BRIGHTNESS_NIGHT == 0 && TaskCallApi_h != NULL)
        {
            // Get the API task state, disable it if it's enabled
            eTaskState state = eTaskGetState(TaskCallApi_h);
            if (state != eSuspended)
            {
                Serial.println("Suspending API polling task.");
                vTaskSuspend(TaskCallApi_h);
            }
        }
    } else {
        // Get the API task state, enable it if it's disabled
        if (TaskCallApi_h != NULL)
        {
            eTaskState state = eTaskGetState(TaskCallApi_h);
            if (state == eSuspended)
            {
                Serial.println("Resuming API polling task.");
                vTaskResume(TaskCallApi_h);
            }
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

// Updates the widget values in the UI with the data pulled from the APIs.
void UpdateDisplayFields(PlantFlowData_t &flowData, PlantTotals_t &dailyTotals, bool &ready, bool &refreshed)
{
    if (WiFi.status() == WL_CONNECTED)
    {
        // Prevent Wi-Fi symbol hysteresis
        int8_t rssi = WiFi.RSSI();
        int8_t rssiTemp = rssi;
        if (rssi < lastRssi + 2 && rssi > lastRssi - 2)
        {
            rssi = lastRssi;
        }
        lastRssi = rssiTemp;

        // Show the appropriate WiFi symbol
        if (rssi < -80) // Poor signal
        {
            lv_obj_clear_flag(ui_wifiLow, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(ui_wifiMed, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(ui_wifiHigh, LV_OBJ_FLAG_HIDDEN);
        }
        else if (rssi < -67) // Moderate signal
        {
            lv_obj_add_flag(ui_wifiLow, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(ui_wifiMed, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(ui_wifiHigh, LV_OBJ_FLAG_HIDDEN);
        }
        else // Good signal
        {
            lv_obj_add_flag(ui_wifiLow, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(ui_wifiMed, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(ui_wifiHigh, LV_OBJ_FLAG_HIDDEN);
        }
    }
    else
    {
        lv_obj_add_flag(ui_wifiLow, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_wifiMed, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_wifiHigh, LV_OBJ_FLAG_HIDDEN);
    }

    if (showInfoMessage)
    {
        lv_label_set_text(ui_infoText, infoMessage);
        lv_obj_clear_flag(ui_infoPanel, LV_OBJ_FLAG_HIDDEN);
    } else
    {
        lv_obj_add_flag(ui_infoPanel, LV_OBJ_FLAG_HIDDEN);
    }

    if (ready &! refreshed)
    {
        // Hide the syncing icon
        lv_obj_add_flag(ui_syncing, LV_OBJ_FLAG_HIDDEN);

        // Update the PV energy
        int pvWattsColor = UI_GREY;
        if (flowData.pvWatts > 0)
        { // Generating
            pvWattsColor = UI_GREEN;
        }
        lv_obj_set_style_text_color(ui_pvWatts, lv_color_hex(pvWattsColor), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_label_set_text_fmt(ui_pvWatts, "%d", flowData.pvWatts);

        // Update the grid energy
        int gridWattsColor = UI_GREY;
        if (flowData.gridWatts > 0)
        {
            if (flowData.toGrid)
            { // Exporting
                gridWattsColor = UI_GREEN;
            }
            else
            { // Importing
                gridWattsColor = UI_RED;
            }
        }
        lv_obj_set_style_text_color(ui_gridWatts, lv_color_hex(gridWattsColor), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_label_set_text_fmt(ui_gridWatts, "%d", flowData.gridWatts);

        // Update the battery energy
        int battWattsColor = UI_GREY;
        if (flowData.battWatts != 0)
        { // Neither charging nor discharging
            if (flowData.toBatt)
            { // Charging
                battWattsColor = UI_GREEN;
            }
            else
            { // Discharging
                battWattsColor = UI_RED;
            }
        }
        lv_obj_set_style_text_color(ui_battWatts, lv_color_hex(battWattsColor), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_label_set_text_fmt(ui_battWatts, "%d", abs(flowData.battWatts));

        // Update the load energy
        int loadWattsColor = UI_GREY;
        if (flowData.loadWatts > 0)
        { // Consuming
            loadWattsColor = UI_RED;
        }
        lv_obj_set_style_text_color(ui_loadWatts, lv_color_hex(loadWattsColor), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_label_set_text_fmt(ui_loadWatts, "%d", flowData.loadWatts);

        // Update the battery SOC
        lv_label_set_text_fmt(ui_battSoc, "%d%%", flowData.battSoc);

        // Update total export
        double eTodayToDbl = dailyTotals.gridExport;
        char eTodayToStr[8];
        dtostrf(eTodayToDbl, 3, 1, eTodayToStr);
        lv_label_set_text(ui_gridDailyExport, eTodayToStr);

        // Update total import
        double eTodayFromDbl = dailyTotals.gridImport;
        char eTodayFromStr[8];
        dtostrf(eTodayFromDbl, 3, 1, eTodayFromStr);
        lv_label_set_text(ui_gridDailyImport, eTodayFromStr);

        // Update total battery charge
        double eTodayChgDbl = dailyTotals.battCharge;
        char eTodayChgStr[8];
        dtostrf(eTodayChgDbl, 3, 1, eTodayChgStr);
        lv_label_set_text(ui_battDailyCharge, eTodayChgStr);

        // Update total battery discharge
        double eTodayDischgDbl = dailyTotals.battDischarge;
        char eTodayDischgStr[8];
        dtostrf(eTodayDischgDbl, 3, 1, eTodayDischgStr);
        lv_label_set_text(ui_battDailyDischarge, eTodayDischgStr);

        // Update total load
        double loadTotalDbl = dailyTotals.load;
        char eLoadTotalStr[8];
        dtostrf(loadTotalDbl, 3, 1, eLoadTotalStr);
        lv_label_set_text(ui_loadTotal, eLoadTotalStr);

        // Update PV total
        char eTodayStr[8];
        dtostrf(dailyTotals.pv, 3, 1, eTodayStr);
        lv_label_set_text(ui_pvTotal, eTodayStr);

        refreshed = true;
    }
    else if (!ready && refreshed)
    {
        // Show the syncing icon
        lv_obj_clear_flag(ui_syncing, LV_OBJ_FLAG_HIDDEN);
    }
    else
    {
        // Hide the syncing icon
        lv_obj_add_flag(ui_syncing, LV_OBJ_FLAG_HIDDEN);
    }

    // Update time
    lv_label_set_text(ui_time, ihdTime.c_str());
}

// Returns the current (non-localized) epoch time
unsigned long getTime()
{
    time_t now;
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo))
    {
        return 0;
    }
    time(&now);
    return now;
}

// Returns the current local time as a 24hr clock time string for the IHD UI
String getTimeString()
{
    struct tm timeinfo;
    char timeString[20];
    if (!getLocalTime(&timeinfo))
    {
        return "--:--";
    }
    sprintf(timeString, "%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);
    return String(timeString);
}

// Returns the current local time as number of minutes since midnight
int16_t getTimeMinutes()
{
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo))
    {
        return -1;
    }

    uint16_t m = (timeinfo.tm_hour * 60) + timeinfo.tm_min;
    return m;
}

// Converts a time string in 24hr format (e.g. 00:00 to 23:59) to the number of minutes since midnight
int16_t timeToMinutes(String s)
{
    if (s.length() != 5 || s[0] > '9' || s[0] < '0' || s[1] > '9' || s[1] < '0' || s[2] != ':' || s[3] > '9' || s[3] < '0' || s[4] > '9' || s[4] < '0')
        return -1;

    uint8_t h_int = s.substring(0, 2).toInt();
    uint8_t m_int = s.substring(3, 5).toInt();

    if (h_int > 23 || m_int > 59)
        return -1;

    uint16_t t = (h_int * 60) + m_int;
    return t;
}

// LVGL display flushing callback
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
    int w = (area->x2 - area->x1 + 1);
    int h = (area->y2 - area->y1 + 1);
    gfx->startWrite();
    gfx->setAddrWindow(area->x1, area->y1, w, h);
    gfx->writePixels((lgfx::rgb565_t*)&color_p->full, w*h);
    gfx->endWrite();
    lv_disp_flush_ready(disp);
}

// LVGL touch callback
void my_touchpad_read(lv_indev_drv_t * indev_driver, lv_indev_data_t * data)
{
    uint16_t touchX, touchY;
    int16_t touch_last_x = 0, touch_last_y = 0;

    data->state = LV_INDEV_STATE_REL;
    if(gfx->getTouchRaw(&touchX, &touchY))
    {
        
#if defined(TOUCH_SWAP_XY)
        touch_last_x = map(touchY, TOUCH_MAP_X1, TOUCH_MAP_X2, 0, SCREEN_WIDTH - 1);
        touch_last_y = map(touchX, TOUCH_MAP_Y1, TOUCH_MAP_Y2, 0, SCREEN_HEIGHT - 1);
#else
        touch_last_x = map(touchX, TOUCH_MAP_X1, TOUCH_MAP_X2, 0, SCREEN_WIDTH - 1);
        touch_last_y = map(touchY, TOUCH_MAP_Y1, TOUCH_MAP_Y2, 0, SCREEN_HEIGHT - 1);
#endif
        data->state = LV_INDEV_STATE_PR;
        data->point.x = touch_last_x;
        data->point.y = touch_last_y;

        //Serial.printf("Raw:   X:%d Y:%d\n", touchX, touchY);
        Serial.printf("Touch: X:%d Y:%d\n", data->point.x, data->point.y);
        lastTouchTime = getTime();
    }
}
