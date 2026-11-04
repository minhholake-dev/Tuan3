#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

WiFiClient wifi;
PubSubClient mqtt(wifi);

const char* ssid = "YOUR_WIFI";
const char* password = "YOUR_PASSWORD";
const char* broker = "test.mosquitto.org";
const char* clientId = "nhom06-kit1";  // Change for kit 2

const char* topic = "bica/tuan3/nhom-06";
const char* ledTopic = "bica/tuan3/nhom-06/led";

const int LED_PIN = 2;
int dem = 0;
unsigned long lastSend = 0;

void callback(char* topic, byte* payload, unsigned int length) {
    if (length == 1) {
        if (payload[0] == '1') digitalWrite(LED_PIN, HIGH);
        if (payload[0] == '0') digitalWrite(LED_PIN, LOW);
    }
}

void setup() {
    Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
    }

    mqtt.setServer(broker, 1883);
    mqtt.setCallback(callback);
}

void loop() {
    if (!mqtt.connected()) {
        if (mqtt.connect(clientId)) {
            mqtt.subscribe(ledTopic);
        } else {
            delay(1000);
            return;
        }
    }

    mqtt.loop();

    // Type 1 or 0 in Serial Monitor to control the LEDs.
    if (Serial.available()) {
        char command = Serial.read();

        if (command == '1') mqtt.publish(ledTopic, "1");
        if (command == '0') mqtt.publish(ledTopic, "0");
    }

    // Send the counter every 2 seconds.
    if (millis() - lastSend >= 2000) {
        lastSend = millis();
        mqtt.publish(topic, String(dem++).c_str());
    }
}