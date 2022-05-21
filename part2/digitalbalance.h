#ifndef DIGITALBALANCE_H
#define DIGITALBALANCE_H

typedef enum 
{
  e_angle,
  e_rawval,
  e_findx,
  e_findy,
  e_findxy,
  e_cintv,
  e_feedval
} Modes;

byte emptyBar[8] = {
  B11111,
  B10010,
  B10010,
  B10000,
  B00000,
  B00000,
  B00000,
  B11111,
};
byte filledBar[8] = {
  B11111,
  B11110,
  B11110,
  B11110,
  B01110,
  B01110,
  B01110,
  B11111,
};

void SetLCD(LiquidCrystal_I2C lcd, float accX, float accY);
long Find(float x, int pin, float checkThis, long timer);
void SetLedBar(byte* ledArray, float accelValue, int* currPos, int* lastPos);
int floatToIntMap(float x, float xLow, float xHigh, int outLow, int outHigh);
//Header for all the custom functions

#endif
