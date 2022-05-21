//Source file for all the digital balance functions

#include "Arduino.h"
#include <LiquidCrystal_I2C.h>

int floatToIntMap(float x, float xLow, float xHigh, int outLow, int outHigh){
  return (x - xLow) * (outHigh - outLow) / (xHigh - xLow) + outHigh;
}

void SetLedBar(byte* ledArray, float accelValue, int* currPos, int* lastPos)
{
  *currPos = map(accelValue * 10, -11, 10, 0, 9);
  if (!(*currPos == *lastPos))
    {
      digitalWrite(ledArray[*currPos], HIGH);
      delay(5);
      digitalWrite(ledArray[*lastPos], LOW);
      *lastPos = *currPos;
    }
}

long Find(float target, int pin, float checkThis, long timer)
{
  if ((checkThis > target-3) && (checkThis < target+3))
  {
      pinMode(pin, OUTPUT);
      digitalWrite(pin, HIGH);
      delay(50);
      digitalWrite(pin, LOW);
    return (millis() - timer); 
  }
  else
  {
    digitalWrite(pin, LOW);
    return 0;
  }
}

void SetLCD(LiquidCrystal_I2C lcd, float accX, float accY)
{
    int lcdPos = map(accX * 10, -10, 10, 0, 16);
    lcd.clear();
    //Initialize bar screen
    lcd.setCursor(0,0);
    for (int i=0; i<16; i++)
    {
      lcd.write(byte(0));
    }
    lcd.setCursor(8,0);//Set cursor to center
    if (lcdPos >= 8)
    {
      for (int i=8; i<=lcdPos; i++)
      {
        lcd.write(byte(1));
      }
    }
    else
    {
      lcd.rightToLeft();
      lcd.setCursor(8,0);
      for (int i=8; i >=lcdPos; i--)
      {
        //lcd.printLeft();
        lcd.write(byte(1));
      }
    }
    lcd.leftToRight();
    lcd.setCursor(0,1);
    //lcd.print(balMode);
    lcd.print("x:");
    lcd.print(accX*90);
    lcd.print("y:");
    lcd.print(accY*90);
}
