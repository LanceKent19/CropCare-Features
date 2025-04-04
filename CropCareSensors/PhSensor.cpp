#include "PhSensor.h"
// passing the constructors with variables
PhSensor::PhSensor(int pin, float caliVal, LiquidCrystal_I2C& lcd)
  : pin(pin), caliVal(caliVal), lcd(lcd) {}

float PhSensor::readPh() { // Reading the ph sensors
    for (int i = 0; i < 10; i++) {
        buffer_arr[i] = analogRead(pin);
        delay(30);
    }

    for (int i = 0; i < 9; i++) {
        for (int j = i + 1; j < 10; j++) {
            if (buffer_arr[i] > buffer_arr[j]) {
                int temp = buffer_arr[i];
                buffer_arr[i] = buffer_arr[j];
                buffer_arr[j] = temp;
            }
        }
    }

    unsigned long avgval = 0;
    for (int i = 2; i < 8; i++) avgval += buffer_arr[i];

    float volt = (float)avgval * 3.3 / 4096.0 / 6;
    float ph_act = -4.34 * volt + caliVal;

    lcd.setCursor(0, 0);
    lcd.print("PH:");
    lcd.setCursor(4, 0);
    lcd.print(ph_act, 2);

    Serial.print("Ph Value: ");
    Serial.println(ph_act);

    return ph_act;
}