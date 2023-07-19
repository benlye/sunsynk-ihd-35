#ifndef __DATETIME_H
#define __DATETIME_H

#include <Arduino.h>

void setClock(void);
unsigned long getTime(void);
String getDateTimeString(void);
String getDateTimeString(unsigned long val);
String getTimeString(void);
String getDateString(void);

#endif // __DATETIME_H
