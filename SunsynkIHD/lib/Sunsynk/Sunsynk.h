#ifndef SUNSYNK_H
#define SUNSYNK_H
#include "Arduino.h"
#include <ArduinoJson.h>

typedef struct
{
    uint16_t pvWatts;
    int16_t battWatts;
    uint16_t gridWatts;
    uint16_t loadWatts;
    uint8_t battSoc;
    bool toBatt;
    bool toGrid;
} PlantFlowData_t;

typedef struct
{
    double pv = 0;
    double load = 0;
    double battCharge = 0;
    double battDischarge = 0;
    double gridExport = 0;
    double gridImport = 0;
} PlantTotals_t;

typedef struct
{
    int16_t pv[288];
    int16_t battery[288];
    int16_t grid[288];
    int16_t load[288];
    int16_t soc[288];
    int16_t count = 0;
    int16_t pri_y_max = 0;
    int16_t pri_y_min = 0;
    int16_t sec_y_max = 0;
    int16_t sec_y_min = 0;
} PlantDailyPlot_t;

class Sunsynk
{
public:
    Sunsynk();

    bool Authenticate(const char *username, const char *password);
    void ClearAuth(void);
    void SetCaCert(const char *cert);
    void SetCaCertBundle(const uint8_t *bundle);
    void SetInsecure(bool b = true);
    void SetTimeout(uint16_t timeout);
    bool CheckAccessToken(void);

    DynamicJsonDocument CallApi(String uri, int size, DeserializationOption::Filter(filter));

    void GetPlantFlow(uint32_t plant, PlantFlowData_t &data);
    void GetDailyTotals(uint32_t plant, tm date, PlantTotals_t &data);
    void GetDailyPlotData(uint32_t plant, tm date, PlantDailyPlot_t &data);

private:
    const char *_authEndpoint = "https://pv.inteless.com/oauth/token";
    const char *_apiEndoint = "https://pv.inteless.com/api/v1";

    uint16_t _apiTimeout = 10000;
    bool _insecure = false;
    String _accessToken = "";
    String _refreshToken = "";
    unsigned long _expiresIn = 0;
    unsigned long _acquiredAt = 0;
    unsigned long _expiresAt = 0;

    const uint8_t *_rootCertBundle;

    /*
     * Root certificate for pv.inteless.com
     * Friendly Name: ISRG Root X1
     * Expires: 04/06/2035
     * Serial Number: 00:82:10:CF:B0:D2:40:E3:59:44:63:E0:BB:63:82:8B:00
     */
    const char *_rootCert =
        "-----BEGIN CERTIFICATE-----\n"
        "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\n"
        "-----END CERTIFICATE-----\n";
};

#endif // SUNSYNK_H
