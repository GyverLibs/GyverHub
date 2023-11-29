// быстрый старт

#include <Arduino.h>

// логин-пароль WiFi
#define AP_SSID ""
#define AP_PASS ""

// подключаем библиотеку и настраиваем устройство
#include <GyverHub.h>
GyverHub hub("MyDevices", "ESP8266", "");
// иконки
// https://fontawesome.com/v5/cheatsheet/free/solid
// https://fontawesome.com/v5/search?o=r&m=free&s=solid

// переменные для интерфейса
GHbutton b2;
uint8_t sld_i;
float sld_f;

String inp_str;
char inp_cstr[11];  // строка на 10 символов (+1 нулевой)
int16_t inp_int;

// это наш билдер. Он будет вызываться библиотекой
// для сборки интерфейса, чтения значений и проч.
void build() {
    // сделаем интерфейс в виде стильных виджетов
    // BeginWidgets() начинает новую горизонтальную строку виджетов
    hub.BeginWidgets();

    // ширина следующих виджетов задаётся в %
    hub.WidgetSize(25);

    // это кнопка без настроек
    // Кнопка - активный компонент: вернёт 1 если нажата
    if (hub.Button()) Serial.println("Button 1 press");

    // ко второй кнопке подключим переменную b2
    // она будет true, пока кнопка удерживается. Опросим её ниже в loop
    // добавим название и цвет
    hub.Button(&b2, F("Button 2"), GH_RED);

    // сменим ширину на 50%
    hub.WidgetSize(50);

    // Label - пассивный компонент, он не возвращает результата действия
    // пусть он выводит актуальный millis() при загрузке страницы
    hub.Label(String(millis()), F("millis label"));

    // сделаем ещё один label с личным именем, к функции добавится _
    // ниже в loop будем отправлять обновления на его имя
    hub.Label_(F("lbl"), String(millis()), F("millis update"));

    // добавим слайдер шириной 100, во всю строку
    hub.WidgetSize(100);

    // к этому слайдеру не подключена переменная, но мы можем
    // вывести значение, которое отправено на его имя
    // Слайдер -активный компонент, вернёт true при изменении значения
    if (hub.Slider()) {
        Serial.print("Slider1 value: ");
        Serial.println(hub.action().valueInt());  // получим как целое число
    }

    hub.WidgetSize(50);
    // добавим ещё два слайдера и подключим переменные. Система будет сама обновлять в них значения!
    // нужно передать адрес переменной и её точный тип
    // у слайдера также есть минимум, максимум, шаг и цвет
    if (hub.Slider(&sld_i, GH_UINT8, F("Slider I"), 0, 10, 2)) {
        Serial.print("Slider2 value: ");
        Serial.println(sld_i);
        // переменная уже обновилась и новое значение доступно во всей области определения
    }
    if (hub.Slider(&sld_f, GH_FLOAT, F("Slider F"), 0.0, 1.0, 0.01, GH_PINK)) {
        Serial.print("Slider3 value: ");
        Serial.println(sld_f);
    }

    // добавим заголовок. Он также закроет предыдущую строку виджетов
    hub.Title(F("Input"));
    
    // кстати, установка ненулевого значения ширины также 
    // автоматически создаст виджет после заголовка
    hub.WidgetSize(50);

    // Компонент Input может работать переменнми любых типов и автоматически переписывать в них значение!
    // попробуем с String, char[] и int
    // для char[] нужно обязательно указать размер массива, чтобы не превысить его, что приведёт к сбою программы!!
    hub.Input(&inp_str, GH_STR, F("String input"));
    hub.Input(&inp_cstr, GH_CSTR, F("cstring input"), 10);      // <- 10 - макс. длина строки
    hub.Input(&inp_int, GH_INT16, F("int input"), 0, F("^\\d{4}$"));  // + regex на 4 цифры

    // что будет, если не создавать строку виджетов через BeginWidgets()?
    // закончим отрисовку виджетов
    hub.EndWidgets();
    
    // и выведем пару компонентов
    hub.Slider();
    hub.Input();
    hub.Switch();
    
    // они выведутся вертикальным списком!
}

void setup() {
    Serial.begin(115200);

    // если это ESP - подключаемся к WiFI
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

    // можно подключить обработчик веб-консоли
    // зайди в info/cli в приложении
    // или нажми ~ при открытом устройстве
    hub.onCLI([](String& s) {
        Serial.println(s);
        hub.print(s);       // отправить обратно
        // вдогонку отправим строку из Input inp_str
        hub.print(inp_str, GH_BLUE);
    });

    hub.onBuild(build);     // подключаем билдер
    hub.begin();            // запускаем систему
}


void loop() {
    hub.tick();  // обязательно тикаем тут

    if (b2) {
        static GHtimer tmr(500);
        if (tmr.ready()) Serial.println("hold b2");
    }

    if (b2.changed()) Serial.println(b2 ? "b2 press" : "b2 release");

    // обновим лейбл с именем lbl по таймеру на 1 секунду
    static GHtimer tmr(1000);
    if (tmr) hub.sendUpdate("lbl");
    // значение будет прочитано внутри билдера, а там у нас millis()
}