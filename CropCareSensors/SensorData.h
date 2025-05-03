#ifndef SENSOR_DATA_H
#define SENSOR_DATA_H

struct SensorData {
  float phValue = NAN;
  int moisturePercent = -1;
  float temperature = NAN;
  int humidity = NAN;
  String powerState = "off";
  
  bool anySensorFailed() const {
    return isnan(phValue) || 
           moisturePercent == -1 || 
           isnan(temperature) || 
           isnan(humidity);
  }
};

#endif