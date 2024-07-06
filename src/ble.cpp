#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

void ble_init(void)
{
    BLEDevice::init("La Roue");
    BLEServer *pServer = BLEDevice::createServer();

    /* Create Cycling Speed and Cadence service */
    BLEService *pCSCService = pServer->createService("1816");
    /* Create CSC Measurement characteristic */
    BLECharacteristic *pCSCMeasurement =
        pCSCService->createCharacteristic("2A5B", BLECharacteristic::PROPERTY_NOTIFY);
    /* Create CSC Feature characteristic */
    BLECharacteristic *pCSCFeature =
        pCSCService->createCharacteristic("2A5C", BLECharacteristic::PROPERTY_INDICATE);
    /* Create Sensor Location characteristic */
    BLECharacteristic *pSensorLocation =
        pCSCService->createCharacteristic("2A5D", BLECharacteristic::PROPERTY_READ);
    /* Create Sensor Location characteristic */
    BLECharacteristic *pSCControlPoint =
        pCSCService->createCharacteristic("2A55", BLECharacteristic::PROPERTY_INDICATE);

    pCSCService->start();

    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID("1816");
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06); // functions that help with iPhone connections issue
    pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();
}