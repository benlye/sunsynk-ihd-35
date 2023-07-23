#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <esp_http_client.h>

#include "Config.h"
#include "DateTime.h"
#include "SunsynkApi.h"
#include "ui.h"

struct ApiToken apiToken;

void ClearSunsynkAuthToken()
{
    apiToken.accessToken = "";
    apiToken.refreshToken = "";
    apiToken.expiresAt = 0;
    apiToken.expiresIn = 0;
}

boolean CheckSunsynkAuthToken()
{
    if (apiToken.accessToken == "" || apiToken.expiresAt < getTime()) {
        ClearSunsynkAuthToken();
        return false;
    }
    return true;
}

boolean GetSunsynkAuthToken()
{
    apiToken.accessToken = "";
    apiToken.refreshToken = "";
    apiToken.expiresIn = 0;
    apiToken.expiresAt = 0;
    boolean gotToken = false;

    Serial.println("Fetching API auth token ...");
    DynamicJsonDocument authResponseJson(384);
    WiFiClientSecure *client = new WiFiClientSecure;

    if (client)
    {
        Serial.println(SUNSYNK_LOGIN_URL);
        client->setCACertBundle(certBundle);
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
                            //Serial.printf("Token Expires: %s\n\n", getDateTimeString(expiresAt).c_str());

                            apiToken.accessToken = String(accessToken);
                            apiToken.refreshToken = String(refreshToken);
                            apiToken.expiresIn = expiresIn;
                            apiToken.expiresAt = expiresAt;

                            gotToken = true;
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
    Serial.println();
    return gotToken;
}

DynamicJsonDocument CallSunsynkApi(String uri, int size, DeserializationOption::Filter(filter))
{
    Serial.println(uri);
    DynamicJsonDocument doc(size);
    WiFiClientSecure *client = new WiFiClientSecure;
    if (client)
    {
        client->setCACertBundle(certBundle);
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
                    else if (httpCode == HTTP_CODE_UNAUTHORIZED)
                    {
                        ClearSunsynkAuthToken();
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

        /*
        Serial.println();
        Serial.printf("PV:      %6d W\n", ihdData.pvWatts);
        Serial.printf("Battery: %6d W\n", ihdData.battWatts);
        Serial.printf("Grid:    %6d W\n", ihdData.gridWatts);
        Serial.printf("Load:    %6d W\n", ihdData.loadWatts);
        Serial.printf("SOC:     %6d %%\n", ihdData.battSoc);
        Serial.println();
        */
    }
    Serial.println();
}

void GetDailyTotals()
{
    Serial.println("Getting daily total data ...");
    String month = getMonthString();
    String today = getDateString();

    char apiUri[128];
    sprintf(apiUri, "%s/plant/energy/%s/month?lan=en&date=%s&id=%s", SUNSYNK_API_URL, SUNSYNK_PLANT_ID, month, SUNSYNK_PLANT_ID);

    StaticJsonDocument<200> filter;
    filter = true;
    filter["code"] = true;
    filter["msg"] = true;
    filter["data"]["infos"][0]["label"] = true;
    filter["data"]["infos"][0]["records"][0]["time"] = true;
    filter["data"]["infos"][0]["records"][0]["value"] = true;

    int docSize = 16384;
    DynamicJsonDocument responseJson(docSize);
    responseJson = CallSunsynkApi(apiUri, docSize, DeserializationOption::Filter(filter));

    int numElements = responseJson["data"]["infos"].size();
    //Serial.println();
    for (int i = 0; i < numElements; i++)
    {
        String label = responseJson["data"]["infos"][i]["label"];
        int lastRecord = responseJson["data"]["infos"][i]["records"].size() - 1;
        String date = responseJson["data"]["infos"][i]["records"][lastRecord]["time"];
        double value = responseJson["data"]["infos"][i]["records"][lastRecord]["value"];

        if (today == date)
        {
            char valStr[8];
            dtostrf(value, 3, 1, valStr);

            /*
            String labelStr = label + ":";
            while (labelStr.length() < 10) {
                labelStr = labelStr + " ";
            }
            Serial.printf("%s % 5s kWh\n", labelStr.c_str(), valStr);
            */

            if (label == "PV")
            {
                ihdData.pvDailyTotal = value;
            }
            else if (label == "Load")
            {
                ihdData.loadDailyTotal = value;
            }
            else if (label == "Export")
            {
                ihdData.gridDailyExport = value;
            }
            else if (label == "Import")
            {
                ihdData.gridDailyImport = value;
            }
            else if (label == "Discharge")
            {
                ihdData.battDailyDischarge = value;
            }
            else if (label == "Charge")
            {
                ihdData.battDailyCharge = value;
            }
        }
    }
    Serial.println();
}

void GetIhdData()
{
    ihdDataReady = false;
    if (!CheckSunsynkAuthToken())
    {
        GetSunsynkAuthToken();
    }
    GetPlantFlow();
    GetDailyTotals();
    ihdDataReady = true;
}