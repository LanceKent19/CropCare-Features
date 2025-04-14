#ifndef PH_SENSOR_H
#define PH_SENSOR_H

// Libraries with the communication of the PH Level Sensors
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include "WifiManager.h"

class PhSensor {
private:
  int pin;
  float caliVal;
  int buffer_arr[10];
  LiquidCrystal_I2C& lcd;  // Pass by reference (note the &)

  int redLed;
  int greenLed;
  int blueLed;
  int buzzer;
  WiFiManager& wifiManager;
  const char* serverURL = "http://192.168.100.42:3000/update_ph.php";
public:
  PhSensor(int pin, float caliVal, int redLed, int greenLed, int blueRed, int buzzer, LiquidCrystal_I2C& lcd, WiFiManager& wifiManager);  // Constructor also uses &
  float readPh();

  void sendPhToServer(float phSensor) {
    String body = "phSensor=" + String(phSensor, 2);
    wifiManager.sendHTTPPost(serverURL, body);
  }
};

#endif
