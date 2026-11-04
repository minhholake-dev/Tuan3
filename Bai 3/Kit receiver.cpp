#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>

uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

typedef struct struct_message {
    int counter;
   
} struct_message;

struct_message callData;

void onReceive(const uint8_t * mac, const uint8_t *incomingData, int len) {
    memcpy(&callData, incomingData, sizeof(callData));
    Serial.print("Counter: ");
    Serial.println(callData.counter);
}

void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    esp_now_init();
    esp_now_register_recv_cb(onReceive);

void loop() {

}
