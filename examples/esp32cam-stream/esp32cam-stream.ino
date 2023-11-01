// HTTPD STREAM DEMO

#define AP_SSID ""
#define AP_PASS ""
#include <Arduino.h>

#include "camera.h"
#include "camera_stream.h"

#include <GyverHub.h>
GyverHub hub("MyDevices", "ESP8266", "");

void build() {
    hub.BeginWidgets();

    hub.WidgetSize(100);
    hub.Stream();

    /*hub.WidgetSize(50);

    static bool sw = 0;
    if (hub.Switch(&sw)) {
        if (sw) cam_stream_begin();
        else cam_stream_end();
    }*/
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
#endif

    hub.onBuild(build);
    hub.begin();

    cam_init();
    cam_stream_begin();
}

void loop() {
    hub.tick();
}
