#ifndef HUMIDITY_SENSOR_H
#define HUMIDITY_SENSOR_H

#include <Arduino.h>
#include <Bonezegei_DHT11.h>
#include <LiquidCrystal_I2C.h>
#include "WifiManager.h"

class HumiditySensor {
private:
  int pin;
  Bonezegei_DHT11 dht;
  LiquidCrystal_I2C* lcd;
  WiFiManager& wifiManager;
  int lastHumidity = NAN; // Add this to store last value

public:
  HumiditySensor(int humidityPin, LiquidCrystal_I2C& display, WiFiManager& wifiManager);
  void begin();
  int getLastValue() const { return lastHumidity; } // Add this method
  void update();
  void forcePowerOffUpdate();  // Add this to support OFF updates
};

#endif
