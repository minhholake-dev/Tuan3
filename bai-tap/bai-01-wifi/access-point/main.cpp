#include <Arduino.h>
#include "network_helpers.h"
#include "secrets.h"

void setup() {
    Serial.begin(115200);
    network_helpers::startAccessPoint(secrets::AP_SSID, secrets::AP_PASSWORD);
}

void loop() {
    delay(10);
}
