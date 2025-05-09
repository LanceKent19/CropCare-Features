#ifndef SENSOR_DATA_H
#define SENSOR_DATA_H

struct SensorData {
  float ph = NAN;
  int moisture = -1;
  float temperature = NAN;
  int humidity = NAN;
  String powerState = "off";
  
  bool anySensorFailed() const {
    return isnan(ph) || 
           moisture == -1 || 
           isnan(temperature) || 
           isnan(humidity);
  }
};

#endif