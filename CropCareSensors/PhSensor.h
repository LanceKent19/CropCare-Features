#ifndef PH_SENSOR_H
#define PH_SENSOR_H

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include "WifiManager.h"

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
    
    // Display and communication
    LiquidCrystal_I2C& lcd;
    WiFiManager& wifiManager;
    const char* serverURL = "http://192.168.100.42:3000/update_ph.php";
    
    // State management
    bool isPoweredOn = true;
    float lastPhValue = NAN;
    
    // Internal methods
    float readRawPh();
    void updateIndicators(float phValue);
    void updateDisplay(float phValue);
    void updateServer();

public:
    // Constructor
    PhSensor(int pin, float caliVal, int redLed, int greenLed, int blueLed, int buzzer, 
            LiquidCrystal_I2C& lcd, WiFiManager& wifiManager);

    // Power management
    void powerOn();
    void powerOff();
    
    // Sensor operations
    float readPh();
    void sendPhToServer(float phValue);
    
    // State checking
    bool isOn() const { return isPoweredOn; }
    float getLastReading() const { return lastPhValue; }
};

#endif