#include <ArduinoJson.h>
#include <HTTPClient.h>
#include "Sunsynk.h"

Sunsynk::Sunsynk()
{
}

// Set a CA root certificate for the API calls.
void Sunsynk::SetCaCert(const char *cert)
{
    _rootCert = cert;
}

// Set a CA root certificate bundle for the API calls.
void Sunsynk::SetCaCertBundle(const uint8_t *bundle)
{
    _rootCertBundle = bundle;
}

// Disable validation of the API endpoint's SSL certificate.
void Sunsynk::SetInsecure(bool b)
{
    _insecure = b;
}

// Set the timeout for API calls.
void Sunsynk::SetTimeout(uint16_t t)
{
    _apiTimeout = t;
}

// Authenticate with the API and store an access token
bool Sunsynk::Authenticate(const char *username, const char *password)
{
    ClearAuth();
    Serial.println("Fetching API auth token ...");
    DynamicJsonDocument authResponseJson(384);
    WiFiClientSecure *client = new WiFiClientSecure;

    if (client)
    {
        Serial.println(_authEndpoint);

        if (_insecure)
        {
            Serial.println("WARNING: Not validating API endpoint certificate!");
            client->setInsecure();
        }
        else
        {
            if (_rootCertBundle != NULL)
            {
                client->setCACertBundle(_rootCertBundle);
            }
            else
            {
                client->setCACert(_rootCert);
            }
        }

        {
            // Add a scoping block for HTTPClient https to make sure it is destroyed before WiFiClientSecure *client is
            HTTPClient https;
            https.useHTTP10(true);
            https.setTimeout(_apiTimeout);
            if (https.begin(*client, _authEndpoint))
            { // HTTPS

                // start connection and send HTTP header
                https.addHeader("Content-Type", "application/json");
                https.addHeader("Accept", "application/json");

                char authPayload[1024];
                sprintf(authPayload, "{\"username\": \"%s\", \"password\": \"%s\", \"grant_type\": \"password\", \"client_id\":\"csp_web\"}", username, password);

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

                            _accessToken = accessToken;
                            _refreshToken = refreshToken;
                            _acquiredAt = millis() / 1000;
                            _expiresIn = expiresIn;
                            _expiresAt = _expiresIn + _acquiredAt;
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
    return (_accessToken != "" && _refreshToken != "" && _acquiredAt != 0 && _expiresIn != 0 && _expiresAt != 0);
}

// Check that the stored access token is still valid based on its expiry, discard it if not.
bool Sunsynk::CheckAccessToken()
{
    unsigned long now = millis() / 1000;
    if (now < _acquiredAt)
    {
        return false;
    }
    else
    {
        return (_accessToken != "" && _refreshToken != "" && _acquiredAt != 0 && _expiresIn != 0 && _expiresAt != 0 && _expiresAt > now);
    }
}

// Clear the stored authentication token.
void Sunsynk::ClearAuth()
{
    _accessToken = "";
    _refreshToken = "";
    _acquiredAt = 0;
    _expiresIn = 0;
    _expiresAt = 0;
}

// Call the API and return a JSON document containing the response.
DynamicJsonDocument Sunsynk::CallApi(String uri, int size, DeserializationOption::Filter(filter))
{
    Serial.println(uri);
    DynamicJsonDocument doc(size);
    WiFiClientSecure *client = new WiFiClientSecure;
    if (client)
    {
        if (_insecure)
        {
            Serial.println("WARNING: Not validating API endpoint certificate!");
            client->setInsecure();
        }
        else
        {
            if (_rootCertBundle != NULL)
            {
                client->setCACertBundle(_rootCertBundle);
            }
            else
            {
                client->setCACert(_rootCert);
            }
        }

        {
            // Add a scoping block for HTTPClient https to make sure it is destroyed before WiFiClientSecure *client is
            HTTPClient https;
            https.useHTTP10(true);
            https.setTimeout(_apiTimeout);
            if (https.begin(*client, uri))
            { // HTTPS
                // start connection and send HTTP header
                https.addHeader("Authorization", "Bearer " + _accessToken);
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
                        ClearAuth();
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

// Get the plant's realtime flow data.
void Sunsynk::GetPlantFlow(uint32_t plant, PlantFlowData_t &data)
{
    Serial.println("Getting plant flow data ...");
    char apiUri[128];
    sprintf(apiUri, "%s/plant/energy/%d/flow", _apiEndoint, plant);

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
    responseJson = CallApi(apiUri, docSize, DeserializationOption::Filter(filter));

    if (responseJson["code"] == 0 && responseJson["msg"] == "Success")
    {
        data.pvWatts = responseJson["data"]["pvPower"];
        data.battWatts = responseJson["data"]["battPower"];
        data.gridWatts = responseJson["data"]["gridOrMeterPower"];
        data.loadWatts = responseJson["data"]["loadOrEpsPower"];
        data.battSoc = responseJson["data"]["soc"];
        data.toBatt = responseJson["data"]["toBat"];
        data.toGrid = responseJson["data"]["toGrid"];
    }
    Serial.println();
}

// Get the daily totals for a given date.
void Sunsynk::GetDailyTotals(uint32_t plant, tm date, PlantTotals_t &data)
{
    Serial.println("Getting daily total data ...");

    char month_s[20];
    char date_s[20];

    sprintf(month_s, "%d-%02d", date.tm_year + 1900, date.tm_mon + 1);
    sprintf(date_s, "%d-%02d-%02d", date.tm_year + 1900, date.tm_mon + 1, date.tm_mday);

    char apiUri[128];
    sprintf(apiUri, "%s/plant/energy/%d/month?lan=en&date=%s&id=%d", _apiEndoint, plant, month_s, plant);

    StaticJsonDocument<200> filter;
    filter = true;
    filter["code"] = true;
    filter["msg"] = true;
    filter["data"]["infos"][0]["label"] = true;
    filter["data"]["infos"][0]["records"][0]["time"] = true;
    filter["data"]["infos"][0]["records"][0]["value"] = true;

    int docSize = 16384;
    DynamicJsonDocument responseJson(docSize);
    responseJson = CallApi(apiUri, docSize, DeserializationOption::Filter(filter));

    int numElements = responseJson["data"]["infos"].size();
    for (int i = 0; i < numElements; i++)
    {
        String label = responseJson["data"]["infos"][i]["label"];
        int lastRecord = responseJson["data"]["infos"][i]["records"].size() - 1;
        String recordDate = responseJson["data"]["infos"][i]["records"][lastRecord]["time"];
        double value = responseJson["data"]["infos"][i]["records"][lastRecord]["value"];

        if (String(date_s) == recordDate)
        {
            char valStr[8];
            dtostrf(value, 3, 1, valStr);

            if (label == "PV")
            {
                data.pv = value;
            }
            else if (label == "Load")
            {
                data.load = value;
            }
            else if (label == "Export")
            {
                data.gridExport = value;
            }
            else if (label == "Import")
            {
                data.gridImport = value;
            }
            else if (label == "Discharge")
            {
                data.battDischarge = value;
            }
            else if (label == "Charge")
            {
                data.battCharge = value;
            }
        }
    }
    Serial.println();
}

// Get the daily plot data for a given date.
void Sunsynk::GetDailyPlotData(uint32_t plant, tm date, PlantDailyPlot_t &data)
{
    Serial.println("Getting daily plot data ...");

    char date_s[20];
    sprintf(date_s, "%d-%02d-%02d", date.tm_year + 1900, date.tm_mon + 1, date.tm_mday);

    char apiUri[128];
    sprintf(apiUri, "%s/plant/energy/%s/day?lan=en&date=%s&id=%s", _apiEndoint, plant, date_s, plant);

    StaticJsonDocument<200> filter;
    filter["code"] = true;
    filter["msg"] = true;
    filter["data"]["infos"][0]["unit"] = true;
    filter["data"]["infos"][0]["label"] = true;
    filter["data"]["infos"][0]["records"][0]["time"] = true;
    filter["data"]["infos"][0]["records"][0]["value"] = true;

    int docSize = 98304;
    DynamicJsonDocument responseJson(docSize);
    responseJson = CallApi(apiUri, docSize, DeserializationOption::Filter(filter));

    if (responseJson["code"] == 0 && responseJson["msg"] == "Success")
    {
        // do something to parse the response
    }
}
