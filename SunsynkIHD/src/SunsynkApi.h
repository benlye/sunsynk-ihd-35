#ifndef __SUNSYNKAPI_H
#define __SUNSYNKAPI_H

// Stores an API authentication token.
struct ApiToken
{
    String accessToken = "";
    String refreshToken = "";
    unsigned long expiresIn = 0;
    unsigned long expiresAt = 0;
};

// Stores the plant data retrieved by the API calls.
struct IhdData {
  uint16_t pvWatts = 0;
  double pvDailyTotal = 0;
  int16_t battWatts = 0;
  uint16_t battSoc = 0;
  double battDailyCharge = 0.0;
  double battDailyDischarge = 0.0;
  uint16_t gridWatts = 0;
  double gridDailyExport = 0;
  double gridDailyImport = 0;
  uint16_t loadWatts = 0;
  double loadDailyTotal = 0;
  bool toBatt = false;
  bool toGrid = false;
  String time = "00:00";
};
extern IhdData ihdData;

// Root CA certificate bundle.
extern const uint8_t certBundle[] asm("_binary_data_cert_x509_crt_bundle_bin_start");

// Indicates if all the data has been collected from the API and is ready for the UI refresh.
extern boolean ihdDataReady;

// Oauth token API endpoint URL.
#define SUNSYNK_LOGIN_URL "https://pv.inteless.com/oauth/token"

// API endpoint URL.
#define SUNSYNK_API_URL "https://pv.inteless.com/api/v1"

// Wrapper around the individual API calls.
void GetIhdData(void);

// Clears the stored API token.
void ClearSunsynkAuthToken(void);

// Checks the validity of a stored API token.
boolean CheckSunsynkAuthToken(void);

// Connects to the API oauth endpoint and retrieves and stores an authentication token.
boolean GetSunsynkAuthToken(void);

// Gets the realtime energy flow for the plant.
void GetPlantFlow(void);

// Gets the daily totals for the plant.
void GetDailyTotals(void);

#endif // __SUNSYNKAPI_H
