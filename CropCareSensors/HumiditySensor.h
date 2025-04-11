#ifndef HUMIDITY_SENSOR_H
#define HUMIDITY_SENSOR_H

#include <Arduino.h>
#include <Bonezegei_DHT11.h>
#include <LiquidCrystal_I2C.h>

class HumiditySensor {
private:
  int pin;
  Bonezegei_DHT11 dht;
  LiquidCrystal_I2C* lcd;

public:
  HumiditySensor(int humidityPin, LiquidCrystal_I2C& display);  // Constructor
  void begin();                     // Initialize the DHT11
  void update(bool showOnLCD = false);  // Read and optionally display humidity
};

#endif
