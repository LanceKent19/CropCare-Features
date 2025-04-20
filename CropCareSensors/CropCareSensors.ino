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
unsigned long sensorUpdateInterval = 1000;
unsigned long lastHumidityReadTime = 0;
const unsigned long humidityInterval = 3000;

// --- Button variables for Battery Toggle Button (BATTERY_TOGGLE_PIN)
const int batteryButtonPin = BATTERY_TOGGLE_PIN;
int batteryButtonStatePrevious = HIGH;  // With INPUT_PULLUP, default is HIGH
unsigned long batteryPreviousMillis = 0;
const int batteryDebounceDelay = 50;
bool batteryMode = false;         // false = show sensors, true = show battery percentage
bool batteryDisplayMode = false;  // true if LCD is showing battery

unsigned long tempRequestTime = 0;
bool tempRequested = false;
unsigned long tempReadInterval = 5000;  // every 5 seconds
unsigned long lastTempReadTime = 0;

// Replace your existing button variables with these:
volatile bool buttonPressed = false;           // Flag set by interrupt
volatile unsigned long buttonPressTime = 0;    // When button was pressed
const unsigned long longPressDuration = 3000;  // 3 second long-press
bool buttonHandled = false;                    // Prevents duplicate handling


// Battery button interrupt handling
volatile bool batteryButtonPressed = false;
volatile unsigned long batteryButtonPressTime = 0;
bool batteryButtonHandled = false;
const unsigned long batteryButtonDebounceTime = 50;  // 50ms debounce

void IRAM_ATTR handlePowerButtonInterrupt() {
  if (!buttonPressed) {
    buttonPressed = true;
    buttonPressTime = millis();
  }
}
void IRAM_ATTR handleBatteryButtonInterrupt() {
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  
  // Simple debounce - ignore if interrupts come too fast
  if (interrupt_time - last_interrupt_time > 200) {
    batteryButtonPressed = true;
    batteryButtonPressTime = interrupt_time;
  }
  last_interrupt_time = interrupt_time;
}
void setup() {
  Serial.begin(115200);
  pinMode(buttonPin, INPUT_PULLUP);
  // Enable the interrupt for the power button pin
  attachInterrupt(digitalPinToInterrupt(POWER_BUTTON_PIN), handlePowerButtonInterrupt, FALLING);
  pinMode(batteryButtonPin, INPUT_PULLUP);
  // Configure battery button interrupt
   attachInterrupt(digitalPinToInterrupt(BATTERY_TOGGLE_PIN), handleBatteryButtonInterrupt, FALLING);  // Trigger on both press and release
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
  if (buttonPressed) {
    unsigned long pressDuration = millis() - buttonPressTime;

    // Handle long press (restart)
    if (pressDuration >= longPressDuration && !buttonHandled) {
      buttonHandled = true;

      lcdDisplay.getLCDPointer()->clear();
      lcdDisplay.getLCDPointer()->setCursor(3, 0);
      lcdDisplay.getLCDPointer()->print("System");
      lcdDisplay.getLCDPointer()->setCursor(2, 1);
      lcdDisplay.getLCDPointer()->print("Restarting...");
      beepSound.beepBuzzer(100, 3);
      delay(500);
      ESP.restart();
    }

    // Handle button release
    if (digitalRead(buttonPin)) {  // Button released (HIGH with PULLUP)
      if (!buttonHandled && pressDuration < longPressDuration) {
        isOn = !isOn;
        handlePowerToggle();
      }

      // Reset flags
      buttonPressed = false;
      buttonHandled = false;
    }
  }
}

void handlePowerToggle() {
  if (isOn) {
    Serial.println("Sensor ON");
    lcdDisplay.showPowerStatus(true);
    powerManager.powerOnSystem();
    lcdDisplay.printTemporaryMessage(wifiManager.isConnected() ? "WiFi: Connected" : "WiFi: Offline", 1500);
    digitalWrite(POWER_LED_ON_PIN, HIGH);
    digitalWrite(POWER_LED_OFF_PIN, LOW);
    beepSound.beepBuzzer(150, 1);

    // Initialize sensors
    soilSensor.update(true, !batteryMode);
    tempSensor.lcdTemperatureSensor(!batteryMode);
    humiditySensor.update(!batteryMode);
    lastSensorUpdateTime = millis();
  } else {
    Serial.println("Sensor OFF");
    lcdDisplay.showPowerStatus(false);
    digitalWrite(POWER_LED_ON_PIN, LOW);
    digitalWrite(POWER_LED_OFF_PIN, HIGH);
    beepSound.beepBuzzer(150, 3);

    // Turn off all indicators
    digitalWrite(PH_RED_LED_PIN, LOW);
    digitalWrite(PH_GREEN_LED_PIN, LOW);
    digitalWrite(PH_BLUE_LED_PIN, LOW);
    digitalWrite(PH_BUZZER_PIN, LOW);
    digitalWrite(LED_SOIL_PIN, LOW);
    digitalWrite(SOIL_BUZZER_PIN, LOW);

    // Update sensors
    humiditySensor.forcePowerOffUpdate();
    tempSensor.forcePowerOffUpdate();
    soilSensor.forcePowerOffUpdate();
    batteryMode = false;
  }
}
void toggleBatteryMode() {
  batteryMode = !batteryMode;

  lcdDisplay.getLCDPointer()->clear();
  if (batteryMode) {
    Serial.println("Battery Mode");
    lcdDisplay.printTemporaryMessage("Battery Active", 1500);
    beepSound.beepBuzzer(150, 3);

    // Get actual battery percentage from powerManager
    // float batteryPercent = powerManager.getBatteryPercentage(); // You'll need to implement this
    lcdDisplay.getLCDPointer()->setCursor(0, 0);
    lcdDisplay.getLCDPointer()->print("Stats:No Charger");
    lcdDisplay.getLCDPointer()->setCursor(0, 1);
    lcdDisplay.getLCDPointer()->print("Battery: 85%");
    // lcdDisplay.getLCDPointer()->print(batteryPercent);
  } else {
    Serial.println("Sensor Mode");
    lcdDisplay.printTemporaryMessage("Sensors Active", 1500);
    beepSound.beepBuzzer(150, 3);
    // Sensors will update automatically in next loop()
  }
}

void readBatteryToggleButton() {
  if (!isOn) return; // Still prevent toggles when system is off

  if (batteryButtonPressed) {
    // Wait for button to be properly released
    if (digitalRead(BATTERY_TOGGLE_PIN) == HIGH) {
      // Only toggle if press was >50ms (debounce) and <2s (avoid accidental presses)
      unsigned long pressDuration = millis() - batteryButtonPressTime;
      
      if (pressDuration > 50 && pressDuration < 2000 && !batteryButtonHandled) {
        toggleBatteryMode();
        batteryButtonHandled = true;
      }
      
      batteryButtonPressed = false;
      batteryButtonHandled = false;
    }
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
  if (isOn && (millis() - lastSensorUpdateTime >= sensorUpdateInterval)) {
    lastSensorUpdateTime = millis();
    humiditySensor.update(!batteryMode);
    tempSensor.requestTemperature();                // NEW function you will define
    tempSensor.lcdTemperatureSensor(!batteryMode);  // NEW function you will define
    soilSensor.update(true, !batteryMode);
    // if (millis() - lastHumidityReadTime >= humidityInterval) {
    //   lastHumidityReadTime = millis();
    //   humiditySensor.update(!batteryMode);
    // }

    // Async Temp Sensor Handling
    // if (!tempRequested && millis() - lastTempReadTime >= tempReadInterval) {
    //   tempSensor.requestTemperature();  // NEW function you will define
    //   tempRequestTime = millis();
    //   tempRequested = true;
    // }
    //   if (tempRequested && millis() - tempRequestTime >= 750) {
    //   tempSensor.lcdTemperatureSensor(!batteryMode);  // NEW function you will define
    //   lastTempReadTime = millis();
    //   tempRequested = false;
    // }
  }
}
