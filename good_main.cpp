#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <imumaths.h>
#include "mbed.h"

/* Set the delay between fresh samples */
#define BNO055_SAMPLERATE_DELAY_MS (100)

Serial pc(USBTX, USBRX);
I2C i2c(p30, p7);
Adafruit_BNO055 bno = Adafruit_BNO055(-1, BNO055_ADDRESS_A, &i2c);

void loop();

/**************************************************************************/
/*
    Arduino setup function (automatically called at startup)
*/
/**************************************************************************/
int main()
{
  pc.baud(9600);
  pc.printf("Orientation Sensor Raw Data Test\r\n");

  /* Initialise the sensor */
  if(!bno.begin())
  {
    /* There was a problem detecting the BNO055 ... check your connections */
    pc.printf("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!\r\n");
    while(1);
  }
  else
    pc.printf("BNO055 was detected!\r\n");

  wait(1);

  /* Display the current temperature */
  int8_t temp = bno.getTemp();
  pc.printf("Current Temperature: %d C\r\n", temp);
  bno.setExtCrystalUse(true);

  pc.printf("Calibration status values: 0=uncalibrated, 3=fully calibrated\r\n");

  while(true)
      loop();
}

/**************************************************************************/
/*
    Arduino loop function, called once 'setup' is complete (your own code
    should go here)
*/
/**************************************************************************/
void loop(void)
{
  // Possible vector values can be:
  // - VECTOR_ACCELEROMETER - m/s^2
  // - VECTOR_MAGNETOMETER  - uT
  // - VECTOR_GYROSCOPE     - rad/s
  // - VECTOR_EULER         - degrees
  // - VECTOR_LINEARACCEL   - m/s^2
  // - VECTOR_GRAVITY       - m/s^2
  imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);

  /* Display the floating point data */
  pc.printf("X: %f Y: %f Z: %f\t\t", euler.x(), euler.y(), euler.z());

  /*
  // Quaternion data
  imu::Quaternion quat = bno.getQuat();
  Serial.print("qW: ");
  Serial.print(quat.w(), 4);
  Serial.print(" qX: ");
  Serial.print(quat.y(), 4);
  Serial.print(" qY: ");
  Serial.print(quat.x(), 4);
  Serial.print(" qZ: ");
  Serial.print(quat.z(), 4);
  Serial.print("\t\t");
  */

  /* Display calibration status for each sensor. */
  uint8_t system, gyro, accel, mag = 0;
  bno.getCalibration(&system, &gyro, &accel, &mag);
  pc.printf("CALIBRATION: Sys=%d, Gyro=%d, Accel=%d, Mag=%d\r\n", (int)(system), (int)(gyro), (int)(accel), (int)(mag));
  wait_ms(BNO055_SAMPLERATE_DELAY_MS);
}
