#pragma once
#include <Arduino.h>

namespace board_config {
// External LED + current-limiting resistor on QT Py ESP32-C3 GPIO4.
constexpr uint8_t LED_PIN = 4;
constexpr uint8_t WIFI_CHANNEL = 1;
}  // namespace board_config
