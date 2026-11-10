#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <cstring>
#include "secrets.h"

namespace {
struct Message {
    uint32_t counter;
};
portMUX_TYPE receiveLock = portMUX_INITIALIZER_UNLOCKED;
Message latestMessage{};
bool messageReady = false;

// Runs in the Wi-Fi task: validate, copy, and return without Serial/blocking work.
void onReceive(const esp_now_recv_info_t* info, const uint8_t* incomingData, int length) {
    if (info == nullptr || incomingData == nullptr || length != sizeof(Message)) {
        return;
    }
    Message received{};
    std::memcpy(&received, incomingData, sizeof(received));
    portENTER_CRITICAL(&receiveLock);
    latestMessage = received;
    messageReady = true;
    portEXIT_CRITICAL(&receiveLock);
}
}

void setup() {
    Serial.begin(115200);
    if (secrets::ESPNOW_CHANNEL < 1 || secrets::ESPNOW_CHANNEL > 13) {
        Serial.println("Invalid ESP-NOW channel in include/secrets.h.");
        return;
    }
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    if (esp_wifi_set_channel(secrets::ESPNOW_CHANNEL, WIFI_SECOND_CHAN_NONE) != ESP_OK ||
        esp_now_init() != ESP_OK || esp_now_register_recv_cb(onReceive) != ESP_OK) {
        Serial.println("Could not initialize the ESP-NOW receiver.");
        return;
    }
    Serial.println("ESP-NOW receiver ready.");
}

void loop() {
    Message received{};
    bool ready = false;
    portENTER_CRITICAL(&receiveLock);
    if (messageReady) {
        received = latestMessage;
        messageReady = false;
        ready = true;
    }
    portEXIT_CRITICAL(&receiveLock);
    if (ready) {
        Serial.print("Counter: ");
        Serial.println(received.counter);
    }
    delay(1);
}
