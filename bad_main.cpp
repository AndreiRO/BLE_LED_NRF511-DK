#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <imumaths.h>
#include "mbed.h"
#include "ble/BLE.h"

/* Set the delay between fresh samples */
#define BNO055_SAMPLERATE_DELAY_MS (100)

DigitalOut led(LED1, 0);
DigitalOut success(LED2, 1);

uint16_t customServiceUUID  = 0xA000;
uint16_t readCharUUID       = 0xA001;
uint16_t writeCharUUID      = 0xA002;

const static char     DEVICE_NAME[]        = "Gluv"; // change this
static const uint16_t uuid16_list[]        = {0xFFFF}; //Custom UUID, FFFF is reserved for development


/* Set Up custom Characteristics */
static uint8_t readValue = 0;
//ReadOnlyArrayGattCharacteristic<uint8_t, 1> readChar(readCharUUID, readValue);
ReadOnlyGattCharacteristic<uint8_t> rochar(readCharUUID, &readValue, GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY);
ReadOnlyGattCharacteristic<uint8_t> roll(0xA100, &readValue, GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY);
ReadOnlyGattCharacteristic<uint8_t> pitch(0xA110, &readValue, GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY);
ReadOnlyGattCharacteristic<uint8_t> yaw(0xA111, &readValue, GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY);

static uint8_t writeValue = 0;
//WriteOnlyArrayGattCharacteristic<uint8_t, sizeof(writeValue)> writeChar(writeCharUUID, writeValue);
WriteOnlyGattCharacteristic<uint8_t> wochar(writeCharUUID, &writeValue);

/* Set up custom service */
GattCharacteristic *characteristics[] = {&rochar, &wochar, &roll, &pitch, &yaw};
GattService        customService(customServiceUUID, characteristics, sizeof(characteristics) / sizeof(GattCharacteristic *));



Serial pc(USBTX, USBRX);
I2C i2c(p30, p7);
Adafruit_BNO055 bno = Adafruit_BNO055(-1, BNO055_ADDRESS_A, &i2c);

/*
 *  Restart advertising when phone app disconnects
*/
void disconnectionCallback(const Gap::DisconnectionCallbackParams_t *)
{
    BLE::Instance(BLE::DEFAULT_INSTANCE).gap().startAdvertising();
}

/*
 *  Handle writes to writeCharacteristic
*/
void writeCharCallback(const GattWriteCallbackParams *params)
{
    /* Check to see what characteristic was written, by handle */
    if(params->handle == wochar.getValueHandle()) {
        /* toggle LED if only 1 byte is written */
        if(params->len == 1) {
            led = params->data[0];
            (params->data[0] == 0x00) ? printf("led on\n\r") : printf("led off\n\r"); // print led toggle
        }
        /* Print the data if more than 1 byte is written */
        else {
            printf("Data received: length = %d, data = 0x",params->len);
            for(int x=0; x < params->len; x++) {
                printf("%x", params->data[x]);
            }
            printf("\n\r");
        }
    }
}


void change() {
  readValue = rand();
  ble.updateCharacteristicValue(rochar.getValueHandle(), &readValue, 1);

  printf("Before read\r\n");
  //imu::Vector<3> accel_ = sensor.getVector(Adafruit_BNO055::VECTOR_EULER);
  imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);

  /* Display the floating point data */
  pc.printf("X: %f Y: %f Z: %f\t\t", euler.x(), euler.y(), euler.z());
  ble.updateCharacteristicValue(roll.getValueHandle(), (uint8_t*)&euler.x(), 8);
  ble.updateCharacteristicValue(yaw.getValueHandle(), (uint8_t*)&euler.y(), 8);
  ble.updateCharacteristicValue(pitch.getValueHandle(), (uint8_t*)&euler.z(), 8);
  //printf("X: %lf, Y: %lf, Z: %lf\r\n", accel_.x(), accel_.y(), accel_.z());
  printf("After read\r\n");

  //uint8_t system, gyro, accel, mag = 0;
  //sensor.getCalibration(&system, &gyro, &accel, &mag);
  //pc.printf("CALIBRATION: Sys=%d, Gyro=%d, Accel=%d, Mag=%d\r\n", (int)(system), (int)(gyro), (int)(accel), (int)(mag));
  //wait_ms(BNO055_SAMPLERATE_DELAY_MS);

}

/*
 * Initialization callback
 */
void bleInitComplete(BLE::InitializationCompleteCallbackContext *params)
{
    BLE &ble          = params->ble;
    ble_error_t error = params->error;

    if (error != BLE_ERROR_NONE) {
        return;
    }

    ble.gap().onDisconnection(disconnectionCallback);
    ble.gattServer().onDataWritten(writeCharCallback);

    /* Setup advertising */
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::BREDR_NOT_SUPPORTED | GapAdvertisingData::LE_GENERAL_DISCOVERABLE); // BLE only, no classic BT
    ble.gap().setAdvertisingType(GapAdvertisingParams::ADV_CONNECTABLE_UNDIRECTED); // advertising type
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LOCAL_NAME, (uint8_t *)DEVICE_NAME, sizeof(DEVICE_NAME)); // add name
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LIST_16BIT_SERVICE_IDS, (uint8_t *)uuid16_list, sizeof(uuid16_list)); // UUID's broadcast in advertising packet
    ble.gap().setAdvertisingInterval(100); // 100ms.

    /* Add our custom service */
    ble.addService(customService);

    /* Start advertising */
    ble.gap().startAdvertising();
}


void loop();

/**************************************************************************/
/*
    Arduino setup function (automatically called at startup)
*/
/**************************************************************************/
int main()
{
  BLE& ble = BLE::Instance(BLE::DEFAULT_INSTANCE);
  ble.init(bleInitComplete);

  while (!ble.hasInitialized()) {}

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
  Ticker t;
  t.attach(&change, 1);
  pc.printf("Calibration status values: 0=uncalibrated, 3=fully calibrated\r\n");

  while(true)
  {
    ble.waitForEvent(); /* Save power */
  }
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
