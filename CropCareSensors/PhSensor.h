#ifndef PH_SENSOR_H
#define PH_SENSOR_H

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include "WifiManager.h"
#include "PowerManager.h"

class PhSensor {
private:
  // Hardware pins and calibration
  int pin;
  float caliVal;

  // Indicator pins
  int redLed;
  int greenLed;
  int blueLed;
  int buzzer;

  unsigned long lastReadTime = 0;
  const int readInterval = 10;  // ms between samples
  int sampleIndex = 0;
  int buffer_arr[10];
  bool isSampling = false;

  unsigned long buzzerStartTime = 0;
  bool buzzerActive = false;

  // Dependencies
  LiquidCrystal_I2C& lcd;
  WiFiManager& wifiManager;
  PowerManager& powerManager;

  const char* serverURL = "http://192.168.100.42/Plant-disease-detection/update_ph.php";
  float lastPhValue = NAN;

  // Internal methods
  float readRawPh();
  void sendPhToServer(float phValue);  // Handles normal data send

public:
  PhSensor(int pin, float caliVal, int redLed, int greenLed, int blueLed, int buzzer,
           LiquidCrystal_I2C& lcd, WiFiManager& wifiManager, PowerManager& powerManager);

  float readPh();  // High-level function that includes reading + sending
  void begin();    // Used at setup
  void forcePowerOffUpdate();
  void readPhNonBlocking();
};

#endif
