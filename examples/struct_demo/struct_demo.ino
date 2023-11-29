// DATA STRUCT

// WiFi
#define AP_SSID ""
#define AP_PASS ""

#include <Arduino.h>
#include <GyverHub.h>
#include <Stamp.h>
GyverHub hub("MyDevices", "ESP8266", "");

struct Data {
    bool sw;
    char inp[10];
    uint16_t sld;
    float spin;
    Stamp date;
    GHcolor col;
    GHflags flags;
};

Data data;

void build() {
    // Задача - получить сигнал о том, что было изменение и
    // нужно сохранить данные в память (EEPROM, файл итд)

    // ================= СПОСОБ 1 =================
    GHbuild b = hub.getBuild();

    hub.Switch(&data.sw, 0);
    hub.Input(&data.inp, GH_CSTR, 0, 10);
    hub.Slider(&data.sld, GH_INT16);
    hub.Spinner(&data.spin, GH_FLOAT);
    hub.Date(&data.date);
    hub.Color(&data.col);
    hub.Flags(&data.flags);

    // было действие установки (ЛЮБОЕ НА СТРАНИЦЕ)
    if (b.type == GH_BUILD_ACTION) {
        // сохраняем структуру data
    }

    // ================= СПОСОБ 2 =================
    bool flag = 0;
    flag |= hub.Switch(&data.sw, 0);
    flag |= hub.Input(&data.inp, GH_CSTR, 0, 10);
    flag |= hub.Slider(&data.sld, GH_INT16);
    flag |= hub.Spinner(&data.spin, GH_FLOAT);
    flag |= hub.Date(&data.date);
    flag |= hub.Color(&data.col);
    flag |= hub.Flags(&data.flags);

    // если хоть один из компонентов кликнут, flag будет 1
    // в отличие от первого способа, можно гибко разбивать компоненты
    // на группы, вводя несколько флагов для разных структур
    if (flag) {
        // сохраняем структуру data
    }
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
}