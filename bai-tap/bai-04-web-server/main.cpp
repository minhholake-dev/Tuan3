#include <Arduino.h>
#include <WebServer.h>
#include "board_config.h"
#include "network_helpers.h"
#include "secrets.h"

namespace {
WebServer server(80);
bool ledOn = false;
bool serverStarted = false;

void handleRoot() {
    String html = "<!doctype html><html lang='vi'><meta charset='utf-8'>";
    html += "<meta name='viewport' content='width=device-width,initial-scale=1'>";
    html += "<title>ESP32 LED</title><h1>ESP32 LED</h1><p>LED: ";
    html += ledOn ? "ON" : "OFF";
    html += "</p><form method='post' action='/on'><button>ON</button></form>";
    html += "<form method='post' action='/off'><button>OFF</button></form></html>";
    server.send(200, "text/html; charset=utf-8", html);
}

void setLed(bool value) {
    ledOn = value;
    digitalWrite(board_config::LED_PIN, ledOn ? HIGH : LOW);
    server.sendHeader("Location", "/");
    server.send(303, "text/plain", "");
}
}

void setup() {
    Serial.begin(115200);
    pinMode(board_config::LED_PIN, OUTPUT);
    digitalWrite(board_config::LED_PIN, LOW);
    if (!network_helpers::startAccessPoint(secrets::WEB_AP_SSID, secrets::WEB_AP_PASSWORD)) {
        return;
    }
    server.on("/", HTTP_GET, handleRoot);
    server.on("/on", HTTP_POST, []() { setLed(true); });
    server.on("/off", HTTP_POST, []() { setLed(false); });
    server.onNotFound([]() { server.send(404, "text/plain", "Not found"); });
    server.begin();
    serverStarted = true;
}

void loop() {
    if (serverStarted) {
        server.handleClient();
    }
    delay(1);
}
