// BUTTON ON AND OFF PINS
#define POWER_LED_PIN 14
#define POWER_BUTTON_PIN 4
#define POWER_BUZZER_PIN 13

// SOIL MOISTURE SENSOR
#define LED_SOIL_PIN 25
#define SOIL_BUZZER_PIN 12
// SOIL_PIN 34

// variables will change:
int lastButtonState;
int buttonState;
int ledState = LOW;
bool lcdIsOn = false;

unsigned long lastTimeButtonChangedState = millis();
unsigned long lastSensorUpdateTime = 0;
unsigned long debounceDuration = 50;
unsigned long sensorUpdateInterval = 500;  // 500ms like your original delay

// Classess and Libraries
// #include "TemperatureSensor.h"
#include "SoilMoistureSensor.h"
// #include "PhSensor.h"
#include <LiquidCrystal_I2C.h>
#include "ToggleButton.h"
// Define objects
// TemperatureSensor tempSensor(23);               // Creates and object of the class TemperatureSensor and passed the 23 GPIO pin
LiquidCrystal_I2C lcd(0x27, 16, 2);                                                 // Creates and object of the lcd that is 16x2
SoilMoistureSensor soilSensor(34, 3500, 1000, LED_SOIL_PIN, SOIL_BUZZER_PIN, lcd);  // Creates and object of the class SoilMoistureSensor and passed the 34 GPIO pin with dry and wet value
// PhSensor phSensor(35, 21.34, lcd);              // Creates and object of the class PhSensor and passed the 35 GPIO pin
ToggleButton toggle(POWER_BUTTON_PIN, POWER_LED_PIN);
void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("CropCare System");
  lcd.setCursor(0, 1);
  lcd.print("Setup...");
  delay(2000);  // Show for 2 seconds
  lcd.clear();
  lcd.noBacklight();

  pinMode(LED_SOIL_PIN, OUTPUT);
  pinMode(SOIL_BUZZER_PIN, OUTPUT);
  pinMode(POWER_BUZZER_PIN, OUTPUT);
  toggle.begin();
  // tempSensor.begin();  /   / Starts the Temperature Sensor
  lastButtonState = digitalRead(POWER_BUTTON_PIN);
}

void loop() {
  if (millis() - lastTimeButtonChangedState >= debounceDuration) {
    buttonState = digitalRead(POWER_BUTTON_PIN);
    if (buttonState != lastButtonState) {
      lastTimeButtonChangedState = millis();
      lastButtonState = buttonState;

      if (buttonState == LOW) {
        if (ledState == HIGH) {
          Serial.println("Sensor Off");
          ledState = LOW;
          soilSensor.powerOff();
          soilSensor.beepPowerBuzzer(150, 2);  // or beepPowerBuzzer(150, 2) if you want 2 beeps
        } else {
          Serial.println("Sensor On");
          ledState = HIGH;
          soilSensor.powerOn();
          soilSensor.beepPowerBuzzer(150, 1);  // or beepPowerBuzzer(150, 2) if you want 2 beeps
        }
        digitalWrite(POWER_LED_PIN, ledState);
      }
    }
  }

  if (millis() - lastSensorUpdateTime >= sensorUpdateInterval) {
    lastSensorUpdateTime = millis();

    if (ledState == HIGH) {
      soilSensor.update(true);
    }
  }
}

// void lcdTemperatureSensor(){
//   float tempC = tempSensor.getCelsius();     // Passed the celcius value in the float from TemperatureSensor.cpp
//   float tempF = (tempC * 9.0 / 5.0) + 32.0;  //  Converts the celcius into fahrenheit manually
//   // Prints the Temperature and Fahrenheit Output
//   Serial.print("\nTemperature: ");
//   Serial.print(tempC);
//   Serial.println(" °C");
//   Serial.print("Fahrenheit: ");
//   Serial.println(tempF);

//   lcd.setCursor(9, 0);
//   lcd.print(tempC, 2);
//   lcd.write(223); //This prints the  ° symbol
//   lcd.print("C");
// } }