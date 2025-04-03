/////////////////////////////////////////////INITALIZATION FOR TEMPERATURE SENSOR///////////////////////////////////////////////////////////
  // Libraries of essentials for Temperature Sensor
  #include <OneWire.h>            // Allows communication for one wire devices like DS18b20
  #include <DallasTemperature.h>  // For Temperature Readings

  #define ONE_WIRE_BUS 23  // GPIO Pin that I use for sensoring temperature

  OneWire oneWire(ONE_WIRE_BUS);        // Creating an object and assigning the oneWire object into the GPIO 22
  DallasTemperature sensors(&oneWire);  // & means addres of so it passes the sensor to read the temperature


/////////////////////////////////////////////INITALIZATION FOR SOIL MOISTURE////////////////////////////////////////////////////////////////
  #define SOIL_MOISTURE_PIN 34  // Use any available analog pin

  const int DRY_VALUE = 3500;  // The range of the Dry moisture Value
  const int WET_VALUE = 1000;  // The range of the Wet moisture Value
  String soilCondition;        // A string variable for storing the condition of the soil

/////////////////////////////////////////////INITALIZATION FOR PH LEVEL/////////////////////////////////////////////////////////////////////
  // Pins : LCD(SCL - P22, SDA - P21)
  #include <LiquidCrystal_I2C.h>       // Libraries for LiquidCrystal I2C
  LiquidCrystal_I2C lcd(0x27, 16, 2);  // Initialize the LiquidCrystal with 16x2 with I2C address 0x27
  float calibri_val = 21.34;           // Calibration value of ph level
  int phpin = 35;                      // Pins use in analog readings
  unsigned long int avgval;            // variable that stores average sensor reading or value
  int buffer_arr[10], temp;            // store 10 sensors readings for noise filtering, the temp used for temporary variable

void setup() {
  Serial.begin(115200);  // the baudrate I use
  phLevelSetup();
}

void loop() {
  temperatureSensor();
  soilMoistureSensor();
  phLevelSensor();
  delay(1000);  // Wait 1 second before reading again
}

void temperatureSensor() {
  sensors.begin();                // Initializing the temperature sensor
  sensors.requestTemperatures();  // Request temperature from sensor

  float temperatureC = sensors.getTempCByIndex(0);  // Read temperature in Celsius from 0
  Serial.print("\nTemperature: ");
  Serial.print(temperatureC);  // Prints the Celcius measured and being displayed in the serial monitor
  Serial.println(" °C");

  float temperatureF = sensors.getTempFByIndex(0);  // Reads temperature by Fahrenheit
  Serial.print("Fahrenheit: ");
  Serial.print(temperatureF);  // Prints the Fahrenheit measured and being displayed in the serial monitor
  Serial.println(" °F");
}

void soilMoistureSensor() {
  int moistureValue = analogRead(SOIL_MOISTURE_PIN);  // Read sensor value which is pin 34 in the esp32

  // Convert to percentage
  int moisturePercent = 100 - ((moistureValue - WET_VALUE) * 100 / (DRY_VALUE - WET_VALUE));

  // will stays from 0-100% using constrain method, 0 is the minumum and 100 is the mazimum
  moisturePercent = constrain(moisturePercent, 0, 100);

  // The condition statements for the soil Condition based on the moisture percent above
  if (moisturePercent <= 10) {
    soilCondition = "Very Dry";  // will print Very Dry if the value is 10 or lower
  } else if (moisturePercent <= 40) {
    soilCondition = "Dry Soil";  // will print Dry Soil if the value is 40 or lower
  } else if (moisturePercent <= 80) {
    soilCondition = "Moist Soil";  // will print Moist Soil if the value is 80 or lower
  } else if (moisturePercent < 100) {
    soilCondition = "Very Wet/Near Mud";  // it will print Very Wet/Near Mud if the value of the moisturePercent is lower than 100
  } else if (moisturePercent == 100) {
    soilCondition = "Submerged in Water";  // will print if the moisturePercent has reached the maximum moisture percent
  }
  Serial.print("\nSoil Moisture Value: ");
  Serial.println(moistureValue);  // Print value to Serial Monitor
  Serial.print("Soil Moisture Percentage: ");
  Serial.print(moisturePercent);  // Print the Moisture percent based on the convertion
  Serial.println(" %");

  Serial.print("Soil Condition: ");
  Serial.println(soilCondition);  // Print the Condition based on the condition statements above
}

void phLevelSetup(){
  lcd.init();           // Initialize the lcd
  lcd.backlight();      // turns the lcd to backlight
  lcd.setCursor(2, 0);  // Moves the cursor to column 2, row 0
  lcd.print("PH");      // then display PH
  lcd.setCursor(3, 1);  // moves again the cursor to column 3, row 1
  lcd.print("Level");   // then display the word Level
  delay(3000);          // delay with 3 seconds
  lcd.clear();          // clear the lcd after it
}

void phLevelSensor(){
  for (int i = 0; i < 10; i++) {        // Will read 10 ph sensor values and stores then in buffer array
    buffer_arr[i] = analogRead(phpin);  // reads the voltage with the use of phpin
    delay(30);                          // wait 3ms between readings
  }
  for (int i = 0; i < 9; i++) {         // sort the buffer readings in ascending order(bubble sort)
    for (int j = i + 1; j < 10; j++) {  // this helps to remove outliers from sensor readings
      if (buffer_arr[i] > buffer_arr[j]) {
        temp = buffer_arr[i];
        buffer_arr[i] = buffer_arr[j];
        buffer_arr[j] = temp;
      }
    }
  }
  avgval = 0;
  for (int i = 2; i < 8; i++)                     // removes the first 2 and the last 2 in readings
    avgval += buffer_arr[i];                      // will compute the average of the remaing 6 readings for more stable readings
  float volt = (float)avgval * 3.3 / 4096.0 / 6;  // Convert ADC Readings to Voltage
  float ph_act = -4.34 * volt + calibri_val;      // Converts the Voltage into a Ph Value

  // Will Display the output in the LCD
  lcd.setCursor(0, 0);     // moves the cursor in column 0, row 0
  lcd.print("PH Value:");  // Display the PH Value
  lcd.setCursor(10, 0);    // move again the cursor in column 10, row 0
  lcd.print(ph_act, 2);    // display the Ph Value will decimal places of 2

  Serial.print("Ph Value: ");  // will also print in the serial monitor
  Serial.println(ph_act);      // will also print the ph value

}