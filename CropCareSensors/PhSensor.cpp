#include "PhSensor.h"

// Constructor
PhSensor::PhSensor(int pin, float caliVal, int redLed, int greenLed, int blueLed, int buzzer ,LiquidCrystal_I2C& lcd, WiFiManager& wifiManager)
  : pin(pin), caliVal(caliVal), redLed(redLed), greenLed(greenLed),blueLed(blueLed),buzzer(buzzer), lcd(lcd), wifiManager(wifiManager) {}

float PhSensor::readPh() {
  int buffer_arr[10];

  // Read analog values into buffer
  for (int i = 0; i < 10; i++) {
    buffer_arr[i] = analogRead(pin); // Use the correct pin passed to constructor
    delay(30);
  }

  // Sort the readings to remove outliers
  for (int i = 0; i < 9; i++) {
    for (int j = i + 1; j < 10; j++) {
      if (buffer_arr[i] > buffer_arr[j]) {
        int temp = buffer_arr[i];
        buffer_arr[i] = buffer_arr[j];
        buffer_arr[j] = temp;
      }
    }
  }

  // Average the middle 6 values
  int avgval = 0;
  for (int i = 2; i < 8; i++) {
    avgval += buffer_arr[i];
  }
  avgval /= 6;

  float voltage = (float)avgval * 3.3 / 4095.0;
  float ph_act = -5.70 * voltage + caliVal;

  // Display on LCD
  lcd.setCursor(0, 0);
  lcd.print("PH:");
  lcd.setCursor(3, 0);
  lcd.print(ph_act, 2);

  Serial.print("pH Value: "); 
  Serial.println(ph_act);

  sendPhToServer(ph_act);
  if(ph_act <= 6.5){
    Serial.println("Acidic");
    digitalWrite(redLed, HIGH);
    digitalWrite(greenLed, LOW);
    digitalWrite(blueLed, LOW);
    digitalWrite(buzzer, HIGH);
  }else if(ph_act <= 7.5){
    Serial.println("Neutral");
    digitalWrite(redLed, LOW);
    digitalWrite(greenLed, HIGH);
    digitalWrite(blueLed, LOW);
    digitalWrite(buzzer, LOW);
  }else if(ph_act > 7.5){
    Serial.println("Alkaline");
    digitalWrite(redLed, LOW);
    digitalWrite(greenLed, LOW);
    digitalWrite(blueLed, HIGH);
    digitalWrite(buzzer, HIGH);
  }
  return ph_act;
}
