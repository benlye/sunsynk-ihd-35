#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <lvgl.h>
#include <WiFi.h>
#include <Time.h>
#include <TimeLib.h>
#include <WiFiClientSecure.h>
#include <WiFiMulti.h>
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

// Task to make API calls.
void TaskCallApi(void *pvParameters);

// Task to update the clock time.
void TaskClock(void *pvParameters);

// Task to update the LVGL UI widget values.
void TaskUpdateIhd(void *pvParameters);

TaskHandle_t TaskCallApi_h;
TaskHandle_t TaskUpdateIhd_h;
TaskHandle_t TaskNightMode_h;

// Global instance of Wi-Fi client
WiFiMulti wiFiMulti;

Sunsynk sunsynk;
PlantFlowData_t flowData;
PlantTotals_t dailyTotals;

boolean ihdDataReady = false;
boolean ihdScreenRefreshed = false;

unsigned long lastTouchTime = 0;
boolean backlightOn = true;
boolean showInfoMessage = false;
const char* infoMessage = "";
String ihdTime = "--:--";

void configureNtpAndSetClock(void);
void timeSyncCallback(struct timeval *tv);
unsigned long getTime(void);
String getDateTimeString(void);
String getDateTimeString(unsigned long val);
String getTimeString(void);
String getDateString(void);
String getMonthString(void);
uint16_t timeToMinutes(String time);

void connectWiFi(void);
void getVersion(char const *date, char const *time, char *buff);


#define UI_GREEN    0x00E05A    // Color used for generating, charging, exporting
#define UI_RED      0xE00000    // Color used for consuming, discharging, importing
#define UI_GREY     0xA2A2A2    // Color used for no activity
#define UI_WHITE    0x000000    // White

void UpdateDisplayFields(PlantFlowData_t &flow, PlantTotals_t &totals, bool &ready, bool &refreshed);
boolean IsNightMode(void);
void SetNightMode(void);

extern boolean showInfoMessage;
extern const char* infoMessage;

void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p);

static lv_disp_draw_buf_t draw_buf;
static lv_color_t *disp_draw_buf;
static lv_disp_drv_t disp_drv;

static uint32_t screenWidth = SCREEN_WIDTH;
static uint32_t screenHeight = SCREEN_HEIGHT;

void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data);

extern unsigned long lastTouchTime;


LGFX *gfx = new LGFX;

int16_t gfx_x1, gfx_y1;
uint16_t gfx_w, gfx_h;

int8_t rssi;
int8_t lastRssi = 0;


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
    configureNtpAndSetClock();

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


// Connect to WiFi
void connectWiFi()
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

// Check if night mode should be enabled.
boolean IsNightMode()
{
    String timeNow = getTimeString();
    if (timeNow == "--:--")
    {
        return false;
    }

    uint16_t timeNowInt = timeToMinutes(timeNow);
    uint16_t timeOffInt = timeToMinutes(SCREEN_OFF_TIME);
    uint16_t timeOnInt = timeToMinutes(SCREEN_ON_TIME);

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

void TaskCallApi(void *pvParameters)
{
    uint32_t api_delay = *((uint32_t *)pvParameters);
    for (;;)
    {
        ihdDataReady = false;
        if (!sunsynk.CheckAccessToken())
        {
            sunsynk.Authenticate(SUNSYNK_USERNAME, SUNSYNK_PASSWORD);
        }
        sunsynk.GetPlantFlow(SUNSYNK_PLANT_ID2, flowData);
        
        struct tm timeinfo;
        if (getLocalTime(&timeinfo))
        {
            char buff[6];
            sunsynk.GetDailyTotals(SUNSYNK_PLANT_ID2, timeinfo, dailyTotals);
        }
        ihdDataReady = true;
        ihdScreenRefreshed = false;
        delay(api_delay);
    }
}

void TaskClock(void *pvParameters)
{
    uint32_t time_delay = *((uint32_t *)pvParameters);
    for (;;)
    {
        ihdTime = getTimeString();
        SetNightMode();
        delay(time_delay);
    }
}

void TaskUpdateIhd(void *pvParameters)
{
    uint32_t output_delay = *((uint32_t *)pvParameters);
    for (;;)
    {
        UpdateDisplayFields(flowData, dailyTotals, ihdDataReady, ihdScreenRefreshed);
        lv_timer_handler();
        delay(output_delay);
    }
}


void timeSyncCallback(struct timeval *tv)
{
    Serial.println("\nTime Sync:");
    Serial.println(tv->tv_sec);
    Serial.println(ctime(&tv->tv_sec));
}

// Use NTP to set the clock
void configureNtpAndSetClock()
{
    sntp_set_sync_interval(86400 * 1000U);
    sntp_set_time_sync_notification_cb(timeSyncCallback);
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
        return "--:--";
    }
    sprintf(timeString, "%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);
    return String(timeString);
}

// Gets a date string in the format YYYY-MM-DD
String getDateString()
{
    struct tm timeinfo;
    char dateString[20];
    if (!getLocalTime(&timeinfo))
    {
        return "1970-01-01";
    }
    sprintf(dateString, "%d-%02d-%02d", timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday);
    return String(dateString);
}

// Gets a date string in the format YYYY-MM
String getMonthString()
{
    struct tm timeinfo;
    char dateString[20];
    if (!getLocalTime(&timeinfo))
    {
        return "1970-01-01";
    }
    sprintf(dateString, "%d-%02d", timeinfo.tm_year + 1900, timeinfo.tm_mon + 1);
    return String(dateString);
}

// Converts a time string in 24hr format (e.g. 00:00 to 23:59) to the number of minutes since midnight
uint16_t timeToMinutes(String s)
{
    if (s.length() != 5 || s[0] > '9' || s[0] < '0' || s[1] > '9' || s[1] < '0' || s[2] != ':' || s[3] > '9' || s[3] < '0' || s[4] > '9' || s[4] < '0')
        return 0;

    uint8_t h_int = s.substring(0, 2).toInt();
    uint8_t m_int = s.substring(3, 5).toInt();

    if (h_int > 23 || m_int > 59)
        return 0;

    uint16_t t = (h_int * 60) + m_int;
    return t;
}

void UpdateDisplayFields(PlantFlowData_t &flowData, PlantTotals_t &dailyTotals, bool &ready, bool &refreshed)
{
    if (WiFi.status() == WL_CONNECTED)
    {
        // Prevent Wi-Fi symbol hysteresis
        if (WiFi.RSSI() > lastRssi + 2 || WiFi.RSSI() < lastRssi - 2)
        {
            lastRssi = rssi;
            rssi = WiFi.RSSI();
        }

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

/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
    int w = (area->x2 - area->x1 + 1);
    int h = (area->y2 - area->y1 + 1);
    gfx->startWrite();
    gfx->setAddrWindow(area->x1, area->y1, w, h);
    gfx->writePixels((lgfx::rgb565_t*)&color_p->full, w*h);
    gfx->endWrite();
    lv_disp_flush_ready(disp);
}


int16_t touch_last_x = 0, touch_last_y = 0;

void my_touchpad_read( lv_indev_drv_t * indev_driver, lv_indev_data_t * data )
{
    uint16_t touchX, touchY;
    data->state = LV_INDEV_STATE_REL;
    if(gfx->getTouchRaw( &touchX, &touchY ))
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
