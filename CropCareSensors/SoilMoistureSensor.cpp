#include "SoilMoistureSensor.h"

SoilMoistureSensor::SoilMoistureSensor(int pin, int dryVal, int wetVal, int ledPin, int buzzerPin, LiquidCrystal_I2C &lcd, WiFiManager &wifiManager)
  : pin(pin), dryValue(dryVal), wetValue(wetVal), ledPin(ledPin), buzzerPin(buzzerPin), lcd(lcd), wifiManager(wifiManager),
    dryTimerStarted(false), dryStartTime(0), ledState(false) {}

int SoilMoistureSensor::readValue() {
  return analogRead(pin);
}

int SoilMoistureSensor::getMoisturePercent() {
  int value = readValue();
  int percent = 100 - ((value - wetValue) * 100 / (dryValue - wetValue));
  return constrain(percent, 0, 100);
}

void SoilMoistureSensor::powerOn() {
  ledState = true;
}

void SoilMoistureSensor::powerOff() {
  digitalWrite(ledPin, LOW);
  digitalWrite(buzzerPin, LOW);
  ledState = false;
  dryTimerStarted = false;
}

void SoilMoistureSensor::sendMoistToServer(int moisture) {
  String body = "moistureSensor=" + String(moisture) + "&powerState=on";
  wifiManager.sendHTTPPost(serverURL, body);
}

void SoilMoistureSensor::forcePowerOffUpdate() {
  String body = "moistureSensor=-&powerState=off";
  wifiManager.sendHTTPPost(serverURL, body);
}

void SoilMoistureSensor::update(bool isActive, bool showOnLCD) {
  if (!isActive) {
    powerOff();
    return;
  }

  int moisture = getMoisturePercent();
  String condition;

  if (moisture >= 30 && moisture <= 40) {
    condition = "Idle";
    ledState = false;
    dryTimerStarted = false;
  } else if (moisture < 10 || moisture < 30) {
    condition = (moisture < 10) ? "Very Dry" : "Dry";
    if (!dryTimerStarted) {
      dryStartTime = millis();
      dryTimerStarted = true;
    }
    ledState = (millis() - dryStartTime >= dryDelay);
  } else if (moisture <= 60) {
    condition = "Moist";
    ledState = false;
    dryTimerStarted = false;
  } else if (moisture <= 90) {
    condition = "Wet";
    ledState = false;
    dryTimerStarted = false;
  } else {
    condition = "Submerged";
    ledState = true;
    dryTimerStarted = false;
  }

  digitalWrite(ledPin, ledState);
  digitalWrite(buzzerPin, ledState);

  Serial.print("Moisture: ");
  Serial.print(moisture);
  Serial.println(" %");

  // Serial.print("Condition: ");
  // Serial.println(condition);

  if (showOnLCD) {
    char buffer[16];
    sprintf(buffer, "MP:%3d%%", moisture);  // right-aligned in 3 spaces
    lcd.setCursor(0, 1);
    lcd.print(buffer);
  }
  sendMoistToServer(moisture);
}
