#ifndef HIH4030_h
#define HIH4030_h

#include "mbed.h"

class HIH4030{

public:
  HIH4030(PinName vout);
  float ratioHumidity();
  float sensorRH();
  float trueSensorRH(float temperature);

private:
  AnalogIn vout_;
  float sample;
  float temperature;
};
#endif