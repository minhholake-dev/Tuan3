#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>

uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

typedef struct struct_message {
    int counter;
   
} struct_message;

struct_message callData;

void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    esp_now_init();
    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, broadcastAddress, 6);
    esp_now_add_peer(&peerInfo);

void loop() {
    callData.counter++;
    esp_now_send(confirmationAddress, (uint8_t *) &callData, sizeof(callData));
    delay(2000);
}
