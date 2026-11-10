#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>

namespace {
constexpr char SERVICE_UUID[] = "4fafc201-1fb5-459e-8fcc-c5c9c331914b";
constexpr char CHARACTERISTIC_UUID[] = "beb5483e-36e1-4688-b7f5-ea07361b26a8";
BLEServer* bleServer = nullptr;
}

void setup() {
    Serial.begin(115200);
    BLEDevice::init("ESP32-C3 BLE Demo");
    bleServer = BLEDevice::createServer();
    bleServer->advertiseOnDisconnect(true);
    BLEService* service = bleServer->createService(SERVICE_UUID);
    BLECharacteristic* characteristic = service->createCharacteristic(
        CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
    characteristic->setValue("Hello from ESP32-C3");
    service->start();
    BLEAdvertising* advertising = BLEDevice::getAdvertising();
    advertising->addServiceUUID(SERVICE_UUID);
    advertising->setScanResponse(true);
    advertising->start();
    Serial.println("BLE service ready: read or write the characteristic.");
}

void loop() {
    delay(20);
}
