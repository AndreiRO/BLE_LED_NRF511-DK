/* mbed Microcontroller Library
 * Copyright (c) 2006-2013 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "mbed.h"
#include "ble/BLE.h"
//
#if 0
#define BMP180_TEST_FORMULA 1

DigitalOut alivenessLED(LED1, 0);
DigitalOut actuatedLED(LED2, 0);
Serial pc(USBTX, USBRX);
I2C i2c(p28, p27);
//BMP180 tap(p30, p7);

const static char     DEVICE_NAME[] = "LED";
static const uint16_t uuid16_list[] = {LEDService::LED_SERVICE_UUID};

LEDService *ledServicePtr;

Ticker ticker;

void disconnectionCallback(const Gap::DisconnectionCallbackParams_t *params)
{
    BLE::Instance().gap().startAdvertising();
}



void periodicCallback(void)
{
    alivenessLED = !alivenessLED;
    actuatedLED = !alivenessLED; /* Do blinky on LED1 to indicate system aliveness. */
//    tap.ReadData(&t, &p);
    //tap.ReadData();
    //pc.printf("Humidity: %f ", humidity.sensorRH());
    //pc.printf("Temperature: %f Pressure: %f\r\n", tap.GetTemperature(), tap.GetPressure());
    char b[2];
    b[0] = 0x2;
    b[1] = 0x0;
    pc.printf("New loop:\r\n");
    i2c.write(0xAA, b, 2);
}

/**
 * This callback allows the LEDService to receive updates to the ledState Characteristic.
 *
 * @param[in] params
 *     Information about the characterisitc being updated.
 */
void onDataWrittenCallback(const GattWriteCallbackParams *params) {
    if ((params->handle == ledServicePtr->getValueHandle()) && (params->len == 1)) {
        actuatedLED = *(params->data);
    }
    pc.printf("BLE: %d %s\r\n", params->handle, params->data);
}

void onDataReadCallback(const GattReadCallbackParams *params) {
    if ((params->handle == ledServicePtr->getValueHandle()) && (params->len == 1)) {

    }
    pc.printf("BLE: %d %s\r\n", params->handle, params->data);
}

/**
 * This function is called when the ble initialization process has failed
 */
void onBleInitError(BLE &ble, ble_error_t error)
{
    /* Initialization error handling should go here */
}

/**
 * Callback triggered when the ble initialization process has finished
 */
void bleInitComplete(BLE::InitializationCompleteCallbackContext *params)
{
    BLE&        ble   = params->ble;
    ble_error_t error = params->error;

    if (error != BLE_ERROR_NONE) {
        /* In case of error, forward the error handling to onBleInitError */
        onBleInitError(ble, error);
        return;
    }

    /* Ensure that it is the default instance of BLE */
    if(ble.getInstanceID() != BLE::DEFAULT_INSTANCE) {
        return;
    }

    ble.gap().onDisconnection(disconnectionCallback);
    ble.gattServer().onDataWritten(onDataWrittenCallback);
    ble.gattServer().onDataRead(onDataReadCallback);

    bool initialValueForLEDCharacteristic = false;
    ledServicePtr = new LEDService(ble, initialValueForLEDCharacteristic);

    /* setup advertising */
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::BREDR_NOT_SUPPORTED | GapAdvertisingData::LE_GENERAL_DISCOVERABLE);
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LIST_16BIT_SERVICE_IDS, (uint8_t *)uuid16_list, sizeof(uuid16_list));
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LOCAL_NAME, (uint8_t *)DEVICE_NAME, sizeof(DEVICE_NAME));
    ble.gap().setAdvertisingType(GapAdvertisingParams::ADV_CONNECTABLE_UNDIRECTED);
    ble.gap().setAdvertisingInterval(1000); /* 1000ms. */
    ble.gap().startAdvertising();
}

int main(void)
{
    ticker.attach(periodicCallback, 1); /* Blink LED every second */

    BLE &ble = BLE::Instance();
    ble.init(bleInitComplete);

//    pc.printf("Initialized: %d\r\n", tap.Initialize(100));

    /* SpinWait for initialization to complete. This is necessary because the
     * BLE object is used in the main loop below. */
    while (ble.hasInitialized()  == false) { /* spin loop */ }

    while (true) {
        ble.waitForEvent();
    }
}

#endif
#include "mbed.h"
#if 0

#include "Adafruit_BNO055.h"
#define BNO055_SAMPLERATE_DELAY_MS (100)
I2C i2c(p30, p7);
Adafruit_BNO055 sensor(-1, BNO055_ADDRESS_A, &i2c);

Serial pc(USBTX, USBRX);

int main() {
  wait(2);
  pc.baud(9600);
    if (sensor.begin())
    {
      pc.printf("Init successful\r\n");
    }
    else
    {
      pc.printf("EPIC fail\r\n");
      while(1);
    }

    wait(1);
    pc.printf("Temperatura: %d\r\n", sensor.getTemp());
    sensor.setExtCrystalUse(false);

    while (1) {
      //imu::Vector<3> accel = sensor.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
      imu::Vector<3> accel_ = sensor.getVector(Adafruit_BNO055::VECTOR_EULER);
      pc.printf("X: %lf, Y: %lf, Z: %lf\r\n", accel_.x(), accel_.y(), accel_.z());

      uint8_t system, gyro, accel, mag = 0;
      sensor.getCalibration(&system, &gyro, &accel, &mag);
      pc.printf("CALIBRATION: Sys=%d, Gyro=%d, Accel=%d, Mag=%d\r\n", (int)(system), (int)(gyro), (int)(accel), (int)(mag));
      wait_ms(BNO055_SAMPLERATE_DELAY_MS);
    }
}
#endif
#if 0
#include "BNO055.h"
Serial pc(USBTX, USBRX);
BNO055 imu(p30, p7);

int main()
{
  pc.baud(9600);
  printf("BNO055 Hello world\r\n");

  imu.reset();
  if (!imu.check())
  {
    printf("Epic fail\r\n");
    while(true);
  }

  printf("BNO055 found\r\n\r\n");
  printf("Chip          ID: %0z\r\n",imu.ID.id);
  printf("Accelerometer ID: %0z\r\n",imu.ID.accel);
  printf("Gyroscope     ID: %0z\r\n",imu.ID.gyro);
  printf("Magnetometer  ID: %0z\r\n\r\n",imu.ID.mag);
  printf("Firmware version v%d.%d\r\n",imu.ID.sw[0],imu.ID.sw[1]);
  printf("Bootloader version v%d\r\n\r\n",imu.ID.bootload);

  for (int i = 0; i<4; i++){
      printf("%0z.%0z.%0z.%0z\r\n",imu.ID.serial[i*4],imu.ID.serial[i*4+1],imu.ID.serial[i*4+2],imu.ID.serial[i*4+3]);
  }
  printf("\r\n");

  while (true)
  {
    imu.setmode(OPERATION_MODE_NDOF);
    imu.get_calib();
    imu.get_angles();
    printf("%0z %5.1d %5.1d %5.1d\r\n",(imu.calib +'0'),imu.euler.roll,imu.euler.pitch,imu.euler.yaw);
    wait(1.0);
  }
}
#endif

#include "Adafruit_BNO055.h"


DigitalOut led(LED1, 0);
DigitalOut success(LED2, 1);
DigitalOut error_(LED3, 1);
uint16_t customServiceUUID  = 0xA000;
uint16_t readCharUUID       = 0xA001;
uint16_t writeCharUUID      = 0xA002;


I2C i2c(p30, p7);
Adafruit_BNO055 sensor(-1, BNO055_ADDRESS_A, &i2c);


const static char     DEVICE_NAME[]        = "TEST"; // change this
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
        /* Update the readChar with the value of writeChar */
        //BLE::Instance(BLE::DEFAULT_INSTANCE).gattServer().write(rochar.getValueHandle(), params->data, params->len);
    }
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

BLE& ble = BLE::Instance(BLE::DEFAULT_INSTANCE);
void change() {
  success = !success;
  readValue = rand();
  ble.updateCharacteristicValue(rochar.getValueHandle(), &readValue, 1);
  printf("Before read\r\n");
  imu::Vector<3> accel_ = sensor.getVector(Adafruit_BNO055::VECTOR_EULER);
  //ble.updateCharacteristicValue(roll.getValueHandle(), (uint8_t*)&accel_.x(), 8);
  //ble.updateCharacteristicValue(yaw.getValueHandle(), (uint8_t*)&accel_.y(), 8);
  //ble.updateCharacteristicValue(pitch.getValueHandle(), (uint8_t*)&accel_.z(), 8);
  //printf("X: %lf, Y: %lf, Z: %lf\r\n", accel_.x(), accel_.y(), accel_.z());
  error_ = ! error_;

  printf("After read\r\n");

  //uint8_t system, gyro, accel, mag = 0;
  //sensor.getCalibration(&system, &gyro, &accel, &mag);
  //pc.printf("CALIBRATION: Sys=%d, Gyro=%d, Accel=%d, Mag=%d\r\n", (int)(system), (int)(gyro), (int)(accel), (int)(mag));
  //wait_ms(BNO055_SAMPLERATE_DELAY_MS);

}

/*
 *  Main loop
*/
int main(void)
{
    /* initialize stuff */
    printf("\n\r********* Starting Main Loop *********\n\r");
    wait(2);
    success = error_ = 1;

    if (sensor.begin())
    {
      printf("Successful init\n\r");
      success = 0;
    }
    else
    {
      printf("Epic fail\r\n");
      error_ = 0;
      while(1);
    }

    wait(1);
    sensor.getTemp();
    sensor.setExtCrystalUse(true);

    ble.init(bleInitComplete);



    /* SpinWait for initialization to complete. This is necessary because the
     * BLE object is used in the main loop below. */
    while (ble.hasInitialized()  == false) { /* spin loop */ }

    Ticker ticker;
    ticker.attach(&change,2);

    /* Infinite loop waiting for BLE interrupt events */
    while (true) {
        printf("waiting for event...\r\n");
        ble.waitForEvent(); /* Save power */
    }
}
/*

#include "Adafruit_BNO055.h"
#define BNO055_SAMPLERATE_DELAY_MS (100)
I2C i2c(p30, p7);
Adafruit_BNO055 sensor(-1, BNO055_ADDRESS_A, &i2c);

Serial pc(USBTX, USBRX);

int main() {
  wait(2);
  pc.baud(9600);
    if (sensor.begin())
    {
      pc.printf("Init successful\r\n");
    }
    else
    {
      pc.printf("EPIC fail\r\n");
      while(1);
    }

    wait(1);
    pc.printf("Temperatura: %d\r\n", sensor.getTemp());
    sensor.setExtCrystalUse(false);

    while (1) {
      //imu::Vector<3> accel = sensor.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
      imu::Vector<3> accel_ = sensor.getVector(Adafruit_BNO055::VECTOR_EULER);
      pc.printf("X: %lf, Y: %lf, Z: %lf\r\n", accel_.x(), accel_.y(), accel_.z());

      uint8_t system, gyro, accel, mag = 0;
      sensor.getCalibration(&system, &gyro, &accel, &mag);
      pc.printf("CALIBRATION: Sys=%d, Gyro=%d, Accel=%d, Mag=%d\r\n", (int)(system), (int)(gyro), (int)(accel), (int)(mag));
      wait_ms(BNO055_SAMPLERATE_DELAY_MS);
    }
*/
