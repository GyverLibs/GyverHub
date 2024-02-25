// пример с хранением данных в базе Pairs
#include <Arduino.h>

#define AP_SSID "Alex"
#define AP_PASS "alexpass"

#include <GyverHub.h>
GyverHub hub;

#include <PairsFile.h>
// pairsfile - автоматически сохраняет базу данных в файл
PairsFile data(&GH_FS, "/data.dat", 3000);
// Pairs data;  // есть без привязки к файлу

// билдер
void build(gh::Builder& b) {
    // для привязки к базе данных достаточно сделать именованный виджет
    // и передать базу данных по адресу. Данные будут сами писаться и читаться по ключу
    b.Input_("input", &data);
    // Это всё!!

    // ещё парочку
    {
        gh::Row r(b);
        b.Slider_("slider", &data);
        b.Spinner_("spinner", &data);
        b.Switch_("switch", &data);
    }

    // выведем содержимое базы данных как текст
    b.Text_("pairs", data);

    // обновить текст при действиях на странице
    if (b.changed()) hub.update("pairs").value(data);
}

void setup() {
    Serial.begin(115200);

#ifdef GH_ESP_BUILD
    // подключение к роутеру
    WiFi.mode(WIFI_STA);
    WiFi.begin(AP_SSID, AP_PASS);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println();
    Serial.println(WiFi.localIP());

    hub.mqtt.config("test.mosquitto.org", 1883);  // + MQTT

    // ИЛИ

    // режим точки доступа
    // WiFi.mode(WIFI_AP);
    // WiFi.softAP("My Hub");
    // Serial.println(WiFi.softAPIP());
#endif

    hub.config(F("MyDevices"), F("ESP"));
    hub.onBuild(build);
    hub.begin();

    // запустить и прочитать базу из файла
    data.begin();
}

void loop() {
    hub.tick();

    // файл сам обновится по таймауту
    data.tick();
}