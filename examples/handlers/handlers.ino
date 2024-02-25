#include <Arduino.h>

#define AP_SSID "Alex"
#define AP_PASS "alexpass"

#include <GyverHub.h>
GyverHub hub;

// билдер
void build(gh::Builder& b) {
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
#endif

    hub.config(F("MyDevices"), F("ESP"));
    hub.onBuild(build);
    hub.begin();

    // обработчик времени из клиента
    hub.onUnix([](uint32_t stamp) {
        Serial.println(stamp);
    });

    // обработчик текста из консоли
    hub.onCLI([](String str) {
        Serial.println(str);
        hub.sendCLI(str + "!");
    });

    // сборщик инфо
    hub.onInfo([](gh::Info& info) {
        switch (info.type) {
            case gh::Info::Type::Version:
                info.add("ur mom", "v3.14");
                break;
            case gh::Info::Type::Network:
                info.add(F("5G"), "50%");
                break;
            case gh::Info::Type::Memory:
                info.add(F("SD"), "10 GB");
                info.add(F("Int"), 100500);
                break;
            case gh::Info::Type::System:
                info.add(F("Battery"), 3.63, 2);
                break;
        }
    });

    // обработчик запроса клиента
    hub.onRequest([](gh::Request& req) -> bool {
        Serial.print("Request: ");
        Serial.print(gh::readConnection(req.client.connection()));
        Serial.print(',');
        Serial.print(req.client.id);
        Serial.print(',');
        Serial.print(gh::readCMD(req.cmd));
        Serial.print(',');
        Serial.print(req.name);
        Serial.print(',');
        Serial.print(req.value);
        Serial.println();
        return 1;  // разрешить
    });

#ifdef GH_ESP_BUILD
    // обработчик перезагрузки
    hub.onReboot([](gh::Reboot r) {
        Serial.println(gh::readReboot(r));
    });

    // обработчик скачивания файла
    hub.onFetch([](gh::Fetcher& f) {
        Serial.print("Fetch: ");
        Serial.println(f.path);
        Serial.println(f.start);
    });

    // обработчик загрузки файла
    hub.onUpload([](String& path) {
        Serial.print("Uploaded: ");
        Serial.println(path);
    });
#endif
}

void loop() {
    hub.tick();
}