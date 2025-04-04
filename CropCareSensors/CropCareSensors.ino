#include "TemperatureSensor.h"
#include "SoilMoistureSensor.h"
#include "PhSensor.h"
#include <LiquidCrystal_I2C.h>

// Define objects
TemperatureSensor tempSensor(23);               // Creates and object of the class TemperatureSensor and passed the 23 GPIO pin
SoilMoistureSensor soilSensor(34, 3500, 1000);  // Creates and object of the class SoilMoistureSensor and passed the 34 GPIO pin with dry and wet value
LiquidCrystal_I2C lcd(0x27, 16, 2);             // Creates and object of the lcd that is 16x2
PhSensor phSensor(35, 21.34, lcd);              // Creates and object of the class PhSensor and passed the 35 GPIO pin

void setup() {
  Serial.begin(115200);
  tempSensor.begin();  // Starts the Temperature Sensor
  phSensor.setupDisplay();
}

void loop() {
  float tempC = tempSensor.getCelsius();     // Passed the celcius value in the float from TemperatureSensor.cpp
  float tempF = (tempC * 9.0 / 5.0) + 32.0;  //  Converts the celcius into fahrenheit manually
  // Prints the Temperature and Fahrenheit Output
  Serial.print("\nTemperature: ");
  Serial.print(tempC);
  Serial.println(" °C");
  Serial.print("Fahrenheit: ");
  Serial.println(tempF);

  lcd.setCursor(10, 0);
  lcd.print(tempC, 2);

  // The condition statements for the soil Condition based on the moisture percent above
  if (moisturePercent <= 10) {
    soilCondition = "Very Dry";  // will print Very Dry if the value is 10 or lower
  } else if (moisturePercent <= 40) {
    soilCondition = "Dry Soil";  // will print Dry Soil if the value is 40 or lower
  } else if (moisturePercent <= 80) {
    soilCondition = "Moist Soil";  // will print Moist Soil if the value is 80 or lower
  } else if (moisturePercent < 100) {
    soilCondition = "Very Wet/Near Mud";  // it will print Very Wet/Near Mud if the value of the moisturePercent is lower than 100
  } else if (moisturePercent == 100) {
    soilCondition = "Submerged in Water";  // will print if the moisturePercent has reached the maximum moisture percent
  }
  Serial.print("\nSoil Moisture Value: ");
  Serial.println(moistureValue);  // Print value to Serial Monitor
  Serial.print("Soil Moisture Percentage: ");
  Serial.print(moisturePercent);  // Print the Moisture percent based on the convertion
  Serial.println(" %");
  Serial.print("Condition: ");
  Serial.println(soilSensor.getCondition());

  lcd.setCursor(0, 1);
  lcd.print("SM: ");
  lcd.setCursor(4, 1);
  lcd.print(soilSensor.getCondition());

  phSensor.readPh();

  delay(1000);
}
