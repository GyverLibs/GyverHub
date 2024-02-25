// CANVAS DEMO
#include <Arduino.h>

#define AP_SSID "Alex"
#define AP_PASS "alexpass"

#include <GyverHub.h>
GyverHub hub;

gh::Pos pos2;  // обработка кликов по холсту для loop

void build(gh::Builder& b) {
    if (b.beginRow()) {
        // 0. ХОЛСТ
        // пустой холст без обработки кликов
        // будем обновлять в loop по имени
        b.Canvas_(F("cv0"), 300, 300);

        // 1. ХОЛСТ
        // холст с рисунком, без имени и обновлений
        gh::Canvas cv1;
        b.BeginCanvas(300, 300, &cv1);
        cv1.fill(0x00ff00);  // жёлтый
        cv1.noStroke();
        cv1.circle(150, 150, 50);  // круг в центре
        b.EndCanvas();

        b.endRow();
    }

    if (b.beginRow()) {
        // 2. ХОЛСТ
        // холст с именем и рисунком и подключенным обработчиком кликов
        gh::Canvas cv2;
        b.BeginCanvas_(F("cv2"), 200, 200, &cv2, &pos2);
        cv2.stroke(0xff0000);
        cv2.strokeWeight(5);
        cv2.line(0, 0, -1, -1);  // крест
        cv2.line(0, -1, -1, 0);
        b.EndCanvas();

        // 3. ХОЛСТ
        // пустой холст с обработкой кликов в билдере
        gh::Pos pos3;
        b.Canvas_(F("cv3"), 300, 300, nullptr, &pos3);

        // был клик
        if (pos3.changed()) {
            Serial.print("Canvas cv3: ");
            Serial.print(pos3.x);
            Serial.print(',');
            Serial.println(pos3.y);

            // рисуем кружок
            gh::CanvasUpdate cv3("cv3", &hub);
            cv3.fill(0xff0000);
            cv3.circle(pos3.x, pos3.y, 10);
            cv3.send();
        }

        b.endRow();
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

    hub.mqtt.config("test.mosquitto.org", 1883);
#endif

    hub.config(F("MyDevices"), F("ESP"), F(""));
    hub.onBuild(build);
    hub.begin();
}

void loop() {
    hub.tick();

    // 0. обновление холста cv0 по таймеру
    static gh::Timer tmr(500);
    if (tmr) {
        gh::CanvasUpdate cv0("cv0", &hub);
        cv0.noStroke();
        cv0.fill(gh::Color(random(255), random(255), random(255)), random(100, 255));
        cv0.circle(random(0, 300), random(0, 300), random(5, 30));
        cv0.send();
    }

    // 2. был клик по холсту cv2
    if (pos2.changed()) {
        Serial.print("Canvas cv2: ");
        Serial.print(pos2.x);
        Serial.print(',');
        Serial.println(pos2.y);

        // рисуем кружок
        gh::CanvasUpdate cv2("cv2", &hub);
        cv2.circle(pos2.x, pos2.y, 10);
        cv2.send();
    }
}