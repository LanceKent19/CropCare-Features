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

// PH LEVEL SENSOR PINS;-
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
#include "HumiditySensor.h"
#include "LCDDisplay.h"
#include "BeepSound.h"
#include "WifiManager.h"
#include "PowerManager.h"

// variables will change:
int lastButtonState;
int buttonState;
int ledState = LOW;
bool lcdIsOn = false;

unsigned long lastTimeButtonChangedState = millis();
unsigned long lastSensorUpdateTime = 0;
unsigned long debounceDuration = 50;
unsigned long sensorUpdateInterval = 500;  // 500ms like your original delay

// DEFINING OBJECTS
WiFiManager wifiManager("THE MAGOLLADOS'S", "DEMO POWER");
BeepSound beepSound(POWER_BUZZER_PIN);
LCDDisplay lcdDisplay;
PowerManager powerManager(lcdDisplay.getLCD());
TemperatureSensor tempSensor(TEMPERATURE_PIN, lcdDisplay.getLCD(), wifiManager);                                                                           // Creates and object of the class TemperatureSensor and passed the 23 GPIO pin
SoilMoistureSensor soilSensor(SOIL_PIN, 3500, 1000, LED_SOIL_PIN, SOIL_BUZZER_PIN, lcdDisplay.getLCD(), wifiManager);                                      // Creates and object of the class SoilMoistureSensor and passed the 34 GPIO pin with dry and wet value
PhSensor phSensor(PH_LEVEL_PIN, 21.40, PH_RED_LED_PIN, PH_GREEN_LED_PIN, PH_BLUE_LED_PIN, PH_BUZZER_PIN, lcdDisplay.getLCD(), wifiManager, powerManager);  // Creates and object of the class PhSensor and passed the 35 GPIO pin
HumiditySensor humiditySensor(HUMIDITY_PIN, lcdDisplay.getLCD(), wifiManager);

void setup() {
  Serial.begin(115200);  // Typo in your original: 115200 is correct
  // 1. Initialize basic hardware first
  pinMode(POWER_LED_ON_PIN, OUTPUT);
  pinMode(POWER_LED_OFF_PIN, OUTPUT);
  pinMode(POWER_BUTTON_PIN, INPUT_PULLUP);

  // 2. Set initial LED states (OFF state)
  digitalWrite(POWER_LED_ON_PIN, LOW);    // Green OFF
  digitalWrite(POWER_LED_OFF_PIN, HIGH);  // Red ON

  // 3. Initialize LCD display
  lcdDisplay.begin();

  // 4. Initialize PowerManager (must be after LCD)
  powerManager.begin();  // This should show "SYSTEM OFF" on LCD

  // 5. Initialize other hardware pins
  pinMode(LED_SOIL_PIN, OUTPUT);
  pinMode(SOIL_BUZZER_PIN, OUTPUT);
  pinMode(POWER_BUZZER_PIN, OUTPUT);
  pinMode(PH_RED_LED_PIN, OUTPUT);
  pinMode(PH_GREEN_LED_PIN, OUTPUT);
  pinMode(PH_BLUE_LED_PIN, OUTPUT);
  pinMode(PH_BUZZER_PIN, OUTPUT);

  digitalWrite(POWER_LED_OFF_PIN, HIGH);
  // 6. Turn off all sensor indicators
  digitalWrite(LED_SOIL_PIN, LOW);
  digitalWrite(SOIL_BUZZER_PIN, LOW);
  digitalWrite(PH_RED_LED_PIN, LOW);
  digitalWrite(PH_GREEN_LED_PIN, LOW);
  digitalWrite(PH_BLUE_LED_PIN, LOW);
  digitalWrite(PH_BUZZER_PIN, LOW);

  // 7. Connect to WiFi (important before sensors)
  wifiManager.connect();

  // 8. Initialize sensors (AFTER WiFi is connected)
  tempSensor.begin();
  humiditySensor.begin();

  phSensor.begin();  // CALL forcePowerOffUpdate() *inside* this method
  humiditySensor.forcePowerOffUpdate();
  tempSensor.forcePowerOffUpdate();
  soilSensor.forcePowerOffUpdate();
  // 9. Initialize button state
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
          lcdDisplay.showPowerStatus(false);

          powerManager.powerOffSystem();
          beepSound.beepBuzzer(150, 2);
          digitalWrite(POWER_LED_OFF_PIN, HIGH);
          digitalWrite(PH_RED_LED_PIN, LOW);
          digitalWrite(PH_GREEN_LED_PIN, LOW);
          digitalWrite(PH_BLUE_LED_PIN, LOW);
          // Force all sensors to update their off state
          phSensor.forcePowerOffUpdate();
        } else {
          Serial.println("Sensor On");
          ledState = HIGH;
          soilSensor.powerOn();
          lcdDisplay.showPowerStatus(true);

          powerManager.powerOnSystem();

          beepSound.beepBuzzer(150, 1);
          digitalWrite(POWER_LED_OFF_PIN, LOW);
        }
        digitalWrite(POWER_LED_ON_PIN, ledState);
         delay(300); // Debounce delay
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