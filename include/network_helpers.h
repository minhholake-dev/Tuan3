#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <cstring>
#include "board_config.h"

namespace network_helpers {
inline bool configured(const char* value) {
    return value != nullptr && value[0] != '\0' &&
           std::strstr(value, "YOUR_") == nullptr &&
           std::strstr(value, "CHANGE_ME") == nullptr;
}

inline bool startAccessPoint(const char* ssid, const char* password) {
    const size_t ssidLength = ssid == nullptr ? 0 : std::strlen(ssid);
    const size_t passwordLength = password == nullptr ? 0 : std::strlen(password);
    if (!configured(ssid) || !configured(password) || ssidLength > 32 ||
        passwordLength < 8 || passwordLength > 63) {
        Serial.println("Invalid AP configuration in include/secrets.h; AP disabled.");
        return false;
    }
    if (!WiFi.mode(WIFI_AP) ||
        !WiFi.softAP(ssid, password, board_config::WIFI_CHANNEL)) {
        Serial.println("Could not start the protected access point.");
        return false;
    }
    Serial.print("AP IP: ");
    Serial.println(WiFi.softAPIP());
    return true;
}

// Call from loop(): connection attempts are spaced out, with no wait loop.
inline bool maintainStation(const char* ssid, const char* password) {
    static bool attempted = false;
    static bool reportedConnected = false;
    static bool reportedMissing = false;
    static uint32_t lastAttempt = 0;
    if (!configured(ssid)) {
        if (!reportedMissing) {
            Serial.println("Set Wi-Fi configuration in include/secrets.h.");
            reportedMissing = true;
        }
        return false;
    }
    if (WiFi.status() == WL_CONNECTED) {
        if (!reportedConnected) {
            Serial.print("Station IP: ");
            Serial.println(WiFi.localIP());
            reportedConnected = true;
        }
        return true;
    }
    reportedConnected = false;
    const uint32_t now = millis();
    if (!attempted || now - lastAttempt >= 15000U) {
        WiFi.persistent(false);
        WiFi.mode(WIFI_STA);
        WiFi.setAutoReconnect(true);
        WiFi.begin(ssid, password);
        lastAttempt = now;
        attempted = true;
        Serial.println("Connecting to Wi-Fi...");
    }
    return false;
}
}  // namespace network_helpers
