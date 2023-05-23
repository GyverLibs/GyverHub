// быстрый старт

#include <Arduino.h>

// логин-пароль WiFi
#define AP_SSID ""
#define AP_PASS ""

// подключаем библиотеку и настраиваем девайс
#include <GyverHUB.h>
GyverHUB hub("MyDevices", "ESP8266", "");
// иконки
// https://fontawesome.com/v5/cheatsheet/free/solid
// https://fontawesome.com/v5/search?o=r&m=free&s=solid

// переменные для интерфейса
bool b2;
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

    // ширина следующего виджета задаётся в %
    hub.WidgetSize(25);

    // это кнопка, которая имеет только имя
    // Кнопка - активный компонент: вернёт 1 если нажата
    if (hub.Button(F("b1"))) Serial.println("b1 press");

    // ко второй кнопке подключим переменную b2
    // она будет true, пока кнопка удерживается. Опросим её ниже в loop
    // добавим название и цвет
    hub.Button(F("b2"), &b2, F("Button 2"), GH_RED);

    // добьём оставшееся место лейблом, ширины осталось 50%
    hub.WidgetSize(50);

    // Label - пассивный компонент, он не возвращает результата действия
    // пусть он выводит актуальный millis() при загрузке страницы
    hub.Label(F("lbl"), String(millis()), F("Some label"));

    // EndWidgets() завершает строку виджетов. В то же время система
    // сама завершит строку и начнёт новую, если следующий виджет не поместится в неё!
    // hub.EndWidgets();    // не будем вызывать

    // добавим слайдер шириной 100, во всю строку
    hub.WidgetSize(100);

    // к этому слайдеру не подключена переменная, но мы можем
    // вывести значение, которое отправено на его имя
    // Слайдер -активный компонент, вернёт true при изменении значения
    if (hub.Slider(F("sld1"))) {
        Serial.print("Slider1 value: ");
        Serial.println(hub.actionValue());
    }

    hub.WidgetSize(50);
    // добавим ещё два слайдера и подключим переменные. Система будет сама обновлять в них значения!
    // нужно передать адрес переменной и её точный тип
    // у слайдера также есть минимум, максимум, шаг и цвет
    if (hub.Slider(F("sld2"), &sld_i, GH_UINT8, F("Slider I"), 0, 10, 2)) {
        Serial.print("Slider2 value: ");
        Serial.println(sld_i);  // переменная уже обновилась и новое значение доступно во всей области определения
    }
    if (hub.Slider(F("sld3"), &sld_f, GH_FLOAT, F("Slider F"), 0.0, 1.0, 0.01, GH_PINK)) {
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
    hub.Input(F("inp_s"), &inp_str, GH_STR, F("String input"));
    hub.Input(F("inp_c"), &inp_cstr, GH_CSTR, F("cstring input"), 10);      // <- 10 - макс. длина строки
    hub.Input(F("inp_i"), &inp_int, GH_INT16, F("int input"), 0, GH_BLUE);  // 0 - лимит отключен. И добавим цвет

    // что будет, если не создавать строку виджетов через BeginWidgets()?
    // закончим отрисовку виджетов
    hub.EndWidgets();
    
    // и выведем пару компонентов
    hub.Slider(F("sld_n"));
    hub.Input(F("inp_n"));
    hub.Switch(F("sw_n"));
    
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

// функция поможет печатать в порт с задержкой,
// не блокируя выполнение программы
void asyncPrint(const char* str) {
    static GHtimer tmr(500);
    if (tmr.ready()) Serial.println(str);
}

void loop() {
    hub.tick();  // обязательно тикаем тут

    if (b2) asyncPrint("b2 hold");
}