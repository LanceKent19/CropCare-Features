#include "SoilMoistureSensor.h"

SoilMoistureSensor::SoilMoistureSensor(int pin, int dryVal, int wetVal, int ledPin, int buzzerPin, LiquidCrystal_I2C &lcd)
  : pin(pin), dryValue(dryVal), wetValue(wetVal), ledPin(ledPin), buzzerPin(buzzerPin), lcd(lcd),
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
    lcd.clear();
  lcd.backlight();
  lcd.setCursor(3, 0);
  lcd.print("System On");
  delay(1000);  // Show "System On" briefly
  lcd.clear();
}

void SoilMoistureSensor::powerOff() {
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("System OFF");
  delay(1000);  // Optional: small delay before LCD fully turns off
  lcd.clear();
  lcd.noBacklight();

  digitalWrite(ledPin, LOW);
  digitalWrite(buzzerPin, LOW);
  ledState = false;
  dryTimerStarted = false;
}

void SoilMoistureSensor::update(bool isActive) {
  if (!isActive) {
    powerOff();
    return;
  }

  int moisture = getMoisturePercent();
  String condition;

  if (moisture >= 30 && moisture <= 40) {
    condition = "Sensor Idle";
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

  Serial.print("Condition: ");
  Serial.println(condition);

  lcd.setCursor(0, 1);
  lcd.print("MP:");
  lcd.print(moisture);
  lcd.print("%");

  lcd.setCursor(7, 1);
  lcd.print("          ");
  lcd.setCursor(7, 1);
  lcd.print(condition);
}
