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
  const char* serverURL = "http://192.168.100.42:3000/update_humidity.php";

public:
  HumiditySensor(int humidityPin, LiquidCrystal_I2C& display, WiFiManager& wifiManager);  // Constructor
  void begin();                                                                           // Initialize the DHT11
  void update(bool showOnLCD = false);                                                    // Read and optionally display humidity

  void sendHumidityToServer(int humidity) {
  String body = "humiditySensor=" + String(humidity);
  wifiManager.sendHTTPPost(serverURL, body);
}

};

#endif
