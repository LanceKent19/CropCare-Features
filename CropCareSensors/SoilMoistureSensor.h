#ifndef SOIL_MOISTURE_SENSOR_H
#define SOIL_MOISTURE_SENSOR_H

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include "WifiManager.h"

class SoilMoistureSensor {
private:
  int pin;  
  int dryValue, wetValue;
  int ledPin, buzzerPin;
  LiquidCrystal_I2C &lcd;
  unsigned long dryStartTime;
  bool dryTimerStarted;
  const unsigned long dryDelay = 3000;
  bool ledState;
  WiFiManager &wifiManager;
  const char *serverURL = "http://192.168.100.42/Plant-disease-detection/update_moisture.php";

public:
  SoilMoistureSensor(int pin, int dryVal, int wetVal, int ledPin, int buzzerPin, LiquidCrystal_I2C &lcd, WiFiManager &wifiManager);
  int readValue();
  int getMoisturePercent();
  void update(bool showOnLCD);
  void forcePowerOffUpdate();
  void sendMoistToServer(int moisture);
};

#endif
