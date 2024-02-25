#include <Arduino.h>

#define AP_SSID "Alex"
#define AP_PASS ""

#include <GyverHub.h>
GyverHub hub;

#define DYN_MAX 6

bool sws[DYN_MAX];
int16_t slds[DYN_MAX];
String inputs[DYN_MAX];
char chinputs[DYN_MAX][10];
uint8_t spin_am = 2;
uint8_t tab = 0;

// билдер
void build(gh::Builder& b) {
    if (b.beginRow()) {
        // делаем вкладки, перезагрузка по клику
        b.Tabs(&tab).text(F("Sliders;Switches;Inputs;Char input")).size(4);

        // спиннер с количеством, перезагрузка по клику
        b.Spinner(&spin_am).label(F("Amount")).range(0, DYN_MAX, 1).size(1);

        // перезагрузим если были действия
        if (b.changed()) b.refresh();
        b.endRow();
    }

    for (int i = 0; i < spin_am; i++) {
        b.beginRow();
        switch (tab) {
            case 0:
                b.Slider(&slds[i]).label(String("Slider #") + i);
                break;
            case 1:
                b.Switch(&sws[i]).label(String("Switch #") + i);
                break;
            case 2:
                b.Input(&inputs[i]).label(String("Input #") + i);
                break;
            case 3:
                b.Input(chinputs[i]).maxLen(9).label(String("Input #") + i);
                break;
        }
        b.endRow();
    }
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

    hub.mqtt.config("test.mosquitto.org", 1883);
#endif

    hub.config(F("MyDevices"), F("ESP"));
    hub.onBuild(build);
    hub.begin();
}

void loop() {
    hub.tick();
}