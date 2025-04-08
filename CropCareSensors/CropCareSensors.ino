// Button On and Off
#define LED_PIN 14
#define BUTTON_PIN 4
// LED condition 
#define LED_SOIL_PIN 25
#define SOIL_BUZZER_PIN 12 

// Add these global variables at the top
unsigned long dryStartTime = 0;
bool dryTimerStarted = false;
const unsigned long dryDelay = 3000;  // 3 seconds to confirm dryness

int ledSoilState = LOW;
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
SoilMoistureSensor soilSensor(34, 3500, 1000);  // Creates and object of the class SoilMoistureSensor and passed the 34 GPIO pin with dry and wet value
LiquidCrystal_I2C lcd(0x27, 16, 2);             // Creates and object of the lcd that is 16x2
// PhSensor phSensor(35, 21.34, lcd);              // Creates and object of the class PhSensor and passed the 35 GPIO pin
ToggleButton toggle(BUTTON_PIN, LED_PIN);
void setup() {
  Serial.begin(115200);
  lcdSetup();
  pinMode(LED_SOIL_PIN, OUTPUT);
  pinMode(SOIL_BUZZER_PIN, OUTPUT);
  toggle.begin();
  // tempSensor.begin();  /   / Starts the Temperature Sensor
  lastButtonState = digitalRead(BUTTON_PIN);
  digitalWrite(LED_SOIL_PIN, ledSoilState);
  digitalWrite(SOIL_BUZZER_PIN, ledSoilState);
}
void loop() {

  if (millis() - lastTimeButtonChangedState >= debounceDuration) {
    // read the state of the pushbutton value:
    buttonState = digitalRead(BUTTON_PIN);
    if (buttonState != lastButtonState) {
      lastTimeButtonChangedState = millis();
      lastButtonState = buttonState;
      if (buttonState == LOW) {
        if (ledState == HIGH) {
          Serial.println("Sensor Off");
          ledState = LOW;
          digitalWrite(SOIL_BUZZER_PIN, HIGH);
        } else {
          Serial.println("Sensor On");
          ledState = HIGH;
          digitalWrite(SOIL_BUZZER_PIN, HIGH);
        }
        digitalWrite(LED_PIN, ledState);
      } 
    }
  }
  // Sensor + LCD update (every 500ms but non-blocking)
  if (millis() - lastSensorUpdateTime >= sensorUpdateInterval) {
    lastSensorUpdateTime = millis();

    if (ledState == HIGH) {
      if (!lcdIsOn) {
        openSoilMoistureDisplay();
        lcdIsOn = true;
      }
      lcdSoilMoistureSensor();
    } else {
      if (lcdIsOn) {
        lcdIsOn = false;
      }
      clearSoilMoistureDisplay();
       // Force all indicators OFF just to be sure
      digitalWrite(LED_SOIL_PIN, LOW);
      digitalWrite(SOIL_BUZZER_PIN, LOW);
      // digitalWrite(BUZZER_PIN, LOW);
      ledSoilState = false;
  }
  }
}
void lcdSetup() {
  lcd.init();
  lcd.backlight();
  lcd.setCursor(4, 0);
  lcd.print("CropCare");
  lcd.setCursor(5, 1);
  lcd.print("System");
  delay(4000);
  lcd.clear();
}
void lcdSoilMoistureSensor() {
  int moisture = soilSensor.getMoisturePercent();  // read ONCE
  String condition;

  // Idle sensor detection
  if (moisture >= 30 && moisture <= 40) {
    condition = "Sensor Idle";
    ledSoilState = false;
    dryTimerStarted = false;  // Reset timer if idle
  } 
  else if (moisture < 10 || moisture < 30) {
    condition = (moisture < 10) ? "Very Dry" : "Dry";

    // Start the dry timer if not already started
    if (!dryTimerStarted) {
      dryStartTime = millis();
      dryTimerStarted = true;
    }

    // Check if dryness lasted more than dryDelay
    if (millis() - dryStartTime >= dryDelay) {
      ledSoilState = true;  // Trigger alarm only after delay
    } else {
      ledSoilState = false; // Still waiting
    }
  } 
  else if (moisture <= 60) {
    condition = "Moist";
    ledSoilState = false;
    dryTimerStarted = false;  // Reset timer
  } 
  else if (moisture <= 90) {
    condition = "Wet";
    ledSoilState = false;
    dryTimerStarted = false;
  } 
  else {
    condition = "Submerged";
    ledSoilState = true;
    dryTimerStarted = false;
  }

  // OUTPUT CONTROL
  digitalWrite(LED_SOIL_PIN, ledSoilState);
  digitalWrite(SOIL_BUZZER_PIN, ledSoilState);

  // Serial output
  Serial.print("Moisture: ");
  Serial.print(moisture);
  Serial.println(" %");

  Serial.print("Condition: ");
  Serial.println(condition);

  // LCD Output
  lcd.setCursor(0, 1);
  lcd.print("MP:");
  lcd.print(moisture);
  lcd.print("%");

  lcd.setCursor(7, 1);
  lcd.print("          ");  // clear old condition
  lcd.setCursor(7, 1);
  lcd.print(condition);
}
void clearSoilMoistureDisplay() {
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("System OFF");
  delay(1000);  // Optional: small delay before LCD fully turns off
  lcd.clear();
  lcd.noBacklight();
}
void openSoilMoistureDisplay() {
  lcd.clear();
  lcd.backlight();
  lcd.setCursor(3, 0);
  lcd.print("System On");
  delay(1000);  // Show "System On" briefly
  lcd.clear();
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
// }