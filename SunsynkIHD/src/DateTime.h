#ifndef __DATETIME_H
#define __DATETIME_H

#include <Arduino.h>

void configureNtpAndSetClock(void);
void timeSyncCallback(struct timeval *tv);
unsigned long getTime(void);
String getDateTimeString(void);
String getDateTimeString(unsigned long val);
String getTimeString(void);
String getDateString(void);
String getMonthString(void);
uint16_t timeToMinutes(String time);

#endif // __DATETIME_H
