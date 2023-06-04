// FULL DEMO

// WiFi
#define AP_SSID ""
#define AP_PASS ""

// ====== НАСТРОЙКИ КОМПИЛЯЦИИ ======
#define ATOMIC_FS_UPDATE  // OTA обновление GZIP файлом
//#define GH_ASYNC          // использовать ASYNC библиотеки

// включить сайт в память программы (не нужно загружать файлы в память)
//#define GH_INCLUDE_PORTAL

// отключение модулей
//#define GH_NO_PORTAL    // открытие сайта из памяти esp
//#define GH_NO_WS        // WebSocket
//#define GH_NO_MQTT      // MQTT
//#define GH_NO_FS        // работа с файлами (включая ОТА!)
//#define GH_NO_OTA       // ОТА файлом с приложения
//#define GH_NO_OTA_URL   // ОТА по URL
// =================================

#include <Arduino.h>
#include <GyverHUB.h>
GyverHUB hub("MyDevices", "ESP8266", "");
// иконки
// https://fontawesome.com/v5/cheatsheet/free/solid
// https://fontawesome.com/v5/search?o=r&m=free&s=solid

uint8_t tab = 0;
bool b1, b2;
String label_s;

String inp_str;
char inp_cstr[20];
int16_t inp_int;
float inp_float;
String pass;

int16_t sld = 20;
float sld_f = 20;

int16_t spin;
float spin_f;

bool sw;
GHcolor col;

Stamp gdate(2023, 3, 10);
Stamp gtime(12, 30, 0);
Stamp gdatetime(2025, 6, 20, 06, 06, 06);

uint8_t sel;
GHflags flags;

String disp_s;
String html_s;
GHlog dlog;
GHpos pos;

void build() {
  // hub.buildRead();  // билдер вызван для set или read операций

  GHbuild b = hub.getBuild();  // получить свойства текущего билда

  // Serial.println(GHreadBuild(b.type));    // тип билда
  // Serial.println(GHreadConn(b.hub.conn)); // соединение
  // Serial.println(b.hub.id);               // id клиента
  if (b.type == GH_BUILD_ACTION) {                  // это действие
    Serial.println(GHreadAction(b.action.type));  // тип действия
    Serial.println(b.action.name);                // имя компонента
    Serial.println(b.action.value);               // значение
    Serial.println();
  }

  label_s = random(100);
  disp_s = F("Hello!\nWorld");
  html_s = String(random(100)) + F("some custom\n<strong>Text</strong>");

  // BeginWidgets достаточно вызвать один раз, интерфейс будет собираться сам
  hub.BeginWidgets();
  hub.Tabs(F("tabs"), &tab, F("Tab 1,MY TAB,tab 2,tab 3,TAB 4"));
  // hub.EndWidgets();

  // hub.BeginWidgets();
  hub.WidgetSize(25);
  hub.Button(F("b1"), &b1, F("Button 1"));
  hub.Button(F("b2"), &b2, F("Button 2"), GH_RED);
  hub.ButtonIcon(F("play"), 0, F(""));
  hub.ButtonIcon(F("stop"), 0, F(""), GH_AQUA);
  // hub.EndWidgets();

  // hub.BeginWidgets();
  hub.WidgetSize(50);
  hub.Label(F("lbl"), label_s, F("Some label"));

  hub.WidgetSize(25);
  hub.LED(F("myled"), 0, F("Status"));
  hub.LED(F("myicn"), 0, F("Icon"), F(""));
  // hub.EndWidgets();

  // INPUTS
  hub.Title(F("Inputs"));
  // hub.BeginWidgets();
  hub.WidgetSize(50);
  hub.Input(F("inp_s"), &inp_str, GH_STR, F("String input"), 0, GH_BLUE);
  hub.Input(F("inp_c"), &inp_cstr, GH_CSTR, F("cstring input"), 10);
  hub.Input(F("inp_i"), &inp_int, GH_INT16, F("int input"));
  hub.Input(F("inp_f"), &inp_float, GH_FLOAT, F("float input"));

  hub.Pass(F("pass"), &pass, GH_STR, F("Pass input"), 0, GH_RED);
  // hub.EndWidgets();

  // hub.BeginWidgets();
  hub.WidgetSize(100);
  hub.Slider(F("sld1"), &sld, GH_INT16, F("Slider"));
  hub.Slider(F("sld2"), &sld_f, GH_FLOAT, F("Slider F"), 10, 90, 0.5, GH_PINK);
  // hub.EndWidgets();

  // hub.BeginWidgets();
  hub.WidgetSize(50);
  hub.Gauge(F("ga1"), random(-5, 30), F("°C"), F("Temp"), -5, 30, 0.1, GH_RED);
  if (hub.Joy("joy", &pos)) {
    Serial.println("joy:");
    Serial.println(pos.x());
    Serial.println(pos.y());
  }
  // hub.EndWidgets();

  // hub.BeginWidgets();
  hub.WidgetSize(50);
  hub.Spinner(F("spin"), &spin, GH_INT16, F("Spinner"));
  hub.Spinner(F("spinf"), &spin_f, GH_FLOAT, F("Spinner F"), 0, 10, 0.5);
  // hub.EndWidgets();

  // hub.BeginWidgets();
  hub.WidgetSize(25);
  hub.Switch(F("sw"), &sw, F("My switch"));
  hub.SwitchIcon(F("sw_i"), &sw, F("My switch i"), F(""), GH_BLUE);
  hub.SwitchText(F("sw_t"), &sw, F("My switch t"), F("ON"), GH_VIOLET);
  hub.Color(F("color"), &col, F("Color"));
  // hub.EndWidgets();

  // hub.BeginWidgets();
  hub.WidgetSize(50);
  hub.Date(F("date"), &gdate, F("Date select"), GH_RED);
  hub.Time(F("time"), &gtime, F("Time select"), GH_YELLOW);
  // hub.EndWidgets();

  // hub.BeginWidgets();
  hub.WidgetSize(100);
  hub.DateTime(F("datetime"), &gdatetime, F("Date time"));
  // hub.EndWidgets();

  // hub.BeginWidgets();
  hub.WidgetSize(50);
  hub.Select(F("sel"), &sel, F("kek,puk,lol"), F("List picker"));
  hub.Flags(F("flags"), &flags, F("mode 1,flag,test"), F("My flags"), GH_AQUA);
  // hub.EndWidgets();

  // hub.BeginWidgets();
  hub.Display(F("disp"), disp_s, F(""), GH_BLUE);
  hub.HTML(F("text"), html_s);
  // hub.EndWidgets();

  // hub.BeginWidgets();
  hub.WidgetSize(100);
  hub.Log(F("log"), &dlog);
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

  // обработчик билда
  hub.onBuild(build);

  // обработчик текста из веб-консоли
  hub.onCLI([](String & s) {
    Serial.println(s);
    hub.print(s);   // отправить эхо
    // hub.print(s, GH_RED);
  });

  // обработчик статуса для отладки
  hub.onEvent([](GHevent_t event, GHconn_t conn) {
    // GHreadXXXX берёт текстовое описание статуса из PROGMEM
    Serial.print(GHreadConn(conn));
    Serial.print(": ");
    Serial.println(GHreadEvent(event));
  });

  // вывести причину перезагрузки
  hub.onReboot([](GHreason_t r) {
    Serial.println(GHreadReason(r));
  });

  hub.begin();    // запустить
  // hub.setVersion("v1.1");      // установить версию (отображается в info)
  // hub.setPIN(1234);            // установить пин-код
  // hub.sendGetAuto(true);       //автоматически отправлять новое состояние на get-топик

  dlog.begin();           // лог тоже запустить
}

void loop() {
  // тикер. Вызывать в loop
  hub.tick();

  // true если кнопка удерживается. Выводим асинхронно
  if (b1) {
    static GHtimer tmr(500);
    if (tmr.ready()) Serial.println("hold b1");
  }
  if (b2) {
    static GHtimer tmr(500);
    if (tmr.ready()) Serial.println("hold b2");
  }

  static GHtimer tmr(3000);
  if (tmr.ready()) {                                  // таймер на 3 секунды
    hub.sendUpdate("ga1", String(random(-5, 30)));
    // if (hub.focused()) Serial.println("focus");  // проверяем, смотрит ли кто на нас
    // hub.print(String("hello #") + millis());     // печатать в веб-консоль
    // hub.sendPush("Hello!");                      // отправить пуш-уведомление

    // hub.sendUpdate("lbl,text");                  // отправить апдейт, получить значения из build
    // hub.sendUpdate("lbl", String(random(100)));  // отправить апдейт

    // hub.sendGet("lbl,text");                     // отправить значения на get топик, получить значения из build
    // hub.sendGet("lbl", String(random(100)));     // отправить значение на get топик
  }
}
