#ifndef TEMPERATURE_SENSOR_H  // include guards to avoid duplicate definitions
#define TEMPERATURE_SENSOR_H

#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal_I2C.h>
class TemperatureSensor {  // creates class named TemperatureSensor
private:                   // private objects so that we can only use it internally
  int pin;
  OneWire oneWire;
  DallasTemperature sensors;
  LiquidCrystal_I2C &lcd;

public:                        // public objects so that we can call it outside the class
  TemperatureSensor(int pin, LiquidCrystal_I2C &lcd);  // A constructor to pass the GPIO pin came from the user or initialized it from the main
  void begin();                // Initialize the temperature sensor
  float getCelsius();          // Getter for returning the value celcius
  float getFahrenheit();       // Getter for returning the value fahrenheit
  void lcdTemperatureSensor();
};

#endif
