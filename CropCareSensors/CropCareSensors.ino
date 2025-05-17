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

#include "TemperatureSensor.h"
#include "SoilMoistureSensor.h"
#include "PhSensor.h" 
#include "HumiditySensor.h"
#include "LCDDisplay.h"
#include "BeepSound.h"
#include "WifiManager.h"
#include "PowerManager.h"
#include "SensorData.h"  // Add this with other includes
#include "Config.h"

// Classes
BeepSound beepSound(POWER_BUZZER_PIN);
WiFiManager wifiManager("OPPO Reno8 T 5G", "wilms0523");  
LCDDisplay lcdDisplay;
PowerManager powerManager(lcdDisplay.getLCD());
TemperatureSensor tempSensor(TEMPERATURE_PIN, lcdDisplay.getLCD(), wifiManager);
SoilMoistureSensor soilSensor(SOIL_PIN, 3500, 1000, LED_SOIL_PIN, SOIL_BUZZER_PIN, lcdDisplay.getLCD(), wifiManager);
PhSensor phSensor(PH_LEVEL_PIN, 21.75, PH_RED_LED_PIN, PH_GREEN_LED_PIN, PH_BLUE_LED_PIN, PH_BUZZER_PIN, lcdDisplay.getLCD(), wifiManager, powerManager);
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

// For timing sensor updates
unsigned long lastSensorUpdateTime = 0;
const unsigned long sensorUpdateInterval = 1000;  // 1 second (adjust as needed)


// --- BUTTON INTERRUPTS
volatile bool powerInterruptTriggered = false;  // Flag to indicate interrupt
unsigned long lastPowerToggleTime = 0;          // For debounce logic
const unsigned long debounceDelay = 300;        // 300ms debounce delay

void IRAM_ATTR handlePowerButtonInterrupt() {
  powerInterruptTriggered = true;  // Flag when the power button is pressed
}
  // Add this near your other defines
const char* serverURL = "https://cropcare.flashlearn.site/sse_updates.php";

void setup() {
  Serial.begin(115200);
  pinMode(POWER_BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(POWER_BUTTON_PIN), handlePowerButtonInterrupt, FALLING);

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

  powerManager.begin();

  lcdDisplay.begin();
  lcdDisplay.getLCDPointer()->backlight();  // Ensure backlight is on
  wifiManager.setLCD(lcdDisplay.getLCDPointer());
  wifiManager.setBeepSound(&beepSound);
  wifiManager.connect(6000);

  tempSensor.begin();
  tempSensor.forcePowerOffUpdate();
  humiditySensor.begin();
  humiditySensor.forcePowerOffUpdate();
  soilSensor.forcePowerOffUpdate();
  phSensor.forcePowerOffUpdate();
  // Initialize button states
  buttonStatePrevious = digitalRead(POWER_BUTTON_PIN);
}

void readPowerButtonState() {
  if (powerInterruptTriggered) {
    unsigned long currentMillis = millis();
    if (currentMillis - lastPowerToggleTime >= debounceDelay) {
      Serial.println("Power button pressed (interrupt).");
      isOn = !isOn;
      Serial.println(isOn ? "Turn On" : "Turn Off");
      handlePowerToggle();
      lastPowerToggleTime = currentMillis;
    }
    powerInterruptTriggered = false;
    return;  // Important: stop further processing after interrupt is handled
  }

  // Existing logic for long press detection
  if (millis() - previousButtonMillis > intervalButton) {
    int buttonState = digitalRead(POWER_BUTTON_PIN);

    // Detect button press
    if (buttonState == HIGH && buttonStatePrevious == LOW && !buttonStateLongPress) {
      buttonLongPressMillis = millis();
      buttonStatePrevious = HIGH;
    }

    buttonPressDuration = millis() - buttonLongPressMillis;

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

    buttonStatePrevious = buttonState;
    previousButtonMillis = millis();
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
    soilSensor.update();  // We already reset batteryMode = false
    tempSensor.lcdTemperatureSensor();
    humiditySensor.update();
    phSensor.readPhNonBlocking();
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
    phSensor.forcePowerOffUpdate();
  }
}
void loop() {
  currentMillis = millis();

  // Check power button (and restart) logic
  readPowerButtonState();

  // Only update sensors if the system is ON and batteryMode is off.
  if (isOn && (millis() - lastSensorUpdateTime >= sensorUpdateInterval)) {
    lastSensorUpdateTime = millis();
    
      SensorData data;
    data.powerState = isOn ? "on" : "off";
    
    // Update sensors and collect data
    humiditySensor.update();
    data.humidity = humiditySensor.getLastValue();
    
    tempSensor.requestTemperature();
    tempSensor.lcdTemperatureSensor();
    data.temperature = tempSensor.getLastValue();
    
    soilSensor.update();
    data.moisture = soilSensor.getLastValue();
    
    phSensor.readPhNonBlocking();
    data.ph = phSensor.getLastValue();

    // Send all data in one HTTP request
    wifiManager.sendAllSensorData(serverURL, data);
  }
}