// JOY DEMO

// WiFi
#define AP_SSID ""
#define AP_PASS ""

#include <Arduino.h>
#include <GyverHub.h>
GyverHub hub("MyDevices", "ESP8266", "");

GHpos pos2;

void build() {
    hub.BeginWidgets();
    hub.WidgetSize(50);

    // первый обработаем в билдере
    GHpos pos1;
    if (hub.Joystick(&pos1)) {
        Serial.println(pos1.x);
        Serial.println(pos1.y);
    }

    // второй в loop
    hub.Joystick(&pos2, 0, 1, F("my joy"), GH_RED);
}

void setup() {
    Serial.begin(115200);

#ifdef GH_ESP_BUILD
    WiFi.mode(WIFI_STA);
    WiFi.begin(AP_SSID, AP_PASS);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println();
    Serial.println(WiFi.localIP());

    hub.setupMQTT("test.mosquitto.org", 1883);
#endif

    hub.onBuild(build);
    hub.begin();
}

void loop() {
    hub.tick();

    if (pos2.changed()) {
        Serial.println("joy 2:");
        Serial.println(pos2.x);
        Serial.println(pos2.y);
    }
}