#include "hih-4030.h"

HIH4030::HIH4030(PinName vout):vout_(vout) {
  
}

/*
    gives humidity as a ratio of VDD
*/
float HIH4030::ratioHumidity(){
  //poll analogue in
  return vout_.read(); 
}

/*
    gives humidity as a percentage - numbers taken from datasheet
*/

float HIH4030::sensorRH(){
  //poll analogue in
  sample = vout_.read()*5; //multiply by 5 as sample is a decimal of Vdd
  return (sample-0.958)/0.0307;
}

/*
    gives humidity adjusted for temperature (in degrees C) - numbers taken from datasheet
*/

float HIH4030::trueSensorRH(float temperature){
  float rh = sensorRH();
  temperature = temperature*0.00216;
  return rh/(1.0546 - temperature);
}