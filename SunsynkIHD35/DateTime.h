#ifndef __DATETIME_H
#define __DATETIME_H

#include <Arduino.h>

void setClock();
unsigned long getTime();
String getDateTimeString();
String getDateTimeString(unsigned long val);
String getTimeString();

#endif // __DATETIME_H
