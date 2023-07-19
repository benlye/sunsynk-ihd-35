#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

#include "Config.h"
#include "SunsynkApi.h"
#include "ui.h"

ApiToken apiToken;

boolean CheckSunsynkAuthToken()
{

    // Check if the access token is valid
    char *tokenState = "Unknown";
    bool tokenValid = false;
    if (apiToken.accessToken != "" && apiToken.expiresIn > 0)
    {
        Serial.printf("Stored Token Expiry: %s\n", getDateTimeString(apiToken.expiresAt).c_str());
        if (apiToken.expiresAt > getTime())
        {
            tokenState = "Valid";
            tokenValid = true;
        }
        else
        {
            tokenState = "Expired";
        }
    }
    else
    {
        tokenState = "Missing";
    }

    // Report Sunsynk token state
    Serial.printf("Stored Token State: %s\n", tokenState);
    Serial.println();

    return tokenValid;
}

void GetSunsynkAuthToken()
{
    Serial.println("Fetching new Sunsynk auth token ...");
    DynamicJsonDocument authResponseJson(384);
    WiFiClientSecure *client = new WiFiClientSecure;

    if (client)
    {
        client->setCACert(SUNSYNK_API_CERT);
        {
            // Add a scoping block for HTTPClient https to make sure it is destroyed before WiFiClientSecure *client is
            HTTPClient https;
            https.useHTTP10(true);
            if (https.begin(*client, SUNSYNK_LOGIN_URL))
            { // HTTPS

                // start connection and send HTTP header
                https.addHeader("Content-Type", "application/json");
                https.addHeader("Accept", "application/json");

                char authPayload[1024];
                sprintf(authPayload, "{\"username\": \"%s\", \"password\": \"%s\", \"grant_type\": \"password\", \"client_id\":\"csp_web\"}", SUNSYNK_USERNAME, SUNSYNK_PASSWORD);

                int httpCode = https.POST(authPayload);

                if (httpCode > 0)
                {
                    // HTTP header has been send and Server response header has been handled
                    Serial.printf("Response: %d\n", httpCode);

                    // file found at server
                    if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY)
                    {
                        DeserializationError error = deserializeJson(authResponseJson, https.getStream());

                        if (error)
                        {
                            Serial.print(F("deserializeJson() failed: "));
                            Serial.println(error.f_str());
                        }
                        else
                        {
                            const char *accessToken = authResponseJson["data"]["access_token"];
                            const char *refreshToken = authResponseJson["data"]["refresh_token"];
                            unsigned long expiresIn = authResponseJson["data"]["expires_in"];
                            unsigned long expiresAt = getTime() + expiresIn;
                            Serial.printf("New Token Expires: %s\n\n", getDateTimeString(expiresAt).c_str());

                            apiToken.accessToken = String(accessToken);
                            apiToken.refreshToken = String(refreshToken);
                            apiToken.expiresIn = expiresIn;
                            apiToken.expiresAt = expiresAt;
                        }
                    }
                }
                else
                {
                    Serial.printf("[HTTPS] POST... failed, error: %s\n", https.errorToString(httpCode).c_str());
                }

                https.end();
            }
            else
            {
                Serial.printf("[HTTPS] Unable to connect\n");
            }
            // End extra scoping block
        }

        client->stop();
        delete client;
    }
    else
    {
        Serial.println("Unable to create client");
    }
}

DynamicJsonDocument CallSunsynkApi(String uri, int size, DeserializationOption::Filter(filter))
{
    // Serial.println(uri);
    DynamicJsonDocument doc(size);
    WiFiClientSecure *client = new WiFiClientSecure;
    if (client)
    {
        client->setCACert(SUNSYNK_API_CERT);
        {
            // Add a scoping block for HTTPClient https to make sure it is destroyed before WiFiClientSecure *client is
            HTTPClient https;
            https.useHTTP10(true);
            if (https.begin(*client, uri))
            { // HTTPS

                // start connection and send HTTP header
                https.addHeader("Authorization", "Bearer " + apiToken.accessToken);
                https.addHeader("Content-Type", "application/json");
                https.addHeader("Accept", "application/json");

                int httpCode = https.GET();

                if (httpCode > 0)
                {
                    Serial.printf("Response: %d\n", httpCode);
                    if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY)
                    {
                        DeserializationError error = deserializeJson(doc, https.getStream(), DeserializationOption::Filter(filter));
                        if (error)
                        {
                            Serial.print(F("deserializeJson() failed: "));
                            Serial.println(error.f_str());
                        }
                    }
                }
                else
                {
                    Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
                }

                https.end();
            }
            else
            {
                Serial.printf("[HTTPS] Unable to connect\n");
            }
            // End extra scoping block
        }
        client->stop();
        delete client;
    }
    else
    {
        Serial.println("Unable to create client");
    }
    return doc;
}

void GetPlantRealtime()
{
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

    if (responseJson["code"] == 0 && responseJson["msg"] == "Success")
    {
        int pac = responseJson["data"]["pac"];
        double eTodayDbl = responseJson["data"]["etoday"];

        char eTodayStr[8];
        dtostrf(eTodayDbl, 3, 1, eTodayStr);
        ihdData.pvWatts = pac;
        ihdData.pvDailyTotal = eTodayDbl;
        
        Serial.println();
        Serial.printf("PV Now:   %6d W\n", pac);
        Serial.printf("PV Today: %6s kWh\n", eTodayStr);
        Serial.println();
    }
}

void GetPlantFlow()
{
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

    if (responseJson["code"] == 0 && responseJson["msg"] == "Success")
    {
        ihdData.pvWatts = responseJson["data"]["pvPower"];
        ihdData.battWatts = responseJson["data"]["battPower"];
        ihdData.gridWatts = responseJson["data"]["gridOrMeterPower"];
        ihdData.loadWatts =  responseJson["data"]["loadOrEpsPower"]; 
        ihdData.battSoc = responseJson["data"]["soc"];
        ihdData.toBatt = responseJson["data"]["toBat"]; 
        ihdData.toGrid = responseJson["data"]["toGrid"];

        Serial.println();
        Serial.printf("PV:      %6d W\n", ihdData.pvWatts);
        Serial.printf("Battery: %6d W\n", ihdData.battWatts);
        Serial.printf("Grid:    %6d W\n", ihdData.gridWatts);
        Serial.printf("Load:    %6d W\n", ihdData.loadWatts);
        Serial.printf("SOC:     %6d %%\n", ihdData.battSoc);
        Serial.println();
    }
}

void GetGridTotals()
{
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

    if (responseJson["code"] == 0 && responseJson["msg"] == "Success")
    {
        ihdData.gridDailyExport = responseJson["data"]["etodayTo"];
        ihdData.gridDailyImport = responseJson["data"]["etodayFrom"];
    
        double eTodayToDbl = responseJson["data"]["etodayTo"];
        char eTodayToStr[8];
        dtostrf(eTodayToDbl, 3, 1, eTodayToStr);

        double eTodayFromDbl = responseJson["data"]["etodayFrom"];
        char eTodayFromStr[8];
        dtostrf(eTodayFromDbl, 3, 1, eTodayFromStr);

        Serial.println();
        Serial.printf("Export Today: %4s kWh\n", eTodayToStr);
        Serial.printf("Import Today: %4s kWh\n", eTodayFromStr);
        Serial.println();
    }
}

void GetBatteryTotals()
{
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

    if (responseJson["code"] == 0 && responseJson["msg"] == "Success")
    {
        ihdData.battDailyCharge = responseJson["data"]["etodayChg"];
        ihdData.battDailyDischarge = responseJson["data"]["etodayDischg"];

        double eTodayChgDbl = responseJson["data"]["etodayChg"];
        char eTodayChgStr[8];
        dtostrf(eTodayChgDbl, 3, 1, eTodayChgStr);

        double eTodayDischgDbl = responseJson["data"]["etodayDischg"];
        char eTodayDischgStr[8];
        dtostrf(eTodayDischgDbl, 3, 1, eTodayDischgStr);

        Serial.println();
        Serial.printf("Charge Today:    %4s kWH\n", eTodayChgStr);
        Serial.printf("Dischange Today: %4s kWh\n", eTodayDischgStr);
        Serial.println();
    }
}

void GetLoadTotal()
{
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

    if (responseJson["code"] == 0 && responseJson["msg"] == "Success")
    {
        ihdData.loadDailyTotal = responseJson["data"]["dailyUsed"];
        
        double loadTotalDbl = responseJson["data"]["dailyUsed"];
        char eLoadTotalStr[8];
        dtostrf(loadTotalDbl, 3, 1, eLoadTotalStr);

        Serial.println();
        Serial.printf("Load Today: %4s kWh\n", eLoadTotalStr);
        Serial.println();
    }
}

void GetIhdData()
{
    ihdDataReady = false;
    GetPlantRealtime();
    GetPlantFlow();
    GetGridTotals();
    GetBatteryTotals();
    GetLoadTotal();
    //GetPlotData();
    ihdDataReady = true;
    //Serial.printf("Waiting %d seconds before next API poll ...\n\n", loopDelaySec);
}