#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <lvgl.h>
#include <MQTT.h>
#include <WiFi.h>
#include <Time.h>
#include <TimeLib.h>
#include "esp_sntp.h"

#include <Sunsynk.h>

#include "Config.h"
#include "esp32_ili9488_spi.h"
#include "esp32s3_elecrow_hmi_35.h"
#include "esp32s3_elecrow_hmi_50.h"
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

// WiFi Client
WiFiClient wifi;

// MQTT Client
MQTTClient mqtt;

// Struct for storing the realtime flow data.
PlantFlowData_t flowData;

// Struct for storing the daily totals.
PlantTotals_t dailyTotals;

BattData_t mqttBattData;

PvData_t mqttPvData;

InverterData_t mqttInverterData;

LoadData_t mqttLoadData;

GridData_t mqttGridData;

// Struct for storing the realtime flow data.
PlantFlowData_t mqttFlowData;

// Struct for storing the daily totals.
PlantTotals_t mqttDailyTotals;

// Struct for storing the daily plot data.
PlantDailyPlot_t dailyPlotData;

// Flag to indicate that all API calls are complete and the data is ready to be transferred to the display.
boolean ihdDataReady = false;

// Flag to indicate that the most recent API data has been displayed.
boolean ihdScreenRefreshed = false;

// Flag to indicate that API calls are in progress
boolean apiSyncing = false;

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

bool homeScreenActive = true;
unsigned long detailScreenTimeout = 0;

// LVGL structs
static lv_disp_draw_buf_t draw_buf;
static lv_color_t *disp_draw_buf;
static lv_disp_drv_t disp_drv;

// Connects to the Wi-Fi network.
void connectWiFi(void);

// Callback for Wi-Fi disconnection
void WiFiDisconnected(WiFiEvent_t event, WiFiEventInfo_t info);

// Callback for Wi-Fi connection
void WiFiReconnected(WiFiEvent_t event, WiFiEventInfo_t info);

// Connects to the MQTT broker.
void connectMQTT(void);

// Callback for MQTT message received.
void mqttReceived(String &topic, String &payload);

// Configures NTP and syncs the time.
void configureNtp(void);

// Builds a version string from the date and time.
void getVersion(char *buff);

// Determines if night mode should be enabled or not. Returns true (night mode) or false (day mode).
boolean IsNightMode(void);

// Toggles night mode on or off. Night mode dims the screen and disables API polling.
void ToggleNightMode(void);

// Toggles API polling on or off.
void TogglePolling(void);

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

// Converts a double to string with single digit precision
String doubleToString(double d);

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

    // Connect to MQTT
    infoMessage = "Connecting to Solar Assistant ...";
    mqtt.begin(MQTT_BROKER, wifi);
    mqtt.onMessage(mqttReceived);
    connectMQTT();

    // Authenticate
    //infoMessage = "Authenticating ...";
    //sunsynk.Authenticate(SUNSYNK_USERNAME, SUNSYNK_PASSWORD, SUNSYNK_REGION);

    // Create the task to get API data
    infoMessage = "Fetching data ...";
    //uint32_t api_call_delay = 30000; // 30 seconds
    uint32_t api_call_delay = 1000; // 5 seconds
    xTaskCreate(TaskCallApi, "API Call Task", 20480, (void *)&api_call_delay, 2, &TaskCallApi_h);

    // Create the task to update the time on the IHD display
    uint32_t time_delay = 500; // One second
    xTaskCreate(TaskClock, "Task Clock", 2048, (void *)&time_delay, 2, NULL);

    // Wait up to 10s for API data
    /*
    int delayEnd = millis() + (10 * 1000);
    while (!ihdDataReady && millis() < delayEnd)
    {
        delay(100);
    }
    */

    infoMessage = "Ready!";
    delay(500);

    // Hide the info panel
    infoMessage = "";
    showInfoMessage = false;
}

void loop()
{
    mqtt.loop();
    delay(10);
    if (!mqtt.connected()) {
        Serial.println("MQTT not connected!");
        connectMQTT();
    }
    // Nothing to do here as all the work is done by the tasks
}

// Task to make API calls. Should run every 30s as the API will have new data at most every 60s (but could be 300s).
void TaskCallApi(void *pvParameters)
{
    uint32_t api_delay = *((uint32_t *)pvParameters);
    for (;;)
    {
        // Show the syncing icon
        apiSyncing = true;

        // Signal that data isn't ready
        ihdDataReady = false;

/*
        // Check if the access token is still valid, renew it if now
        if (!sunsynk.CheckAccessToken())
        {
            sunsynk.Authenticate(SUNSYNK_USERNAME, SUNSYNK_PASSWORD, SUNSYNK_REGION);
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
*/
        flowData = mqttFlowData;
        dailyTotals = mqttDailyTotals;

        // Signal that data is ready
        ihdDataReady = true;

        // Signal that the IHD has not been refreshed with this data
        ihdScreenRefreshed = false;

        // Hide the syncing icon
        apiSyncing = false;

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
        ToggleNightMode();

        // Check if we need to return to the home screen
        if (homeScreenActive == false && detailScreenTimeout < getTime())
        {
            _ui_screen_change(&ui_homeScreen, LV_SCR_LOAD_ANIM_NONE, 100, 0, &ui_homeScreen_screen_init);
        }

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
        // Check if we need to toggle API polling
        TogglePolling();

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

    // Register Wi-Fi disconnection callback
    WiFi.onEvent(WiFiDisconnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);

    // Register Wi-Fi reconnection callback
    WiFi.onEvent(WiFiReconnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_CONNECTED);
}

// Handle Wi-Fi disconnection
void WiFiDisconnected(WiFiEvent_t event, WiFiEventInfo_t info)
{
    Serial.println("Disconnected from WiFi access point");
    TogglePolling();

    Serial.println("Trying to reconnect ...");
    WiFi.reconnect();
}

// Handle Wi-Fi reconnection
void WiFiReconnected(WiFiEvent_t event, WiFiEventInfo_t info)
{
    Serial.println("Reconnected to WiFi access point");
    TogglePolling();
}

void connectMQTT() {
    Serial.print(" - Connecting to MQTT broker ...");
    char espid[17];
    snprintf(espid, 17, "ihd-%llX", ESP.getEfuseMac());

    while (!mqtt.connect(espid)) {
        Serial.print(".");
        delay(1000);
    }

    Serial.println(" connected");
    mqtt.subscribe("solar_assistant/inverter_1/grid_power/state");
    mqtt.subscribe("solar_assistant/inverter_1/load_power/state");
    mqtt.subscribe("solar_assistant/inverter_1/pv_power/state");
    mqtt.subscribe("solar_assistant/total/battery_power/state");
    mqtt.subscribe("solar_assistant/total/battery_state_of_charge/state");
    mqtt.subscribe("solar_assistant/total/battery_energy_in/state");
    mqtt.subscribe("solar_assistant/total/battery_energy_out/state");
    mqtt.subscribe("solar_assistant/total/grid_energy_in/state");
    mqtt.subscribe("solar_assistant/total/grid_energy_out/state");
    mqtt.subscribe("solar_assistant/total/load_energy/state");
    mqtt.subscribe("solar_assistant/total/pv_energy/state");

    // Battery details
    mqtt.subscribe("solar_assistant/inverter_1/battery_current/state");
    mqtt.subscribe("solar_assistant/inverter_1/battery_voltage/state");
    mqtt.subscribe("solar_assistant/total/battery_temperature/state");

    // PV details
    mqtt.subscribe("solar_assistant/inverter_1/pv_current_1/state");
    mqtt.subscribe("solar_assistant/inverter_1/pv_current_2/state");
    mqtt.subscribe("solar_assistant/inverter_1/pv_voltage_1/state");
    mqtt.subscribe("solar_assistant/inverter_1/pv_voltage_2/state");
    mqtt.subscribe("solar_assistant/inverter_1/pv_power_1/state");
    mqtt.subscribe("solar_assistant/inverter_1/pv_power_2/state");

    // Inverter details
    mqtt.subscribe("solar_assistant/inverter_1/ac_output_voltage/state");
    mqtt.subscribe("solar_assistant/inverter_1/ac_output_frequency/state");
    mqtt.subscribe("solar_assistant/inverter_1/temperature/state");
    mqtt.subscribe("solar_assistant/inverter_1/device_mode/state");
    mqtt.subscribe("solar_assistant/inverter_1/work_mode/state");
    mqtt.subscribe("solar_assistant/inverter_1/energy_pattern/state");

    // Load details
    mqtt.subscribe("solar_assistant/inverter_1/load_percentage/state");
    mqtt.subscribe("solar_assistant/inverter_1/load_power_non-essential/state");
    mqtt.subscribe("solar_assistant/inverter_1/load_power_essential/state");
    
    //Grid details
    mqtt.subscribe("solar_assistant/inverter_1/grid_power_ld/state");
    mqtt.subscribe("solar_assistant/inverter_1/grid_power_ct/state");
    mqtt.subscribe("solar_assistant/inverter_1/grid_voltage/state");
    mqtt.subscribe("solar_assistant/inverter_1/grid_frequency/state");
}

void mqttReceived(String &topic, String &payload) {
    //Serial.println("Incoming: " + topic + ": " + payload);

    if (topic == "solar_assistant/inverter_1/grid_power/state") {
        mqttFlowData.gridWatts = payload.toInt();
        mqttFlowData.toGrid = (mqttFlowData.gridWatts < 0);
    } else if (topic == "solar_assistant/inverter_1/load_power/state") {
        mqttFlowData.loadWatts = payload.toInt();
    } else if (topic == "solar_assistant/inverter_1/pv_power/state") {
        mqttFlowData.pvWatts = payload.toInt();
    } else if (topic == "solar_assistant/total/battery_power/state") {
        mqttFlowData.battWatts = payload.toInt();
        mqttFlowData.toBatt = (mqttFlowData.battWatts > 0);
    } else if (topic == "solar_assistant/total/battery_state_of_charge/state") {
        mqttFlowData.battSoc = payload.toInt();
    } else if (topic == "solar_assistant/total/battery_energy_in/state") {
        mqttDailyTotals.battCharge = payload.toDouble();
    } else if (topic == "solar_assistant/total/battery_energy_out/state") {
        mqttDailyTotals.battDischarge = payload.toDouble();
    } else if (topic == "solar_assistant/total/grid_energy_in/state") {
        mqttDailyTotals.gridImport = payload.toDouble();
    } else if (topic == "solar_assistant/total/grid_energy_out/state") {
        mqttDailyTotals.gridExport = payload.toDouble();
    } else if (topic == "solar_assistant/total/pv_energy/state") {
        mqttDailyTotals.pv = payload.toDouble();
    } else if (topic == "solar_assistant/total/load_energy/state") {
        mqttDailyTotals.load = payload.toDouble();
    } else if (topic == "solar_assistant/inverter_1/battery_current/state") {
        mqttBattData.current = payload.toDouble();
    } else if (topic == "solar_assistant/inverter_1/battery_voltage/state") {
        mqttBattData.voltage = payload.toDouble();
    } else if (topic == "solar_assistant/total/battery_temperature/state") {
        mqttBattData.temperature = payload.toDouble();
    } else if (topic == "solar_assistant/inverter_1/pv_current_1/state") {
        mqttPvData.mppt1Current = payload.toDouble();
    } else if (topic == "solar_assistant/inverter_1/pv_current_2/state") {
        mqttPvData.mppt2Current = payload.toDouble();
    } else if (topic == "solar_assistant/inverter_1/pv_voltage_1/state") {
        mqttPvData.mppt1Voltage = payload.toDouble();
    } else if (topic == "solar_assistant/inverter_1/pv_voltage_2/state") {
        mqttPvData.mppt2Voltage = payload.toDouble();
    } else if (topic == "solar_assistant/inverter_1/pv_power_1/state") {
        mqttPvData.mppt1Power = payload.toInt();
    } else if (topic == "solar_assistant/inverter_1/pv_power_2/state") {
        mqttPvData.mppt2Power = payload.toInt();
    } else if (topic == "solar_assistant/inverter_1/ac_output_voltage/state") {
        mqttInverterData.acOutputVoltage = payload.toDouble();
    } else if (topic == "solar_assistant/inverter_1/ac_output_frequency/state") {
        mqttInverterData.acOutputFrequency = payload.toDouble();
    } else if (topic == "solar_assistant/inverter_1/temperature/state") {
        mqttInverterData.temperature = payload.toDouble();
    } else if (topic == "solar_assistant/inverter_1/device_mode/state") {
        mqttInverterData.deviceMode = payload;
    } else if (topic == "solar_assistant/inverter_1/work_mode/state") {
        mqttInverterData.workMode = payload;
    } else if (topic == "solar_assistant/inverter_1/energy_pattern/state") {
        mqttInverterData.energyPattern = payload;
    } else if (topic == "solar_assistant/inverter_1/load_percentage/state") {
        mqttLoadData.loadPercentage = payload.toInt();
    } else if (topic == "solar_assistant/inverter_1/load_power_non-essential/state") {
        mqttLoadData.nonEssentialPower = payload.toInt();
    } else if (topic == "solar_assistant/inverter_1/load_power_essential/state") {
        mqttLoadData.essentialPower = payload.toInt();
    } else if (topic == "solar_assistant/inverter_1/grid_power_ld/state") {
        mqttGridData.ldPower = payload.toInt();
    } else if (topic == "solar_assistant/inverter_1/grid_power_ct/state") {
        mqttGridData.ctPower = payload.toInt();
    } else if (topic == "solar_assistant/inverter_1/grid_voltage/state") {
        mqttGridData.gridVoltage = payload.toDouble();
    } else if (topic == "solar_assistant/inverter_1/grid_frequency/state") {
        mqttGridData.gridFrequency = payload.toDouble();
    }

    // Note: Do not use the client in the callback to publish, subscribe or
    // unsubscribe as it may cause deadlocks when other things arrive while
    // sending and receiving acknowledgments. Instead, change a global variable,
    // or push to a queue and handle it in the loop after calling `client.loop()`.
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
void ToggleNightMode()
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
    } else {
        // Get the backlight state, turn it on if it's off
        if (!backlightOn)
        {
            Serial.println("Turning LCD backlight on.");
            gfx->setBrightness(LCD_BRIGHTNESS_DAY);
            backlightOn = true;
        }
    }
}

// Enable or disable API polling
void TogglePolling()
{
    if (TaskCallApi_h != NULL)
    {
        eTaskState state = eTaskGetState(TaskCallApi_h);

        if (((IsNightMode() == true && LCD_BRIGHTNESS_NIGHT == 0) || WiFi.status() != WL_CONNECTED) && state != eSuspended)
        {
            Serial.println("Suspending API polling task.");
            vTaskSuspend(TaskCallApi_h);
        }

        if ((IsNightMode() == false && WiFi.status() == WL_CONNECTED) && state == eSuspended)
        {
            Serial.println("Resuming API polling task.");
            vTaskResume(TaskCallApi_h);
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
        apiSyncing = false;
    }

    if (apiSyncing)
    {
        lv_obj_clear_flag(ui_syncing, LV_OBJ_FLAG_HIDDEN);
    }
    else
    {
        lv_obj_add_flag(ui_syncing, LV_OBJ_FLAG_HIDDEN);
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
        if (flowData.gridWatts != 0)
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
        lv_label_set_text_fmt(ui_gridWatts, "%d", abs(flowData.gridWatts));

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

        // Update the battery rate
        float battRate = (double)abs(mqttBattData.current) / (double)BATTERY_CAPACITY_AH * (double)100;
        char battRateStr[8];
        dtostrf(battRate, 3, 1, battRateStr);
        String battRateSign = "+";
        if (flowData.battWatts < 0) {
            battRateSign = "-";
        }
        //lv_obj_set_style_text_color(ui_battSocRate, lv_color_hex(battWattsColor), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_label_set_text_fmt(ui_battSocRate, "%s%s %%/hr", battRateSign, battRateStr);

        // Update the battery details
        lv_label_set_text_fmt(ui_battCapValue, "%d Ah", BATTERY_CAPACITY_AH);
        lv_label_set_text_fmt(ui_battSocValue, "%d %%", flowData.battSoc);
        lv_label_set_text_fmt(ui_battPwrValue, "%d W", flowData.battWatts);
        lv_label_set_text_fmt(ui_battCurrValue, "%s A", doubleToString(mqttBattData.current));
        lv_label_set_text_fmt(ui_battVoltValue, "%s V", doubleToString(mqttBattData.voltage));
        lv_label_set_text_fmt(ui_battTempValue, "%s °C", doubleToString(mqttBattData.temperature));

        // Update the PV details
        lv_label_set_text_fmt(ui_mppt1Power, "%d W", mqttPvData.mppt1Power);
        lv_label_set_text_fmt(ui_mppt1Current, "%s A", doubleToString(mqttPvData.mppt1Current));
        lv_label_set_text_fmt(ui_mppt1Voltage, "%s V", doubleToString(mqttPvData.mppt1Voltage));
        lv_label_set_text_fmt(ui_mppt2Power, "%d W", mqttPvData.mppt2Power);
        lv_label_set_text_fmt(ui_mppt2Current, "%s A", doubleToString(mqttPvData.mppt2Current));
        lv_label_set_text_fmt(ui_mppt2Voltage, "%s V", doubleToString(mqttPvData.mppt2Voltage));
        lv_label_set_text_fmt(ui_mpptPowerTotal, "%d W", mqttPvData.mppt1Power + mqttPvData.mppt2Power);

        // Update the inverter details
        lv_label_set_text_fmt(ui_acOutputPowerValue, "%d W", mqttFlowData.loadWatts);
        lv_label_set_text_fmt(ui_acOutputVoltageValue, "%s V", doubleToString(mqttInverterData.acOutputVoltage));
        lv_label_set_text_fmt(ui_acOutputFreqValue, "%s Hz", doubleToString(mqttInverterData.acOutputFrequency));
        lv_label_set_text_fmt(ui_invTempValue, "%s °C", doubleToString(mqttInverterData.temperature));
        lv_label_set_text(ui_deviceModeValue, mqttInverterData.deviceMode.c_str());
        lv_label_set_text(ui_workModeValue, mqttInverterData.workMode.c_str());
        lv_label_set_text(ui_energyPatternValue, mqttInverterData.energyPattern.c_str());

        // Update the grid details
        lv_label_set_text_fmt(ui_gridPowerValue, "%d W", mqttFlowData.gridWatts);
        lv_label_set_text_fmt(ui_gridPowerLdValue, "%d W", mqttGridData.ldPower);
        lv_label_set_text_fmt(ui_gridPowerCtValue, "%d W", mqttGridData.ctPower);
        lv_label_set_text_fmt(ui_gridVoltageValue, "%s V", doubleToString(mqttGridData.gridVoltage));
        lv_label_set_text_fmt(ui_gridFreqValue, "%s Hz", doubleToString(mqttGridData.gridFrequency));

        // Update the load details
        lv_label_set_text_fmt(ui_loadPercentValue, "%d %%", mqttLoadData.loadPercentage);
        lv_label_set_text_fmt(ui_loadPowerValue, "%d W", mqttFlowData.loadWatts);
        lv_label_set_text_fmt(ui_loadPowerEssValue, "%d W", mqttLoadData.essentialPower);
        lv_label_set_text_fmt(ui_loadPowerNonEssValue, "%d W", mqttLoadData.nonEssentialPower);

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

String doubleToString(double d)
{
    char tempStr[8];
    dtostrf(d, 3, 1, tempStr);
    return tempStr;
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
    if(gfx->getTouch(&touchX, &touchY))
    {
        data->state = LV_INDEV_STATE_PR;
        data->point.x = touchX;
        data->point.y = touchY;

        Serial.printf("Touch: X:%d Y:%d\n", data->point.x, data->point.y);
        lastTouchTime = getTime();
    }
}

void homeScreenOnLoad(lv_event_t * e)
{
    // Start a timer to return to the main screen
    Serial.println("Loaded the home screen.");
    homeScreenActive = true;
    detailScreenTimeout = 0;
}

void detailScreenOnLoad(lv_event_t * e)
{
    // Start a timer to return to the main screen
    Serial.println("Loaded a details screen.");
    homeScreenActive = false;
    detailScreenTimeout = getTime() + DETAIL_SCREEN_TIMEOUT;
}

void updatePlotData(lv_event_t * e)
{
    /*
    int16_t minPv = 0;
    int16_t maxPv = 0;
    int16_t minLoad = 0;
    int16_t maxLoad = 0;
    int16_t minGrid = 0;
    int16_t maxGrid = 0;
    int16_t minBatt = 0;
    int16_t maxBatt = 0;
    
    int16_t minY = 0;
    int16_t maxY = 0;

    struct tm timeinfo;

    if (getLocalTime(&timeinfo))
    {
        sunsynk.GetDailyPlotData(SUNSYNK_PLANT_ID, timeinfo, dailyPlotData);

        lv_obj_set_style_size(ui_dailyFlow, 0, LV_PART_INDICATOR);
        lv_chart_set_point_count(ui_dailyFlow, dailyPlotData.count);
        lv_chart_series_t * ui_dailyFlow_pv = lv_chart_add_series(ui_dailyFlow, lv_color_hex(0x4CDB4C), LV_CHART_AXIS_PRIMARY_Y);
        lv_chart_series_t * ui_dailyFlow_load = lv_chart_add_series(ui_dailyFlow, lv_color_hex(0xDE83A6), LV_CHART_AXIS_PRIMARY_Y);
        lv_chart_series_t * ui_dailyFlow_grid = lv_chart_add_series(ui_dailyFlow, lv_color_hex(0xFFDB4C), LV_CHART_AXIS_PRIMARY_Y);
        lv_chart_series_t * ui_dailyFlow_battery = lv_chart_add_series(ui_dailyFlow, lv_color_hex(0x908AFF), LV_CHART_AXIS_PRIMARY_Y);
        lv_chart_series_t * ui_dailyFlow_soc = lv_chart_add_series(ui_dailyFlow, lv_color_hex(0x78E4DE), LV_CHART_AXIS_SECONDARY_Y);

        minPv = * std::min_element(dailyPlotData.pv, dailyPlotData.pv + dailyPlotData.count);
        maxPv = * std::max_element(dailyPlotData.pv, dailyPlotData.pv + dailyPlotData.count);
        minLoad = * std::min_element(dailyPlotData.load, dailyPlotData.load + dailyPlotData.count);
        maxLoad = * std::max_element(dailyPlotData.load, dailyPlotData.load + dailyPlotData.count);
        minGrid = * std::min_element(dailyPlotData.grid, dailyPlotData.grid + dailyPlotData.count);
        maxGrid = * std::max_element(dailyPlotData.grid, dailyPlotData.grid + dailyPlotData.count);
        minBatt = * std::min_element(dailyPlotData.battery, dailyPlotData.battery + dailyPlotData.count);
        maxBatt = * std::max_element(dailyPlotData.battery, dailyPlotData.battery + dailyPlotData.count);

        int mins[] = {minPv, minLoad, minGrid, minBatt};
        int maxs[] = {maxPv, maxLoad, maxGrid, maxBatt};
        minY = * std::min_element(mins, mins + 4);
        maxY = * std::max_element(maxs, maxs + 4);

        for (int i=0; i < dailyPlotData.count; i++)
        {
            lv_chart_set_value_by_id(ui_dailyFlow, ui_dailyFlow_pv, i, dailyPlotData.pv[i]);
            lv_chart_set_value_by_id(ui_dailyFlow, ui_dailyFlow_load, i, dailyPlotData.load[i]);
            lv_chart_set_value_by_id(ui_dailyFlow, ui_dailyFlow_grid, i, dailyPlotData.grid[i]);
            lv_chart_set_value_by_id(ui_dailyFlow, ui_dailyFlow_battery, i, dailyPlotData.battery[i]);
            lv_chart_set_value_by_id(ui_dailyFlow, ui_dailyFlow_soc, i, dailyPlotData.soc[i]);
        }
        lv_chart_set_range(ui_dailyFlow, LV_CHART_AXIS_PRIMARY_Y, minY, maxY);
    }
    */
}
