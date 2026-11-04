#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
WebServer server(80);
const int LED_PIN = 2;
bool pressORbepressed = false;

void handleRoot() {
    String html = "<h1>ESP32 LED</h1>";
    html += "<p>LED: " + String(pressORbepressed ? "ON" : "OFF") + "</p>";
    html += "<a href='/on'><button>ON</button></a> ";
    html += "<a href='/off'><button>OFF</button></a>";
    server.send(200, "text/html", html);
}

void handleOn() {
    pressORbepressed = true;
    digitalWrite(LED_PIN, HIGH);

    server.sendHeader("Location", "/");
    server.send(303);
}

void handleOff() {
    pressORbepressed = false;
    digitalWrite(LED_PIN, LOW);

    server.sendHeader("Location", "/");
    server.send(303);
}

void setup() {
    Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);
    WiFi.softAP("Nhom 6", "12345678");
    Serial.print("AP IP: ");
    Serial.println(WiFi.softAPIP());
    server.on("/", handleRoot);
    server.on("/on", handleOn);
    server.on("/off", handleOff);
    server.begin();
}

void loop() {
    server.handleClient();
}