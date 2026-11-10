#include <Arduino.h>
#include <PubSubClient.h>
#include <cstring>
#include "board_config.h"
#include "network_helpers.h"
#include "secrets.h"

namespace {
WiFiClient transport;
PubSubClient mqtt(transport);
char clientId[64]{};
uint32_t counter = 0;
uint32_t lastSend = 0;
uint32_t lastConnectAttempt = 0;
bool connectAttempted = false;
bool configurationValid = false;

void onMessage(char* topic, byte* payload, unsigned int length) {
    if (topic == nullptr || payload == nullptr || length != 1 ||
        std::strcmp(topic, secrets::MQTT_LED_TOPIC) != 0) {
        return;
    }
    if (payload[0] == '1' || payload[0] == '0') {
        digitalWrite(board_config::LED_PIN, payload[0] == '1' ? HIGH : LOW);
    }
}

bool connectMqtt() {
    const bool connected = secrets::MQTT_USERNAME[0] != '\0'
        ? mqtt.connect(clientId, secrets::MQTT_USERNAME, secrets::MQTT_PASSWORD)
        : mqtt.connect(clientId);
    if (!connected) {
        Serial.print("MQTT connect failed, state: ");
        Serial.println(mqtt.state());
        return false;
    }
    if (!mqtt.subscribe(secrets::MQTT_LED_TOPIC)) {
        Serial.println("MQTT subscription failed; retrying connection later.");
        mqtt.disconnect();
        return false;
    }
    Serial.println("MQTT connected and subscribed.");
    return true;
}
}

void setup() {
    Serial.begin(115200);
    pinMode(board_config::LED_PIN, OUTPUT);
    digitalWrite(board_config::LED_PIN, LOW);
    configurationValid = network_helpers::configured(secrets::MQTT_HOST) &&
        secrets::MQTT_PORT != 0 &&
        network_helpers::configured(secrets::MQTT_COUNTER_TOPIC) &&
        network_helpers::configured(secrets::MQTT_LED_TOPIC) &&
        std::strlen(secrets::MQTT_COUNTER_TOPIC) <= 160 &&
        std::strlen(secrets::MQTT_LED_TOPIC) <= 160 &&
        std::strpbrk(secrets::MQTT_COUNTER_TOPIC, "+#") == nullptr &&
        std::strpbrk(secrets::MQTT_LED_TOPIC, "+#") == nullptr &&
        std::strcmp(secrets::MQTT_COUNTER_TOPIC, secrets::MQTT_LED_TOPIC) != 0;
    if (!configurationValid) {
        Serial.println("Set MQTT configuration in include/secrets.h.");
        return;
    }
    const uint64_t deviceSuffix = ESP.getEfuseMac() & 0xFFFFFFFFFFFFULL;
    snprintf(clientId, sizeof(clientId), "%.40s-%012llx", secrets::MQTT_CLIENT_ID_PREFIX,
             static_cast<unsigned long long>(deviceSuffix));
    transport.setConnectionTimeout(1500);
    if (!mqtt.setBufferSize(768)) {
        Serial.println("Could not allocate MQTT packet buffer.");
        configurationValid = false;
        return;
    }
    mqtt.setServer(secrets::MQTT_HOST, secrets::MQTT_PORT);
    mqtt.setCallback(onMessage);
    mqtt.setSocketTimeout(3);
    mqtt.setKeepAlive(30);
}

void loop() {
    if (!configurationValid) {
        delay(10);
        return;
    }
    if (!network_helpers::maintainStation(secrets::MQTT_WIFI_SSID, secrets::MQTT_WIFI_PASSWORD)) {
        if (mqtt.connected()) {
            mqtt.disconnect();
        }
        delay(10);
        return;
    }
    const uint32_t now = millis();
    if (!mqtt.connected()) {
        if (!connectAttempted || now - lastConnectAttempt >= 5000U) {
            lastConnectAttempt = now;
            connectAttempted = true;
            connectMqtt();
        }
        delay(1);
        return;
    }
    mqtt.loop();
    if (Serial.available()) {
        const char command = static_cast<char>(Serial.read());
        if (command == '1' || command == '0') {
            const char payload[2] = {command, '\0'};
            if (!mqtt.publish(secrets::MQTT_LED_TOPIC, payload)) {
                Serial.println("MQTT command publish failed.");
            }
        }
    }
    if (now - lastSend >= 2000U) {
        lastSend = now;
        char payload[16]{};
        snprintf(payload, sizeof(payload), "%lu", static_cast<unsigned long>(counter));
        if (mqtt.publish(secrets::MQTT_COUNTER_TOPIC, payload)) {
            ++counter;
        } else {
            Serial.println("MQTT counter publish failed.");
        }
    }
    delay(1);
}
