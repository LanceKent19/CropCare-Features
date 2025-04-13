#include "PhSensor.h"

// Constructor
PhSensor::PhSensor(int pin, float caliVal, LiquidCrystal_I2C& lcd)
  : pin(pin), caliVal(caliVal), lcd(lcd) {}

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

  return ph_act;
}
