#include <lvgl.h>
#include <TFT_eSPI.h>
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <Preferences.h>
#include <Time.h>
#include <TimeLib.h>

#include "Config.h"
#include "SunsynkApi.h"
#include "ui.h"

/* Screen resolution */
static const uint16_t screenWidth  = 480;
static const uint16_t screenHeight = 320;

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[ screenWidth * screenHeight / 10 ];

// Variables for looping
long lastUpdateTime = 0;
int loopDelaySec = 30;

const char compileDate[] = __DATE__ " " __TIME__;

// Global TFT instance
TFT_eSPI tft = TFT_eSPI(screenWidth, screenHeight);

// Global instance of preferences
Preferences prefs;

// Global instance of Wi-Fi client
WiFiMulti WiFiMulti;

// Connect to WiFi
void connectWifI() {
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);

  Serial.print(" - Waiting for WiFi to connect ...");
  while ((WiFiMulti.run() != WL_CONNECTED)) {
    Serial.print(".");
  }
  Serial.println(" connected");
}

// Use NTP to set the clock
void setClock() {
  configTzTime(TZ_INFO, "pool.ntp.org");

  Serial.print(F(" - Waiting for NTP time sync ..."));
  time_t nowSecs = time(nullptr);
  while (nowSecs < 8 * 3600 * 2) {
    delay(500);
    Serial.print(F("."));
    yield();
    nowSecs = time(nullptr);
  }
  Serial.println(" synced");
}

// Get current epoch time
unsigned long getTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return(0);
  }
  time(&now);
  return now;
}

// Converts an epoch time to a datetime string
String getDateTimeString(unsigned long val) {
  char timeString[20];
  sprintf(timeString, "%4d-%02d-%02d %02d:%02d:%02d", year(val), month(val), day(val), hour(val), minute(val), second(val));
  return String(timeString);
}

// Gets a 24hr clock time string
String getTimeString() {
  struct tm timeinfo;
  char timeString[20];
  if (!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return "00:00";
  }
  sprintf(timeString, "%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);
  return String(timeString);
}

// Get a string preference and return it
String GetPrefString(char* prefName) {
  prefs.begin("sunsynk", false);
  String prefValue = prefs.getString(prefName, "");
  prefs.end();
  return prefValue;
}

// Get a ULong preference and return it
unsigned long GetPrefULong(char* prefName) {
  prefs.begin("sunsynk", false);
  unsigned long prefValue = prefs.getULong(prefName, 0);
  prefs.end();
  return prefValue;
}

#if LV_USE_LOG != 0
/* Serial debugging */
void my_print(const char * buf)
{
    Serial.printf(buf);
    Serial.flush();
}
#endif

/* Display flushing */
void my_disp_flush( lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p )
{
    uint32_t w = ( area->x2 - area->x1 + 1 );
    uint32_t h = ( area->y2 - area->y1 + 1 );

    tft.startWrite();
    tft.setAddrWindow( area->x1, area->y1, w, h );
    tft.pushColors( ( uint16_t * )&color_p->full, w * h, true );
    tft.endWrite();

    lv_disp_flush_ready( disp );
}

/*Read the touchpad*/
void my_touchpad_read( lv_indev_drv_t * indev_driver, lv_indev_data_t * data )
{
    uint16_t touchX = 0, touchY = 0;
    bool touched = tft.getTouch( &touchX, &touchY, 600 );

    if( !touched )
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

void setup()
{
    Serial.begin( 115200 );

    String LVGL_Arduino = "Hello Arduino! ";
    LVGL_Arduino += String('V') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();

    Serial.println( LVGL_Arduino );
    Serial.println( "I am LVGL_Arduino" );

    lv_init();

  Serial.printf("\nFirmware Date: %s\n", compileDate);
  Serial.println("\nBooting ...");


#if LV_USE_LOG != 0
    lv_log_register_print_cb( my_print ); /* register print function for debugging */
#endif

    tft.begin();          /* TFT init */
    tft.setRotation( 3 ); /* Landscape orientation, flipped */

    uint16_t calData[5] = { 178, 3745, 80, 3693, 1 };
    tft.setTouch(calData);

    lv_disp_draw_buf_init( &draw_buf, buf, NULL, screenWidth * screenHeight / 10 );

    /*Initialize the display*/
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init( &disp_drv );
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register( &disp_drv );

    /*Initialize the (dummy) input device driver*/
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init( &indev_drv );
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_touchpad_read;
    lv_indev_drv_register( &indev_drv );

    // Initialize the TFT and show a message
    ui_init();
    tft.fillScreen(TFT_BLACK);
    //tft.setTextSize(2);
    tft.drawCentreString("Starting up ...", tft.width()/2, tft.height()/2, 2);

    // Connect to WiFi
    connectWifI();

    // Set the clock
    setClock();

    Serial.printf("\nNetwork SSID: %s\n", WIFI_SSID);
    Serial.printf("IP Address:   %s\n", WiFi.localIP().toString().c_str());
    Serial.printf("\nUTC Time:     %s\n", getDateTimeString(getTime()).c_str());
    Serial.println("\nReady.\n");

    Serial.println( "Setup done" );
}

void loop()
{
  // Only poll the sensor at startup or after the interval has elapsed
  if ((lastUpdateTime == 0) || (millis() - lastUpdateTime >= (loopDelaySec * 1000L)))
  {
    // Get access token and token expiry from store
    String accessToken = GetPrefString("access-token");
    unsigned long tokenExpiry = GetPrefULong("expires-at");

    // Check if the access token is valid
    char* tokenState = "Unknown";
    bool tokenValid = false;
    if (accessToken != "" && tokenExpiry > 0){
      Serial.printf("Stored Token Expiry: %s\n", getDateTimeString(tokenExpiry).c_str());
      if (tokenExpiry > getTime()) {
        tokenState = "Valid";
        tokenValid = true;
      } else {
        tokenState = "Expired";
      }
    } else {
      tokenState = "Missing";
    }

    // Report Sunsynk token state
    Serial.printf("Stored Token State: %s\n", tokenState);
    Serial.println();

    // Get a new token if one is needed
    if (!tokenValid) {
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
    GetLoadTotals();

    // Store the time
    lastUpdateTime = millis();

    // Wait for a bit
    Serial.printf("Waiting %d seconds before next API poll ...\n\n", loopDelaySec);
  }

    // Update the time
    String timeNow = getTimeString();
    lv_label_set_text(ui_time, timeNow.c_str());

    lv_timer_handler(); /* let the GUI do its work */
    delay(5);
}
