#include <Arduino.h>
#include <Time.h>
#include <TimeLib.h>
#include "esp_sntp.h"

#include "Config.h"
#include "DateTime.h"

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
        Serial.println("Failed to obtain time");
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
        Serial.println("Failed to obtain time");
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
        Serial.println("Failed to obtain time");
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
        Serial.println("Failed to obtain time");
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
