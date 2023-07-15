#include <ArduinoJson.h>
#include <Preferences.h>

#include "Config.h"
#include "SunsynkApi.h"
#include "ui.h"

void GetSunsynkAuthToken() {
  Serial.println("Fetching new Sunsynk auth token ...");
  WiFiClientSecure *client = new WiFiClientSecure;
  if(client) {
    client -> setCACert(SUNSYNK_API_CERT);
    {
      // Add a scoping block for HTTPClient https to make sure it is destroyed before WiFiClientSecure *client is 
      HTTPClient https;
      if (https.begin(*client, SUNSYNK_LOGIN_URL)) {  // HTTPS

        // start connection and send HTTP header
        https.addHeader("Content-Type", "application/json");
        https.addHeader("Accept", "application/json");

        char authPayload[1024];
        sprintf(authPayload, "{\"username\": \"%s\", \"password\": \"%s\", \"grant_type\": \"password\", \"client_id\":\"csp_web\"}", SUNSYNK_USERNAME, SUNSYNK_PASSWORD);

        int httpCode = https.POST(authPayload);
  
        if (httpCode > 0) {
          // HTTP header has been send and Server response header has been handled
          Serial.printf("Response: %d\n", httpCode);
  
          // file found at server
          if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
            String authReponse = https.getString();

            StaticJsonDocument<512> authResponseJson;
            DeserializationError error = deserializeJson(authResponseJson, authReponse);

            if (error) {
              Serial.print(F("deserializeJson() failed: "));
              Serial.println(error.f_str());
              return;
            }

            const char* accessToken = authResponseJson["data"]["access_token"];
            const char* refreshToken = authResponseJson["data"]["refresh_token"];
            unsigned long expiresIn = authResponseJson["data"]["expires_in"];
            unsigned long expiresAt = getTime() + expiresIn;
            Serial.printf("New Token Expires: %s\n\n", getDateTimeString(expiresAt).c_str());

            prefs.begin("sunsynk", false);
            prefs.putString("access-token", accessToken);
            prefs.putString("refresh-token", refreshToken);
            prefs.putULong("expires-in", expiresIn);
            prefs.putULong("expires-at", expiresAt);
            prefs.end();
          }
        } else {
          Serial.printf("[HTTPS] POST... failed, error: %s\n", https.errorToString(httpCode).c_str());
        }
  
        https.end();
      } else {
        Serial.printf("[HTTPS] Unable to connect\n");
      }

      // End extra scoping block
    }
  
    client -> stop();
    delete client;
  } else {
    Serial.println("Unable to create client");
  }
}

String CallSunsynkApi(String uri) {
  //Serial.println(uri);
  
  String payload;
  WiFiClientSecure *client = new WiFiClientSecure;

  if(client) {
    client -> setCACert(SUNSYNK_API_CERT);
    {
      // Add a scoping block for HTTPClient https to make sure it is destroyed before WiFiClientSecure *client is 
      HTTPClient https;  
      if (https.begin(*client, uri)) {  // HTTPS

        // start connection and send HTTP header
        https.addHeader("Authorization", "Bearer " + GetPrefString("access-token"));
        https.addHeader("Content-Type", "application/json");
        https.addHeader("Accept", "application/json");

        int httpCode = https.GET();
  
        if (httpCode > 0) {
          Serial.printf("Response: %d\n", httpCode);
          if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
            payload = https.getString();
          }
        } else {
          Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
        }
  
        https.end();
      } else {
        Serial.printf("[HTTPS] Unable to connect\n");
      }
      // End extra scoping block
    }
    client -> stop();
    delete client;
  } else {
    Serial.println("Unable to create client");
  }
  return payload;
}

void GetPlantRealtime() {
  Serial.println("Getting plant realtime data ...");
  char apiUri[128];
  sprintf(apiUri, "%s/plant/%s/realtime", SUNSYNK_API_URL, SUNSYNK_PLANT_ID);
  String payload = CallSunsynkApi(apiUri);

  StaticJsonDocument<2048> responseJson;
  DeserializationError error = deserializeJson(responseJson, payload);

  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

  //serializeJsonPretty(responseJson, Serial);
  //Serial.println();

  if (responseJson["code"] == 0 && responseJson["msg"] == "Success") {
    int pac = responseJson["data"]["pac"];
    double eTodayDbl = responseJson["data"]["etoday"];

    char eTodayStr[8];
    dtostrf(eTodayDbl,3,1,eTodayStr);

    lv_label_set_text(ui_pvTotal, eTodayStr);

    Serial.println();
    Serial.printf("PV Now:   %6d W\n", pac);
    Serial.printf("PV Today: %6s kWh\n", eTodayStr);
    Serial.println();
  }
}

void GetPlantFlow() {
  Serial.println("Getting plant flow data ...");
  char apiUri[128];
  sprintf(apiUri, "%s/plant/energy/%s/flow", SUNSYNK_API_URL, SUNSYNK_PLANT_ID);
  String payload = CallSunsynkApi(apiUri);

  StaticJsonDocument<2048> responseJson;
  DeserializationError error = deserializeJson(responseJson, payload);

  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

  //serializeJsonPretty(responseJson, Serial);
  //Serial.println();

  if (responseJson["code"] == 0 && responseJson["msg"] == "Success") {
    int pvPower = responseJson["data"]["pvPower"];                    // Energy from solar generation
    int battPower = responseJson["data"]["battPower"];                // Energy flowing in or out of the battery
    int gridOrMeterPower = responseJson["data"]["gridOrMeterPower"];  // Energy flowing in or out of the grid
    int loadOrEpsPower = responseJson["data"]["loadOrEpsPower"];      // Energy flowing to the load
    int battSoc = responseJson["data"]["soc"];                        // Battery state of charge percentage
    bool toBatt = responseJson["data"]["toBat"];                      // True if battery is charging; false if discharging
    bool toGrid = responseJson["data"]["toGrid"];                     // True if exporting; false if importing

    // Update the PV energy
    lv_label_set_text_fmt(ui_pvWatts, "%dW", pvPower);
    if (pvPower == 0) {
      lv_obj_set_style_text_color(ui_pvWatts, lv_color_hex(UI_GREY), LV_PART_MAIN | LV_STATE_DEFAULT);
    } else {
      lv_obj_set_style_text_color(ui_pvWatts, lv_color_hex(UI_GREEN), LV_PART_MAIN | LV_STATE_DEFAULT);
    }
    
    // Update the grid energy
    int gridWattsColor = UI_GREY;
    int gridExportColor = UI_WHITE;
    int gridImportColor = UI_WHITE;
    int gridExportAlpha = 100;
    int gridImportAlpha = 100;
    
    if (gridOrMeterPower > 0) {
      if (toGrid) { // Exporting
        gridWattsColor = UI_GREEN;
        //gridExportColor = UI_GREEN;
        //gridExportAlpha = 255;
      } else { // Importing
        gridWattsColor = UI_RED;
        //gridImportColor = UI_RED;
        //gridImportAlpha = 255;
      }
    }

    // Grid watts colour
    lv_obj_set_style_text_color(ui_gridWatts, lv_color_hex(gridWattsColor), LV_PART_MAIN | LV_STATE_DEFAULT);

    // Grid Export arrow colour
    lv_obj_set_style_img_recolor(ui_gridExportArrow, lv_color_hex(gridExportColor), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor_opa(ui_gridExportArrow, gridExportAlpha, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Grid import arrow colour
    lv_obj_set_style_img_recolor(ui_gridImportArrow, lv_color_hex(gridImportColor), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor_opa(ui_gridImportArrow, gridImportAlpha, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Grid watts value
    lv_label_set_text_fmt(ui_gridWatts, "%dW", gridOrMeterPower);
    
    // Update the battery energy
    if (battPower == 0) { // Neither charging nor discharging
      lv_obj_set_style_text_color(ui_battWatts, lv_color_hex(UI_GREY), LV_PART_MAIN | LV_STATE_DEFAULT);
    } else {
      if (toBatt) { // Charging
        lv_obj_set_style_text_color(ui_battWatts, lv_color_hex(UI_GREEN), LV_PART_MAIN | LV_STATE_DEFAULT);
        battPower = battPower * -1;        
      } else { // Discharging
        lv_obj_set_style_text_color(ui_battWatts, lv_color_hex(UI_RED), LV_PART_MAIN | LV_STATE_DEFAULT);
      }
    }
    lv_label_set_text_fmt(ui_battWatts, "%dW", battPower);

    // Update the load energy
    if (loadOrEpsPower == 0) {
      lv_obj_set_style_text_color(ui_loadWatts, lv_color_hex(UI_GREY), LV_PART_MAIN | LV_STATE_DEFAULT);
    } else {
      lv_obj_set_style_text_color(ui_loadWatts, lv_color_hex(UI_RED), LV_PART_MAIN | LV_STATE_DEFAULT);
    }
    lv_label_set_text_fmt(ui_loadWatts, "%dW", loadOrEpsPower);

    // Update the battery SOC
    lv_label_set_text_fmt(ui_battSoc, "%d%%", battSoc);

    Serial.println();
    Serial.printf("PV:      %6d W\n", pvPower);
    Serial.printf("Battery: %6d W\n", battPower);
    Serial.printf("Grid:    %6d W\n", gridOrMeterPower);
    Serial.printf("Load:    %6d W\n", loadOrEpsPower);
    Serial.printf("SOC:     %6d %%\n", battSoc);
    Serial.println();
  }
}

void GetGridTotals() {
  Serial.println("Getting grid daily total data ...");
  char apiUri[128];
  sprintf(apiUri, "%s/inverter/grid/%s/realtime?sn=%s", SUNSYNK_API_URL, SUNSYNK_INVERTER_ID, SUNSYNK_INVERTER_ID);
  String payload = CallSunsynkApi(apiUri);

  StaticJsonDocument<768> responseJson;
  DeserializationError error = deserializeJson(responseJson, payload);

  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

  //serializeJsonPretty(responseJson, Serial);
  //Serial.println();

  if (responseJson["code"] == 0 && responseJson["msg"] == "Success") {

    double eTodayToDbl = responseJson["data"]["etodayTo"];
    char eTodayToStr[8];
    dtostrf(eTodayToDbl,3,1,eTodayToStr);
    lv_label_set_text(ui_gridDailyExport, eTodayToStr);

    double eTodayFromDbl = responseJson["data"]["etodayFrom"];
    char eTodayFromStr[8];
    dtostrf(eTodayFromDbl,3,1,eTodayFromStr);
    lv_label_set_text(ui_gridDailyImport, eTodayFromStr);

    Serial.println();
    Serial.printf("Export Today: %4s kWh\n", eTodayToStr);
    Serial.printf("Import Today: %4s kWh\n", eTodayFromStr);
    Serial.println();
  }
}

void GetBatteryTotals() {
  Serial.println("Getting battery daily total data ...");
  char apiUri[128];
  sprintf(apiUri, "%s/inverter/battery/%s/realtime?sn=%s&lan=en", SUNSYNK_API_URL, SUNSYNK_INVERTER_ID, SUNSYNK_INVERTER_ID);
  String payload = CallSunsynkApi(apiUri);

  StaticJsonDocument<2048> responseJson;
  DeserializationError error = deserializeJson(responseJson, payload);

  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

  //serializeJsonPretty(responseJson, Serial);
  //Serial.println();

  if (responseJson["code"] == 0 && responseJson["msg"] == "Success") {

    double eTodayChgDbl = responseJson["data"]["etodayChg"];
    char eTodayChgStr[8];
    dtostrf(eTodayChgDbl,3,1,eTodayChgStr);
    lv_label_set_text(ui_battDailyCharge, eTodayChgStr);

    double eTodayDischgDbl = responseJson["data"]["etodayDischg"];
    char eTodayDischgStr[8];
    dtostrf(eTodayDischgDbl,3,1,eTodayDischgStr);
    lv_label_set_text(ui_battDailyDischarge, eTodayDischgStr);

    Serial.println();
    Serial.printf("Charge Today:    %4s kWH\n", eTodayChgStr);
    Serial.printf("Dischange Today: %4s kWh\n", eTodayDischgStr);
    Serial.println();
  }
}

void GetLoadTotals() {
  Serial.println("Getting load daily total data ...");
  char apiUri[128];
  sprintf(apiUri, "%s/inverter/load/%s/realtime?sn=%s", SUNSYNK_API_URL, SUNSYNK_INVERTER_ID, SUNSYNK_INVERTER_ID);
  String payload = CallSunsynkApi(apiUri);

  StaticJsonDocument<512> responseJson;
  DeserializationError error = deserializeJson(responseJson, payload);

  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

  //serializeJsonPretty(responseJson, Serial);
  //Serial.println();

  if (responseJson["code"] == 0 && responseJson["msg"] == "Success") {
    double loadTotalDbl = responseJson["data"]["dailyUsed"];
    char eLoadTotalStr[8];
    dtostrf(loadTotalDbl,3,1,eLoadTotalStr);
    lv_label_set_text(ui_loadTotal, eLoadTotalStr);

    Serial.println();
    Serial.printf("Load Today: %4s kWh\n", eLoadTotalStr);
    Serial.println();
  }
}