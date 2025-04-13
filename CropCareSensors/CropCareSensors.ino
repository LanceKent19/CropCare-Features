// SYSTEM BUTTONS AND INCLUDE
  // BUTTON ON AND OFF PINS
  #define POWER_LED_ON_PIN 14   
  #define POWER_BUTTON_PIN 4
  #define POWER_LED_OFF_PIN 26
  #define POWER_BUZZER_PIN 13

  // SOIL MOISTURE SENSOR PINS
  #define LED_SOIL_PIN 25
  #define SOIL_BUZZER_PIN 12
  #define SOIL_PIN 34

  // TEMPERATURE SENSOR PIN
  #define TEMPERATURE_PIN 23

  // PH LEVEL SENSOR PINS
  #define PH_LEVEL_PIN 36 
  #define PH_RED_LED_PIN 18
  #define PH_GREEN_LED_PIN 5
  #define PH_BLUE_LED_PIN 17
  #define PH_BUZZER_PIN 16

  // LCD PINS
  #define SCL_PINS 22
  #define SDA 21

  // HUMIDITY PINS
  #define HUMIDITY_PIN 19

  // Classess and Libraries
  #include "TemperatureSensor.h"
  #include "SoilMoistureSensor.h"
  #include "PhSensor.h"
  #include <LiquidCrystal_I2C.h>
  #include <HumiditySensor.h>

// variables will change:
  int lastButtonState;
  int buttonState;
  int ledState = LOW;
  bool lcdIsOn = false;

  unsigned long lastTimeButtonChangedState = millis();
  unsigned long lastSensorUpdateTime = 0;
  unsigned long debounceDuration = 50;
  unsigned long sensorUpdateInterval = 500;  // 500ms like your original delay

// Define objects
LiquidCrystal_I2C lcd(0x27, 16, 2);                                                 // Creates and object of the lcd that is 16x2
TemperatureSensor tempSensor(TEMPERATURE_PIN, lcd);               // Creates and object of the class TemperatureSensor and passed the 23 GPIO pin
SoilMoistureSensor soilSensor(SOIL_PIN, 3500, 1000, LED_SOIL_PIN, SOIL_BUZZER_PIN, lcd);  // Creates and object of the class SoilMoistureSensor and passed the 34 GPIO pin with dry and wet value
PhSensor phSensor(PH_LEVEL_PIN, 21.40, PH_RED_LED_PIN, PH_GREEN_LED_PIN, PH_BLUE_LED_PIN, PH_BUZZER_PIN ,lcd);              // Creates and object of the class PhSensor and passed the 35 GPIO pin
HumiditySensor humiditySensor(HUMIDITY_PIN, lcd);

void beep(int duration, int count) {
  for (int i = 0; i < count; i++) {
    digitalWrite(POWER_BUZZER_PIN, HIGH);
    delay(duration);
    digitalWrite(POWER_BUZZER_PIN, LOW);
    delay(100);  // short pause between beeps
  }
}

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
  pinMode(POWER_BUZZER_PIN,OUTPUT);

  pinMode(PH_RED_LED_PIN, OUTPUT);
  pinMode(PH_GREEN_LED_PIN, OUTPUT);
  pinMode(PH_BLUE_LED_PIN, OUTPUT);
  pinMode(PH_BUZZER_PIN, OUTPUT);

  digitalWrite(PH_RED_LED_PIN, LOW);
  digitalWrite(PH_GREEN_LED_PIN, LOW);
  digitalWrite(PH_BLUE_LED_PIN, LOW);

  pinMode(POWER_LED_ON_PIN, OUTPUT);
  pinMode(POWER_LED_OFF_PIN, OUTPUT);
  pinMode(POWER_BUTTON_PIN, INPUT_PULLUP);

  digitalWrite(POWER_LED_ON_PIN, ledState);
  digitalWrite(POWER_BUTTON_PIN, ledState);
  humiditySensor.begin();
  tempSensor.begin();  // Starts the Temperature Sensor
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
          beep(150, 2); 
          digitalWrite(POWER_LED_OFF_PIN,HIGH);
          digitalWrite(PH_RED_LED_PIN, LOW);
          digitalWrite(PH_GREEN_LED_PIN, LOW);
          digitalWrite(PH_BLUE_LED_PIN, LOW);
        } else {
          Serial.println("Sensor On");
          ledState = HIGH;
          soilSensor.powerOn();
          beep(150, 1);  
          digitalWrite(POWER_LED_OFF_PIN,LOW);
        }
        digitalWrite(POWER_LED_ON_PIN, ledState);
      }
    }
  }

  if (millis() - lastSensorUpdateTime >= sensorUpdateInterval) {
    lastSensorUpdateTime = millis();

    if (ledState == HIGH) {
      soilSensor.update(true);
      tempSensor.lcdTemperatureSensor();
      phSensor.readPh();
      humiditySensor.update(true);
    }
  }
} 