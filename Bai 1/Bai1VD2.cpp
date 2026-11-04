#include <Arduino.h>
#include <Wifi.h>

const char* ssid = "Nhom 6";
const char* password = "12345678";

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println(".");
  }
  Serial.println("\nIP: ");
  Serial.println(WiFi.localIP());
}

void loop() {
    
}