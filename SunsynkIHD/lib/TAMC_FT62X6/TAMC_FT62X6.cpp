#include "Arduino.h"
#include <TAMC_FT62X6.h>
#include <Wire.h>

TAMC_FT62X6::TAMC_FT62X6() {
}
bool TAMC_FT62X6::begin(int thresh) {
  if (test()){
    return false;
  };
  writeByte(REG_TH_GROUP, thresh);
  return true;
}
void TAMC_FT62X6::setRotation(int rot) {
  rotation = rot;
}
void TAMC_FT62X6::read(void) {
  byte data[16];

  Wire.beginTransmission(ADDR);
  Wire.write(0);
  Wire.endTransmission();

  Wire.requestFrom(ADDR, 16);
  for (int i = 0; i < 16; i++)
    data[i] = Wire.read();

  gesture = data[REG_GEST_ID];
  if (gesture != IDLE) {
    touches = data[REG_TD_STATUS];
    isTouched = touches != 0;

    if (touches >= 1){
      byte d[6];
      for (int i=0; i<6; i++){
        d[i] = data[REG_P1_XH + i];
      }
      points[0] = TP_Point(d, rotation);
    }
    if (touches >= 2){
      byte d[6];
      for (int i=0; i<6; i++){
        d[i] = data[REG_P2_XH + i];
      }
      points[1] = TP_Point(d, rotation);
    }
  }
}

byte TAMC_FT62X6::readByte(byte reg) {
  byte x;
  Wire.beginTransmission(ADDR);
  Wire.write(reg);
  Wire.endTransmission();

  Wire.requestFrom(ADDR, 1);
  x = Wire.read();

  return x;
}

void TAMC_FT62X6::writeByte(byte reg, byte val) {
  // use i2c
  Wire.beginTransmission(ADDR);
  Wire.write(reg);
  Wire.write(val);
  Wire.endTransmission();
}

bool TAMC_FT62X6::test(void) {
  Wire.beginTransmission(ADDR);
  byte error = Wire.endTransmission();
  return error == 0;
}

TP_Point::TP_Point(void) {
  x = y = weight = area = 0;
}
TP_Point::TP_Point(long _x, long _y, int _weight, int _area) {
  x = _x;
  y = _y;
  weight = _weight;
  area = _area;
}
TP_Point::TP_Point(byte *data, int rot) {
  long temp;
  x = data[0] & 0x0F;
  x <<= 8;
  x |= data[1];
  y = data[2] & 0x0F;
  y <<= 8;
  y |= data[3];
  weight = data[4];
  area = data[5] & 0x0F;
  switch (rot){
    case ROTATION_LEFT:
      x = FT62X6_HEIGHT - x;
      y = FT62X6_WIDTH - y;
      break;
    case ROTATION_INVERTED:
      temp = x;
      x = FT62X6_WIDTH - y;
      y = temp;
      break;
    case ROTATION_RIGHT:
      x = x;
      y = y;
      break;
    case ROTATION_NORMAL:
      temp = x;
      x = y;
      y = FT62X6_HEIGHT - temp;
      break;
    default:
      break;
  }
}
bool TP_Point::operator==(TP_Point point) {
  return ((point.x == x) && (point.y == y) && (point.weight == weight) && (point.area == area));
}
bool TP_Point::operator!=(TP_Point point) {
  return ((point.x != x) || (point.y != y) || (point.weight != weight) || (point.area != area));
}
