#include <Arduino.h>
#include "board.h"
#include "ble.h"
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

BLEServer *pServer = NULL;
BLEService *pCSCService = NULL;
BLEService *pWheelService = NULL;
BLECharacteristic *pWheelMeasurement = NULL;
BLECharacteristic *pCSCMeasurement = NULL;
BLECharacteristic *pCSCFeature = NULL;
BLECharacteristic *pSensorLocation = NULL;
BLECharacteristic *pSCControlPoint = NULL;

void ble_init(void)
{
    BLEDevice::init("La Roue");
    pServer = BLEDevice::createServer();

    /* Create Cycling Speed and Cadence service */
    pCSCService = pServer->createService("1816");
    /* Create CSC Measurement characteristic */
    pCSCMeasurement = pCSCService->createCharacteristic("2A5B", BLECharacteristic::PROPERTY_NOTIFY);
    /* Create CSC Feature characteristic */
    pCSCFeature = pCSCService->createCharacteristic("2A5C", BLECharacteristic::PROPERTY_INDICATE);
    /* Create Sensor Location characteristic */
    pSensorLocation = pCSCService->createCharacteristic("2A5D", BLECharacteristic::PROPERTY_READ);
    /* Create Sensor Location characteristic */
    pSCControlPoint = pCSCService->createCharacteristic("2A55", BLECharacteristic::PROPERTY_INDICATE);
    pCSCService->start();

    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID("1816");
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06); // functions that help with iPhone connections issue
    pAdvertising->setMinPreferred(0x12);

    /* Create Wheel Service */
    int state = digitalRead(GPIO_WHEEL_POSITION);
    if (state == 0)
    {
        Serial.println("Wheel Position: Left");
        pWheelService = pServer->createService("9b4833f9-a8e0-46e1-bdc6-2d2f69f12dc0");
        pWheelMeasurement = pWheelService->createCharacteristic("9b4833f9-a8e0-46e1-bdc6-2d2f69f12d00", BLECharacteristic::PROPERTY_NOTIFY);
        pAdvertising->addServiceUUID("9b4833f9-a8e0-46e1-bdc6-2d2f69f12dc0");
    }
    else
    {
        Serial.println("Wheel Position: Right");
        pWheelService = pServer->createService("9b4833f9-a8e0-46e1-bdc6-2d2f69f12dc1");
        pWheelMeasurement = pWheelService->createCharacteristic("9b4833f9-a8e0-46e1-bdc6-2d2f69f12d11", BLECharacteristic::PROPERTY_NOTIFY);
        pAdvertising->addServiceUUID("9b4833f9-a8e0-46e1-bdc6-2d2f69f12dc1");
    }

    pWheelService->start();

    BLEDevice::startAdvertising();
}

void ble_update_send_csc_measurement(csc_measurement_t *csc_measurement)
{
    uint8_t data_buf[7];

    // The Cycle Measurement Characteristic data is defined here:
    // https://www.bluetooth.com/wp-content/uploads/Sitecore-Media-Library/Gatt/Xml/Characteristics/org.bluetooth.characteristic.csc_measurement.xml
    // Frist byte is flags. Wheel Revolution Data Present (0 = false, 1 = true) = 1, Crank Revolution Data Present (0 = false, 1 = true), so the flags are 0x03 (binary 11 converted to HEX).
    // buf[0]=0x03;

    data_buf[0] = 0x01; // Wheel Revolution Data Present (0 = false, 1 = true) = 1

    // Setting values for cycling measures (from the Characteristic File)
    // Cumulative Wheel Revolutions (unitless)
    // Last Wheel Event Time (Unit has a resolution of 1/1024s)
    memcpy(&data_buf[1], &csc_measurement->cum_wheel_rev, 4);
    memcpy(&data_buf[5], &csc_measurement->last_wheel_event, 2);

    pCSCMeasurement->setValue(data_buf, 7);
    pCSCMeasurement->notify();

    pWheelMeasurement->setValue(data_buf, 7);
    pWheelMeasurement->notify();
}