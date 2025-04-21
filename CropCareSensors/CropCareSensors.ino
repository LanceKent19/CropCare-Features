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

// --- POWER BUTTON LOGIC
int buttonStatePrevious = LOW;
bool buttonStateLongPress = false;
unsigned long buttonLongPressMillis = 0;
unsigned long buttonPressDuration = 0;
unsigned long previousButtonMillis = 0;
unsigned long currentMillis = 0;
const unsigned long intervalButton = 50;
const unsigned long minButtonLongPressDuration = 3000;
bool isOn = false;

const int buttonPin = POWER_BUTTON_PIN;

// For timing sensor updates
unsigned long lastSensorUpdateTime = 0;
const unsigned long sensorUpdateInterval = 1000; // 1 second (adjust as needed)

// --- BATTERY TOGGLE BUTTON
const int batteryButtonPin = BATTERY_TOGGLE_PIN;
int batteryButtonStatePrevious = HIGH;
bool batteryMode = false;
unsigned long batteryPreviousMillis = 0;
volatile bool batteryButtonPressed = false;
bool batteryButtonHandled = false;

// --- BUTTON INTERRUPTS
volatile bool powerInterruptTriggered = false;  // Flag to indicate interrupt
unsigned long lastPowerToggleTime = 0;  // For debounce logic
const unsigned long debounceDelay = 300;  // 300ms debounce delay

void IRAM_ATTR handlePowerButtonInterrupt() {
  powerInterruptTriggered = true;  // Flag when the power button is pressed
}

// Globals
volatile bool batteryInterruptTriggered = false;
unsigned long lastBatteryToggleTime = 0;

void IRAM_ATTR handleBatteryButtonInterrupt() {
  batteryInterruptTriggered = true;
}


void setup() {
  Serial.begin(115200);
  pinMode(buttonPin, INPUT_PULLUP);
  // Enable the interrupt for the power button pin
  attachInterrupt(digitalPinToInterrupt(buttonPin), handlePowerButtonInterrupt, FALLING);
  pinMode(batteryButtonPin, INPUT_PULLUP);
  // Configure battery button interrupt
  attachInterrupt(digitalPinToInterrupt(batteryButtonPin), handleBatteryButtonInterrupt, FALLING);  // Trigger on both press and release
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
  unsigned long currentMillis = millis();

  // Check if power button interrupt was triggered
  if (powerInterruptTriggered) {
    powerInterruptTriggered = false;  // Reset the flag

    // Ensure debounce by checking time difference
    if (currentMillis - lastPowerToggleTime >= debounceDelay) {
      Serial.println("Power button pressed.");
      
      // Call the function that toggles the power state
      isOn = !isOn;
      Serial.println(isOn ? "Turn On" : "Turn Off");
      handlePowerToggle();  // Toggle the system state (turn on/off)

      lastPowerToggleTime = currentMillis;  // Update the last toggle time
    }
  }

  // Existing logic for long press detection (button press duration logic)
  if (currentMillis - previousButtonMillis > intervalButton) {
    int buttonState = digitalRead(buttonPin);

    // Detect button press (rising edge)
    if (buttonState == HIGH && buttonStatePrevious == LOW && !buttonStateLongPress) {
      buttonLongPressMillis = currentMillis;
      buttonStatePrevious = HIGH;
    }

    // Calculate press duration
    buttonPressDuration = currentMillis - buttonLongPressMillis;

    // Handle long press to restart the system
    if (buttonState == HIGH && !buttonStateLongPress && buttonPressDuration >= minButtonLongPressDuration) {
      buttonStateLongPress = true;
      Serial.println("Restart");

      lcdDisplay.getLCDPointer()->clear();
      lcdDisplay.getLCDPointer()->setCursor(3, 0);
      lcdDisplay.getLCDPointer()->print("System");
      lcdDisplay.getLCDPointer()->setCursor(2, 1);
      lcdDisplay.getLCDPointer()->print("Restarting...");
      beepSound.beepBuzzer(100, 3);
      ESP.restart();
    }

    // Handle short press to toggle power state
    if (buttonState == LOW && buttonStatePrevious == HIGH) {
      buttonStatePrevious = LOW;

      // Toggle power state if it's a short press (not long press)
      if (buttonPressDuration < minButtonLongPressDuration) {
        isOn = !isOn;
        Serial.println(isOn ? "Turn On" : "Turn Off");
        handlePowerToggle();  // Call the function to toggle power state
      }

      // Reset long press detection after processing
      buttonStateLongPress = false;
    }

    // Update previousButtonMillis to the current time to manage debounce
    previousButtonMillis = currentMillis;
  }
}
  void handlePowerToggle() {
    // Always reset these on every toggle
    batteryMode = false;
    batteryButtonPressed = false;
    batteryButtonHandled = false;
    batteryInterruptTriggered = false;

    if (isOn) {
      Serial.println("Sensor ON");
      lcdDisplay.showPowerStatus(true);
      powerManager.powerOnSystem();
      lcdDisplay.printTemporaryMessage(wifiManager.isConnected() ? "WiFi: Connected" : "WiFi: Offline", 1500);
      digitalWrite(POWER_LED_ON_PIN, HIGH);
      digitalWrite(POWER_LED_OFF_PIN, LOW);
      beepSound.beepBuzzer(150, 1);

      // Initialize sensors
      soilSensor.update(true, true);  // We already reset batteryMode = false
      tempSensor.lcdTemperatureSensor(true);
      humiditySensor.update(true);
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
    }
  }

void toggleBatteryMode() {
  batteryMode = !batteryMode;

  lcdDisplay.getLCDPointer()->clear();
  if (batteryMode) {
    Serial.println("Battery Mode");
    lcdDisplay.printTemporaryMessage("Battery Active", 1500);
    beepSound.beepBuzzer(150, 3);
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
  if (!isOn) return;

  if (batteryInterruptTriggered) {
    unsigned long currentTime = millis();
    if (currentTime - lastBatteryToggleTime >= debounceDelay) {
      Serial.println("Interrupted Firing!");
      toggleBatteryMode();
      lastBatteryToggleTime = currentTime;
    }
    batteryInterruptTriggered = false;  // Reset flag *after* checking debounce
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
  }
}
