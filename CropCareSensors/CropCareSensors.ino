// SYSTEM BUTTONS AND INCLUDE
#define POWER_LED_ON_PIN 14
#define POWER_BUTTON_PIN 4
#define POWER_LED_OFF_PIN 26
#define POWER_BUZZER_PIN 13

#define LED_SOIL_PIN 25
#define SOIL_BUZZER_PIN 12
#define SOIL_PIN 34

#define TEMPERATURE_PIN 23
#define PH_LEVEL_PIN 36
#define PH_RED_LED_PIN 18
#define PH_GREEN_LED_PIN 5
#define PH_BLUE_LED_PIN 17
#define PH_BUZZER_PIN 16

#define SCL_PINS 22
#define SDA 21

#define HUMIDITY_PIN 19

#define BATTERY_TOGGLE_PIN 33

#include "TemperatureSensor.h"
#include "SoilMoistureSensor.h"
#include "PhSensor.h"
#include "HumiditySensor.h"
#include "LCDDisplay.h"
#include "BeepSound.h"
#include "WifiManager.h"
#include "PowerManager.h"

// Classes
BeepSound beepSound(POWER_BUZZER_PIN);
WiFiManager wifiManager("THE MAGOLLADOS'S", "DEMO POWER");
LCDDisplay lcdDisplay;
PowerManager powerManager(lcdDisplay.getLCD());
TemperatureSensor tempSensor(TEMPERATURE_PIN, lcdDisplay.getLCD(), wifiManager);
SoilMoistureSensor soilSensor(SOIL_PIN, 3500, 1000, LED_SOIL_PIN, SOIL_BUZZER_PIN, lcdDisplay.getLCD(), wifiManager);
PhSensor phSensor(PH_LEVEL_PIN, 21.40, PH_RED_LED_PIN, PH_GREEN_LED_PIN, PH_BLUE_LED_PIN, PH_BUZZER_PIN, lcdDisplay.getLCD(), wifiManager, powerManager);
HumiditySensor humiditySensor(HUMIDITY_PIN, lcdDisplay.getLCD(), wifiManager);

// --- BUTTON VARIABLES (your original logic)
const int buttonPin = POWER_BUTTON_PIN;
int buttonStatePrevious = LOW;
bool buttonStateLongPress = false;
unsigned long minButtonLongPressDuration = 3000;
unsigned long buttonLongPressMillis;
unsigned long buttonPressDuration;
unsigned long previousButtonMillis;
unsigned long currentMillis;
const int intervalButton = 50;
bool isOn = false;

// --- SENSORS
unsigned long lastSensorUpdateTime = 0;
unsigned long sensorUpdateInterval = 500;
unsigned long lastHumidityReadTime = 0;
const unsigned long humidityInterval = 3000;

// --- Button variables for Battery Toggle Button (BATTERY_TOGGLE_PIN)
const int batteryButtonPin = BATTERY_TOGGLE_PIN;
int batteryButtonStatePrevious = HIGH;  // With INPUT_PULLUP, default is HIGH
unsigned long batteryPreviousMillis = 0;
const int batteryDebounceDelay = 50;
bool batteryMode = false;  // false = show sensors, true = show battery percentage

void setup() {
  Serial.begin(115200);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(batteryButtonPin, INPUT_PULLUP);

  pinMode(POWER_LED_ON_PIN, OUTPUT);
  pinMode(POWER_LED_OFF_PIN, OUTPUT);
  pinMode(POWER_BUZZER_PIN, OUTPUT);
  pinMode(LED_SOIL_PIN, OUTPUT);
  pinMode(SOIL_BUZZER_PIN, OUTPUT);
  pinMode(PH_RED_LED_PIN, OUTPUT);
  pinMode(PH_GREEN_LED_PIN, OUTPUT);
  pinMode(PH_BLUE_LED_PIN, OUTPUT);
  pinMode(PH_BUZZER_PIN, OUTPUT);

  digitalWrite(POWER_LED_ON_PIN, LOW);
  digitalWrite(POWER_LED_OFF_PIN, HIGH);

  digitalWrite(LED_SOIL_PIN, LOW);
  digitalWrite(SOIL_BUZZER_PIN, LOW);
  digitalWrite(PH_RED_LED_PIN, LOW);
  digitalWrite(PH_GREEN_LED_PIN, LOW);
  digitalWrite(PH_BLUE_LED_PIN, LOW);
  digitalWrite(PH_BUZZER_PIN, LOW);

  lcdDisplay.begin();
  powerManager.begin();
  lcdDisplay.getLCDPointer()->backlight();  // Ensure backlight is on
  wifiManager.setLCD(lcdDisplay.getLCDPointer());
  wifiManager.setBeepSound(&beepSound);
  wifiManager.connect(6000);

  tempSensor.begin();
  humiditySensor.begin();

  humiditySensor.forcePowerOffUpdate();
  tempSensor.forcePowerOffUpdate();
  soilSensor.forcePowerOffUpdate();
  // Initialize button states
  buttonStatePrevious = digitalRead(buttonPin);
  batteryButtonStatePrevious = digitalRead(batteryButtonPin);
}

void readPowerButtonState() {
  currentMillis = millis();

  if (currentMillis - previousButtonMillis > intervalButton) {
    int buttonState = digitalRead(buttonPin);

    // Press start
    if (buttonState == HIGH && buttonStatePrevious == LOW && !buttonStateLongPress) {
      buttonLongPressMillis = currentMillis;
      buttonStatePrevious = HIGH;
    }

    // Holding check
    buttonPressDuration = currentMillis - buttonLongPressMillis;
    if (buttonState == HIGH && !buttonStateLongPress && buttonPressDuration >= minButtonLongPressDuration) {
      buttonStateLongPress = true;
      lcdDisplay.getLCDPointer()->clear();
      lcdDisplay.getLCDPointer()->setCursor(3, 0);  // Centered
      lcdDisplay.getLCDPointer()->print("System");
      lcdDisplay.getLCDPointer()->setCursor(2, 1);
      lcdDisplay.getLCDPointer()->print("Restarting...");
      lcdDisplay.getLCDPointer()->backlight();

      beepSound.beepBuzzer(100, 3);
      delay(500);
      ESP.restart();
    }

    // Release check
    if (buttonState == LOW && buttonStatePrevious == HIGH) {
      buttonStatePrevious = LOW;

      if (buttonPressDuration < minButtonLongPressDuration) {
        isOn = !isOn;

        if (isOn) {
          Serial.println("Sensor ON");
          soilSensor.powerOn();
          lcdDisplay.showPowerStatus(true);
          powerManager.powerOnSystem();
          // No need to display "Connecting..." here anymore
          lcdDisplay.printTemporaryMessage(
            wifiManager.isConnected() ? "WiFi: Connected" : "WiFi: Offline",
            1500);
          digitalWrite(POWER_LED_ON_PIN, HIGH);
          digitalWrite(POWER_LED_OFF_PIN, LOW);
          beepSound.beepBuzzer(150, 1);
          // Force immediate sensor display update after turning ON
          soilSensor.update(true);
          tempSensor.lcdTemperatureSensor();
          humiditySensor.update(true);
          lastSensorUpdateTime = millis();
          lastHumidityReadTime = millis();
        } else {
          Serial.println("Sensor OFF");
          soilSensor.powerOff();
          lcdDisplay.showPowerStatus(false);
          powerManager.powerOffSystem();
          digitalWrite(POWER_LED_ON_PIN, LOW);
          digitalWrite(POWER_LED_OFF_PIN, HIGH);
          beepSound.beepBuzzer(150, 3);
          digitalWrite(PH_RED_LED_PIN, LOW);
          digitalWrite(PH_GREEN_LED_PIN, LOW);
          digitalWrite(PH_BLUE_LED_PIN, LOW);
          digitalWrite(PH_BUZZER_PIN, LOW);
          digitalWrite(LED_SOIL_PIN, LOW);
          digitalWrite(SOIL_BUZZER_PIN, LOW);
          humiditySensor.forcePowerOffUpdate();
          tempSensor.forcePowerOffUpdate();
          soilSensor.forcePowerOffUpdate();
          batteryMode = false;
        }
      }

      buttonStateLongPress = false;
    }

    previousButtonMillis = currentMillis;
  }
}

// --- Function to handle battery display toggle
void readBatteryToggleButton() {
  if (!isOn) return; // Prevent toggle if system is off
  unsigned long now = millis();
  // Debounce battery button
  if (now - batteryPreviousMillis > batteryDebounceDelay) {
    int batteryState = digitalRead(batteryButtonPin);

    // When button is pressed (assuming active LOW)
    if (batteryState == LOW && batteryButtonStatePrevious == HIGH) {
      batteryButtonStatePrevious = LOW;
      batteryMode = !batteryMode;  // Toggle display mode

      lcdDisplay.getLCDPointer()->clear();
      if (batteryMode) {
        Serial.println("Battery Mode");
        // Display battery percentage. Replace "85%" with your measured value.
        lcdDisplay.printTemporaryMessage("Battery Active", 1500);
        beepSound.beepBuzzer(150, 3);
        lcdDisplay.getLCDPointer()->setCursor(0, 0);
        lcdDisplay.getLCDPointer()->print("Stats:No Charger");
        lcdDisplay.getLCDPointer()->setCursor(0, 1);
        lcdDisplay.getLCDPointer()->print("Battery: 85%");
      } else {
        Serial.println("Sensor Mode");
        // Switch back to sensor display. You might call a function that refreshes sensor values.
        lcdDisplay.printTemporaryMessage("Sensors Active", 1500);
        beepSound.beepBuzzer(150, 3);
      }
    }

    if (batteryState == HIGH && batteryButtonStatePrevious == LOW) {
      batteryButtonStatePrevious = HIGH;
    }

    batteryPreviousMillis = now;
  }
}

void loop() {
  currentMillis = millis();

  // Check power button (and restart) logic
  readPowerButtonState();

  // Check battery display toggle button only if system is ON
  if (isOn) {
    readBatteryToggleButton();
  }

  // Only update sensors if the system is ON and batteryMode is off.
  if (isOn && !batteryMode && (millis() - lastSensorUpdateTime >= sensorUpdateInterval)) {
    lastSensorUpdateTime = millis();
    soilSensor.update(true);
    tempSensor.lcdTemperatureSensor();
    if (millis() - lastHumidityReadTime >= humidityInterval) {
      lastHumidityReadTime = millis();
      humiditySensor.update(true);
    }
  }
}
