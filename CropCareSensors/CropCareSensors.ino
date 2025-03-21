/////////////////////////////////////////////INITALIZATION FOR TEMPERATURE SENSOR///////////////////////////////////////////////////////////
  // Libraries of essentials for Temperature Sensor
  #include <OneWire.h> // Allows communication for one wire devices like DS18b20
  #include <DallasTemperature.h> // For Temperature Readings

  #define ONE_WIRE_BUS 22  // GPIO Pin that I use for sensoring temperature

  OneWire oneWire(ONE_WIRE_BUS); // Creating an object and assigning the oneWire object into the GPIO 22
  DallasTemperature sensors(&oneWire); // & means addres of so it passes the sensor to read the temperature


/////////////////////////////////////////////INITALIZATION FOR SOIL MOISTURE////////////////////////////////////////////////////////////////
  #define SOIL_MOISTURE_PIN 34  // Use any available analog pin

  const int DRY_VALUE = 3500;  // The range of the Dry moisture Value
  const int WET_VALUE = 1000;  // The range of the Wet moisture Value
  String soilCondition;        // A string variable for storing the condition of the soil

void setup() {
  Serial.begin(115200); // the baudrate I use
  
}

void loop() {
  temperatureSensor();
  soilMoistureSensor();
  delay(1000);  // Wait 1 second before reading again
}

void temperatureSensor(){
  sensors.begin(); // Initializing the temperature sensor
  sensors.requestTemperatures();   // Request temperature from sensor
                   
  float temperatureC = sensors.getTempCByIndex(0);  // Read temperature in Celsius from 0
  Serial.print("\nTemperature: ");
  Serial.print(temperatureC); // Prints the Celcius measured and being displayed in the serial monitor
  Serial.println(" °C");

  float temperatureF = sensors.getTempFByIndex(0); // Reads temperature by Fahrenheit
  Serial.print("Fahrenheit: ");
  Serial.print(temperatureF); // Prints the Fahrenheit measured and being displayed in the serial monitor
  Serial.println(" °F");
}

void soilMoistureSensor(){
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