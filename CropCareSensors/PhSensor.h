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

  // Dependencies
  LiquidCrystal_I2C& lcd;
  WiFiManager& wifiManager;
  PowerManager& powerManager;

  const char* serverURL = "http://192.168.100.42/Plant-disease-detection/update_ph.php";
  float lastPhValue = NAN;

  // Internal methods
  float readRawPh();
  void updateIndicators(float phValue);
  void updateDisplay(float phValue);
  void updateServer();

public:
  PhSensor(int pin, float caliVal, int redLed, int greenLed, int blueLed, int buzzer,
           LiquidCrystal_I2C& lcd, WiFiManager& wifiManager, PowerManager& powerManager);

  // Sensor operations
  float readPh();
  void sendPhToServer(float phValue);

void forcePowerOffUpdate() {
    if (WiFi.status() == WL_CONNECTED) {
        wifiManager.sendHTTPPost(serverURL, "ph=-");
    } else {
        // Write directly to SPIFFS or update a fallback storage, or retry later
        Serial.println("WiFi not ready - OFF state will be retried later");
    }
}


  void begin() {
    if (!powerManager.isSystemOn() && WiFi.status() == WL_CONNECTED) {
        forcePowerOffUpdate();
    }
}

};

#endif