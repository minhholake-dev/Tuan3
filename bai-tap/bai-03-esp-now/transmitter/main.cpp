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
bool ready = false;
uint32_t lastSend = 0;
Message message{};
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
        esp_now_init() != ESP_OK) {
        Serial.println("Could not initialize ESP-NOW.");
        return;
    }
    esp_now_peer_info_t peer{};
    std::memcpy(peer.peer_addr, secrets::ESPNOW_PEER_MAC, sizeof(peer.peer_addr));
    peer.channel = secrets::ESPNOW_CHANNEL;
    peer.ifidx = WIFI_IF_STA;
    peer.encrypt = false;
    if (esp_now_add_peer(&peer) != ESP_OK) {
        Serial.println("Could not add ESP-NOW peer.");
        return;
    }
    ready = true;
}

void loop() {
    const uint32_t now = millis();
    if (ready && now - lastSend >= 2000U) {
        lastSend = now;
        ++message.counter;
        const esp_err_t result = esp_now_send(
            secrets::ESPNOW_PEER_MAC,
            reinterpret_cast<const uint8_t*>(&message), sizeof(message));
        // ESP_OK means queued for transmission, not confirmed delivery.
        Serial.println(result == ESP_OK ? "ESP-NOW packet queued." : "ESP-NOW send failed.");
    }
    delay(1);
}
