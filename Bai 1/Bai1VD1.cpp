#include <Arduino.h>
#include <Wifi.h>

const char* ssid = "Nhom 6";
const char* password = "2444666666";

void setup() {
  Serial.begin(115200);
  WiFi.softAP(ssid, password);
  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());
}

void loop() {
    
}