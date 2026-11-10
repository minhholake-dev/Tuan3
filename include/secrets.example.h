#pragma once
#include <stdint.h>

// Copy this file to secrets.h and fill in your local network settings.
// Keep secrets.h out of Git and shared archives.
namespace secrets {
constexpr char AP_SSID[] = "";
constexpr char AP_PASSWORD[] = "";  // 8-63 characters; an open AP is not allowed.
constexpr char STATION_SSID[] = "";
constexpr char STATION_PASSWORD[] = "";
constexpr char WEB_AP_SSID[] = "";
constexpr char WEB_AP_PASSWORD[] = "";
constexpr char MQTT_WIFI_SSID[] = "";
constexpr char MQTT_WIFI_PASSWORD[] = "";
constexpr char MQTT_HOST[] = "";
constexpr uint16_t MQTT_PORT = 1883;
constexpr char MQTT_USERNAME[] = "";
constexpr char MQTT_PASSWORD[] = "";
constexpr char MQTT_CLIENT_ID_PREFIX[] = "esp32-week03";
constexpr char MQTT_COUNTER_TOPIC[] = "";
constexpr char MQTT_LED_TOPIC[] = "";
constexpr uint8_t ESPNOW_CHANNEL = 1;
// Broadcast for the classroom demo; replace with the receiver's MAC for unicast.
constexpr uint8_t ESPNOW_PEER_MAC[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
}  // namespace secrets
