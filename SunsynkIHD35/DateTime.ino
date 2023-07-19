#include <Time.h>
#include <TimeLib.h>

#include "Config.h"
#include "DateTime.h"

// Use NTP to set the clock
void setClock()
{
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
        return "00:00";
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
