// пример обновления виджетов
#include <Arduino.h>

#define AP_SSID "Alex"
#define AP_PASS "alexpass"

#include <GyverHub.h>
GyverHub hub;

/*
  1. Обновить можно любой параметр виджета, который он поддерживает (кроме size)
  2. Отправлять обновления из билдера можно только по действию
  3. Отправлять обновления из остальной программы можно в любое время

  Общий синтаксис:
  hub.update("имя").параметр(значение).параметр(значение)...
*/

void build(gh::Builder& b) {
    if (b.beginRow()) {
        // для обновления нужно знать имя виджета, поэтому задаём его
        // укажу стартовые параметры, например цвет и значение
        b.Input_("inp").size(3).value("hello").color(gh::Colors::Blue);

        // обновлять будем например по нажатию на кнопку. Кнопки для значения и цвета
        if (b.Button().size(1).label("value").click()) hub.update("inp").value(random(1000));
        if (b.Button().label("color").click()) hub.update("inp").color(gh::Colors::Red);

        // если вызвать обновление вот тут (не по действию) - оно не будет отправлено!
        // hub.update("inp").value(random(1000));  // это не будет работать
        b.endRow();
    }

    if (b.beginRow()) {
        b.Slider_("sld").size(3);

        // а тут обновим сразу несколько параметров
        if (b.Button().size(2).label("update").click()) {
            hub.update("sld").value(random(100)).color(gh::Colors::Pink).unit("°");

            // такие обновления доставляются всем подключенным клиентам. Чтобы отправить
            // только клиенту из билда - можно передать его в update
            // hub.update("sld", &b.build.client).value(random(100)).color(gh::Colors::Pink).unit("°");
        }
        b.endRow();
    }

    if (b.beginRow()) {
        b.Label_("lbl").size(3);

        // отправка всем клиентам и только текущему
        if (b.Button().label("all").size(1).click()) hub.update("lbl").value("all");
        if (b.Button().label("client").click()) hub.update("lbl", &b.build.client).value("client");
        b.endRow();
    }

    // также можно обновить страницу
    if (b.Button().label("refresh").click()) b.refresh();

    // этот виджет будем обновлять ниже в loop
    b.Title_("tit").value("Title");
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

    // в основной программе тоже можно отправлять обновления, но не рекомендуется делать это слишком часто.
    // В библиотеке предусмотрен удобный таймер, например сделаем обновления каждую секунду
    static gh::Timer tmr(1000);
    if (tmr) {
        hub.update("tit").value(millis());
    }

    // в библиотеке предусмотрены также пакетные обновления сразу для нескольких виджетов. Отправим по таймеру
    static gh::Timer tmr2(5000);
    if (tmr2) {
        // создаём обновление
        gh::Update upd(&hub);

        // синтаксис такой:
        // upd.update("name1").параметр(значение).параметр(значение)...
        // upd.update("name2").параметр(значение).параметр(значение)...
        // upd.update("name3").параметр(значение).параметр(значение)...
        // upd.send();

        // таких компонентов у нас нет, это просто пример
        upd.update("name1").value(random(100));
        upd.update("name2").color(gh::Colors::Red).label("kek");

        // отправляем
        upd.send();
    }
}