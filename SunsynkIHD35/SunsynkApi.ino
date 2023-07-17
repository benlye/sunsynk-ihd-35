#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

#include "Config.h"
#include "SunsynkApi.h"
#include "ui.h"

ApiToken apiToken;

boolean CheckSunsynkAuthToken() {

    // Check if the access token is valid
    char* tokenState = "Unknown";
    bool tokenValid = false;
    if (apiToken.accessToken != "" && apiToken.expiresIn > 0){
    Serial.printf("Stored Token Expiry: %s\n", getDateTimeString(apiToken.expiresAt).c_str());
    if (apiToken.expiresAt > getTime()) {
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

    return tokenValid;
}

void GetSunsynkAuthToken() {
  Serial.println("Fetching new Sunsynk auth token ...");
  DynamicJsonDocument authResponseJson(384);
  WiFiClientSecure *client = new WiFiClientSecure;

  if(client) {
    client -> setCACert(SUNSYNK_API_CERT);
    {
      // Add a scoping block for HTTPClient https to make sure it is destroyed before WiFiClientSecure *client is
      HTTPClient https;
      https.useHTTP10(true);
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
            DeserializationError error = deserializeJson(authResponseJson, https.getStream());

            if (error) {
              Serial.print(F("deserializeJson() failed: "));
              Serial.println(error.f_str());
            } else {
              const char* accessToken = authResponseJson["data"]["access_token"];
              const char* refreshToken = authResponseJson["data"]["refresh_token"];
              unsigned long expiresIn = authResponseJson["data"]["expires_in"];
              unsigned long expiresAt = getTime() + expiresIn;
              Serial.printf("New Token Expires: %s\n\n", getDateTimeString(expiresAt).c_str());

              apiToken.accessToken = String(accessToken);
              apiToken.refreshToken = String(refreshToken);
              apiToken.expiresIn = expiresIn;
              apiToken.expiresAt = expiresAt;
            }
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

DynamicJsonDocument CallSunsynkApi(String uri, int size, DeserializationOption::Filter(filter)) {
  //Serial.println(uri);
  DynamicJsonDocument doc(size);
  WiFiClientSecure *client = new WiFiClientSecure;
  if(client) {
    client -> setCACert(SUNSYNK_API_CERT);
    {
      // Add a scoping block for HTTPClient https to make sure it is destroyed before WiFiClientSecure *client is
      HTTPClient https;
      https.useHTTP10(true);
      if (https.begin(*client, uri)) {  // HTTPS

        // start connection and send HTTP header
        https.addHeader("Authorization", "Bearer " + apiToken.accessToken);
        https.addHeader("Content-Type", "application/json");
        https.addHeader("Accept", "application/json");

        int httpCode = https.GET();

        if (httpCode > 0) {
          Serial.printf("Response: %d\n", httpCode);
          if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
            DeserializationError error = deserializeJson(doc, https.getStream(), DeserializationOption::Filter(filter));
            if (error) {
              Serial.print(F("deserializeJson() failed: "));
              Serial.println(error.f_str());
            }
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
  return doc;
}

void GetPlantRealtime() {
  Serial.println("Getting plant realtime data ...");
  char apiUri[128];
  sprintf(apiUri, "%s/plant/%s/realtime", SUNSYNK_API_URL, SUNSYNK_PLANT_ID);

  StaticJsonDocument<200> filter;
  filter["code"] = true;
  filter["msg"] = true;
  filter["data"]["pac"] = true;
  filter["data"]["etoday"] = true;

  int docSize = 192;
  DynamicJsonDocument responseJson(docSize);
  responseJson = CallSunsynkApi(apiUri, docSize, DeserializationOption::Filter(filter));

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

  StaticJsonDocument<200> filter;
  filter["code"] = true;
  filter["msg"] = true;
  filter["data"]["pvPower"] = true;
  filter["data"]["battPower"] = true;
  filter["data"]["gridOrMeterPower"] = true;
  filter["data"]["loadOrEpsPower"] = true;
  filter["data"]["soc"] = true;
  filter["data"]["toBat"] = true;
  filter["data"]["toGrid"] = true;

  int docSize = 512;
  DynamicJsonDocument responseJson(docSize);
  responseJson = CallSunsynkApi(apiUri, docSize, DeserializationOption::Filter(filter));

  if (responseJson["code"] == 0 && responseJson["msg"] == "Success") {
    int pvPower = responseJson["data"]["pvPower"];                    // Energy from solar generation
    int battPower = responseJson["data"]["battPower"];                // Energy flowing in or out of the battery
    int gridOrMeterPower = responseJson["data"]["gridOrMeterPower"];  // Energy flowing in or out of the grid
    int loadOrEpsPower = responseJson["data"]["loadOrEpsPower"];      // Energy flowing to the load
    int battSoc = responseJson["data"]["soc"];                        // Battery state of charge percentage
    bool toBatt = responseJson["data"]["toBat"];                      // True if battery is charging; false if discharging
    bool toGrid = responseJson["data"]["toGrid"];                     // True if exporting; false if importing

    // Update the PV energy
    int pvWattsColor = UI_GREY;
    if (pvPower > 0) { // Generating
      pvWattsColor = UI_GREEN;
    }
    lv_obj_set_style_text_color(ui_pvWatts, lv_color_hex(pvWattsColor), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_label_set_text_fmt(ui_pvWatts, "%dW", pvPower);

    // Update the grid energy
    int gridWattsColor = UI_GREY;
    if (gridOrMeterPower > 0) {
      if (toGrid) { // Exporting
        gridWattsColor = UI_GREEN;
      } else { // Importing
        gridWattsColor = UI_RED;
      }
    }
    lv_obj_set_style_text_color(ui_gridWatts, lv_color_hex(gridWattsColor), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_label_set_text_fmt(ui_gridWatts, "%dW", gridOrMeterPower);

    // Update the battery energy
    int battWattsColor = UI_GREY;
    if (battPower != 0) { // Neither charging nor discharging
      if (toBatt) { // Charging
        battWattsColor = UI_GREEN;
        battPower = battPower * -1;
      } else { // Discharging
        battWattsColor = UI_RED;
      }
    }
    lv_obj_set_style_text_color(ui_battWatts, lv_color_hex(battWattsColor), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_label_set_text_fmt(ui_battWatts, "%dW", battPower);

    // Update the load energy
    int loadWattsColor = UI_GREY;
    if (loadOrEpsPower > 0) {  // Consuming
      loadWattsColor = UI_RED;
    }
    lv_obj_set_style_text_color(ui_loadWatts, lv_color_hex(loadWattsColor), LV_PART_MAIN | LV_STATE_DEFAULT);
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

  StaticJsonDocument<200> filter;
  filter["code"] = true;
  filter["msg"] = true;
  filter["data"]["etodayTo"] = true;
  filter["data"]["etodayFrom"] = true;

  int docSize = 192;
  DynamicJsonDocument responseJson(docSize);
  responseJson = CallSunsynkApi(apiUri, docSize, DeserializationOption::Filter(filter));

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

  StaticJsonDocument<200> filter;
  filter["code"] = true;
  filter["msg"] = true;
  filter["data"]["etodayChg"] = true;
  filter["data"]["etodayDischg"] = true;

  int docSize = 192;
  DynamicJsonDocument responseJson(docSize);
  responseJson = CallSunsynkApi(apiUri, docSize, DeserializationOption::Filter(filter));

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

void GetLoadTotal() {
  Serial.println("Getting load daily total data ...");
  char apiUri[128];
  sprintf(apiUri, "%s/inverter/load/%s/realtime?sn=%s", SUNSYNK_API_URL, SUNSYNK_INVERTER_ID, SUNSYNK_INVERTER_ID);

  StaticJsonDocument<200> filter;
  filter["code"] = true;
  filter["msg"] = true;
  filter["data"]["dailyUsed"] = true;

  int docSize = 128;
  DynamicJsonDocument responseJson(docSize);
  responseJson = CallSunsynkApi(apiUri, docSize, DeserializationOption::Filter(filter));

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
