//I2C logic thanks to freenove, ...
#include "WiFi.h"
#include "PubSubClient.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <MPU6050_tockn.h>
#include "digitalbalance.h"
#include <stdlib.h>


//PINS Generic
byte ledPins[] = {15, 2, 0, 4, 5, 18, 19, 21, 22, 23};
int buzzerPin = 26;

//LED Bar variables
int numLeds;
int currLedPos = 5;
int lastLedPos = 0;

//PINS I2C
#define SDA 13
#define SCL 14

//Wifi Login settings
const char* SSID = "*********";
const char* PASSWORD = "**********";

//MQTT Broker Connection settings 
const char* mqtt_server = "192.168.1.128";
const int mqtt_port = 1883;

//Connection variables
WiFiClient wifiClient;
PubSubClient client(wifiClient);

//Accelerometer variables
MPU6050 mpu6050(Wire);
float ax,ay,az; //Acceleration values
float gx,gy,gz; //Gyro values
long timeSinceUpdate = 0; //Check values for update
int accDelayInterval = 250; //in ms
int calX = -3.62;
int calY = 1.15;
int calZ = 0.02;

//LCD variables
LiquidCrystal_I2C lcd(0x27,16,2);

//Digibalance mode
int digiBalMode = 0;

void setup()
{
  //Start serial connection
  Serial.begin(115200);
  Wire.begin(SDA, SCL); //Initialize i2c
  mpu6050.begin();      //Initialize accelerometer
  

  //LCD setup
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, emptyBar);
  lcd.createChar(1, filledBar);
  lcd.setCursor(0,0);
  lcd.print("Don't move me.");
  lcd.setCursor(0,1);
  lcd.print("I'm calibrating");
  
  //Calibrate accelerometer
  mpu6050.calcGyroOffsets(true);
  mpu6050.setGyroOffsets(calX, calY, calZ); //Calibration
  
  //Pin states
  numLeds = sizeof(ledPins);
  for (int i=0; i<numLeds; i++)
  {
    pinMode(ledPins[i], OUTPUT);
  }
  pinMode(buzzerPin,OUTPUT);
  
  SetupWifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  
}

void loop()
{
  if (millis() - timeSinceUpdate > accDelayInterval)
  {
    mpu6050.update();
    ay=mpu6050.getAccY();
    ax=mpu6050.getAccX();
    SetLedBar(ledPins, ay, &currLedPos, &lastLedPos);
    SetLCD(lcd, ax, ay);
    
    timeSinceUpdate = millis(); 
  }

  if (digiBalMode == e_feedval)
  {
    int tempax = 90 * ax; //Truncate decimal
    int tempay = 90 * ay;
    char varOut[5];
    dtostrf(tempax, 2, 1, varOut);
    client.publish("digiBalance/feed", varOut);
    dtostrf(tempay, 2, 1, varOut);
    client.publish("digiBalance/feed", varOut);
    delay(accDelayInterval);
  }
  
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();
}

void SetupWifi()
{
  delay(10);
  Serial.print("\nConnecting to : ");
  Serial.println(SSID);
  //Attempt to start Wifi connetion
  WiFi.begin(SSID, PASSWORD);
  
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nWifi connected!");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* message, unsigned int length)
{
  Serial.print("Message arrived from topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageBuffer;
  
  //Iterate through pointer storing into buffer
  for (int i=0; i < length; i++)
  {
    Serial.print((char)message[i]);
    messageBuffer += (char)message[i];
  }
  Serial.println();
  
  //Check for messages from Led_Signal
  if (String(topic) == "digiBalance/setMode") 
  {
    Serial.print("Mode is now set to ");
    if (messageBuffer == "angle") 
    {
      Serial.println("angle.");
      client.publish("digiBalance/output", "Set to mode: angle");
      digiBalMode = e_angle;
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Mode: angle");
      delay(1500);
    }
    else if (messageBuffer == "rawval")
    {
      //Unimplemented
      Serial.println("rawval.");
      digiBalMode = e_rawval;
    }
    else if (messageBuffer == "feedval")
    {
      Serial.println("feedval");
      digiBalMode = e_feedval;
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Mode: feedval");
      delay(1500);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Expect");
      lcd.setCursor(0,1);
      lcd.print("screen lag");
      delay(1500);
    }
    
    //Split command into parts
    String arguments = "";
    arguments = arguments + messageBuffer;
    arguments.remove(0,5);//remove find from search
    messageBuffer = messageBuffer.substring(0,5);//Get just the command
    arguments.trim(); //Clear any excess whitespace
    float arg1 = arguments.toFloat();
    
    if (messageBuffer == "findx")
    {
      float argX = arg1;
      digiBalMode = e_findx;
      Serial.println("findx.");
      Serial.print("Set to find x-angle ");
      Serial.print("x = ");
      Serial.print(argX);
      Serial.println(" (deg)");
      bool found = 0;
      long timeAtCall = millis();
      long timeLocked = 0; //Time spent using buzzer
      int timeLockedO;
      MPU6050 mpu(Wire);
      Wire.begin(SDA, SCL);
      mpu.begin();      //Initialize accelerometer

      while (found == 0)
      {
        //Maintain ledbar and screen
        mpu.update();
        ax = mpu.getAccX();
        ay = mpu.getAccY();
        float aZ = mpu.getAccZ();
        SetLedBar(ledPins, ay, &currLedPos, &lastLedPos);
        delay(125);
        SetLCD(lcd, ax, ay);
        delay(125);
        if (timeLocked > 500)
        {

          char varOut[5];
          dtostrf(argX, 4, 2, varOut);
          //publish must be const char[] or byte[]
          client.publish("digiBalance/output", "Found X!");
          client.publish("digiBalance/output", varOut);
          found = 1;
        }
        //After 20 seconds, quit
        else if (millis() - timeAtCall > 20000)
        {
          client.publish("digiBalance/output", "Failed to find in time");
          found = 1;
        }
        //Record time on target
        timeLockedO = Find(argX, buzzerPin, ax*90, millis());
        timeLocked = timeLocked + timeLockedO;
      }
      mpu6050.begin();
      digiBalMode = e_angle;
    }

    else if (messageBuffer == "findy")
    {
      float argY = arg1;
      digiBalMode = e_findy;
      Serial.println("findy.");
      Serial.print("Set to find y-angle ");
      Serial.print("y = ");
      Serial.print(argY);
      Serial.println(" (deg)");
      bool found = 0;
      long timeAtCall = millis();
      long timeLocked = 0; //Time spent using buzzer
      int timeLockedO;
      MPU6050 mpu(Wire);
      Wire.begin(SDA, SCL);
      mpu.begin();      //Initialize accelerometer
      while (found == 0)
      {
        //Maintain ledbar and screen
        mpu.update();
        ax = mpu.getAccX();
        ay = mpu.getAccY();
        float aZ = mpu.getAccZ();
        SetLedBar(ledPins, ay, &currLedPos, &lastLedPos);
        delay(125);
        SetLCD(lcd, ax, ay);
        delay(125);
        if (timeLocked > 500)
        {
          char varOut[5];
          dtostrf(argY, 4, 2, varOut);
          //publish must be const char[] or byte[]
          client.publish("digiBalance/output", "Found Y!");
          client.publish("digiBalance/output", varOut);
          found = 1;
        }
        //After 20 seconds, quit
        else if (millis() - timeAtCall > 20000)
        {
          client.publish("digiBalance/output", "Failed to find in time");
          found = 1;
        }
        //Record time on target
        timeLockedO = Find(argY, buzzerPin, ay*90, millis());
        timeLocked = timeLocked + timeLockedO;
      }
      //Do some more stuff like write to linux or something
      mpu6050.begin();
      digiBalMode = e_angle;
    }

    else if (messageBuffer == "cintv")
    {
      Serial.println("cintv.");
      //digiBalMode = e_cintv;
      if (arg1 > 60000)
      {
        client.publish("digiBalance/output", "Interval cannot exceed 60 seconds");
      }
      else if (arg1 < 50)
      {
        client.publish("digiBalance/output", "Interval cannot be less than 50ms");
      }
      else
      {
        accDelayInterval = arg1;
        char varOut[5];
        dtostrf(arg1, 4, 2, varOut);
        client.publish("digiBalance/output", "Interval updated to:");
        client.publish("digiBalance/output", varOut);
      }
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Interval changed");
      delay(1500);
    }
  }
}

void reconnect()
{
  //Try to reestablish connection
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP8266Client"))
    {
      Serial.println("connected!");
      //Subscribe
      client.publish("esp32/output", "Hello!");
      client.subscribe("digiBalance/setMode");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" trying again in 5 seconds");
      delay(5000);
    }
  }
}
