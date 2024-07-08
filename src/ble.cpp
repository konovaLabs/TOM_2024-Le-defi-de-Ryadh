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
BLECharacteristic *pledRGBValue = NULL;
bool deviceConnected = false;
ConnectionCallback connectionCallback = nullptr;

class MyServerCallbacks : public BLEServerCallbacks
{
    void onConnect(BLEServer *pServer)
    {
        deviceConnected = true;
        Serial.println("Client connected");
        if (connectionCallback)
        {
            connectionCallback(deviceConnected);
        }
    }

    void onDisconnect(BLEServer *pServer)
    {
        deviceConnected = false;
        Serial.println("Client disconnected");
        if (connectionCallback)
        {
            connectionCallback(deviceConnected);
        }
        BLEDevice::startAdvertising();
    }
};

// Callbacks pour la caractéristique LED RGB
class LedRGBCallbacks : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *pCharacteristic)
    {
        uint8_t *data = pCharacteristic->getData();
        if (pCharacteristic->getLength() == 3)
        {
            leds_set_color(data[0], data[1], data[2]);
        }
    }
};

void ble_register_connection_callback(ConnectionCallback callback)
{
    connectionCallback = callback;
}

void ble_init(void)
{
    BLEDevice::init("La Roue");
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks()); // Set the server callback

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
        pWheelService = pServer->createService("02345678-1234-5678-1234-56789abcdef0");
        pWheelMeasurement = pWheelService->createCharacteristic("02345678-1234-5678-1234-56789abcdef1", BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_READ);
        pledRGBValue = pWheelService->createCharacteristic("02345678-1234-5678-1234-56789abcdef2", BLECharacteristic::PROPERTY_WRITE);
        pAdvertising->addServiceUUID("02345678-1234-5678-1234-56789abcdef0");
    }
    else
    {
        Serial.println("Wheel Position: Right");
        pWheelService = pServer->createService("12345678-1234-5678-1234-56789abcdef0");
        pWheelMeasurement = pWheelService->createCharacteristic("12345678-1234-5678-1234-56789abcdef1", BLECharacteristic::PROPERTY_NOTIFY);
        pledRGBValue = pWheelService->createCharacteristic("12345678-1234-5678-1234-56789abcdef2", BLECharacteristic::PROPERTY_WRITE);
        pAdvertising->addServiceUUID("12345678-1234-5678-1234-56789abcdef0");
    }

    pledRGBValue->setCallbacks(new LedRGBCallbacks());

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