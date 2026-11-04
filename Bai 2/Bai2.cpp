#include<Arduino.h>
#include<BLEDevice.h>
#include<BLEServer.h>

#define SV_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CH_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

void setup() {
  Serial.begin(115200);
  BLEDevice::init("Nhom 6 Punk");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SV_UUID);
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CH_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
  pCharacteristic->setValue("Hello World, I am 25119152. Please don't write anything on me.");
  pService->start();
  BLEDevice::startAdvertising();
}

void loop() {
  delay(2000);
}