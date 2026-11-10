#include <Arduino.h>
#include "network_helpers.h"
#include "secrets.h"

void setup() {
    Serial.begin(115200);
}

void loop() {
    network_helpers::maintainStation(secrets::STATION_SSID, secrets::STATION_PASSWORD);
    delay(10);
}
