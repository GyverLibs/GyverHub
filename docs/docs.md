# Документация GyverHUB beta
- **Эта страница** - список всех функций и методов библиотеки. Подробную информацию по использованию смотри на остальных страницах
- **Оглавление** ищи *справа* на полной версии сайта и *снизу* на мобильной
- **Примеры** есть [в папке с библиотекой](https://github.com/GyverLibs/GyverHUB/tree/main/examples)

[![Foo](https://img.shields.io/badge/ENGLISH-VERSION-blueviolet.svg?style=flat-square)](https://github-com.translate.goog/GyverLibs/GyverHUB/wiki?_x_tr_sl=ru&_x_tr_tl=en)

Примечание по типам данных:
- **String** - для краткости под этим типом подразумевается `const String&`
    - `func("текст в кавычках");`  - строки const char*
    - `func(F("строка в flash"));` - строки, сохранённые в Flash памяти через F() - `F("строка")`
    - `String s; func(s);`         - String строки, объявленные выше
    - `func(String(123) + 456);`   - String строки, объявленные внутри (не рекомендуется)
- **FSTR** - для краткости под этим типом подразумевается `const __FlashStringHelper*`
    - `func(F("строка в flash"));` - строки, сохранённые в Flash памяти через F() - `F("строка")`

## GyverHUB
<details>
<summary>Инициализация</summary>

```cpp
GyverHUB;
GyverHUB(char* prefix);                                         // + префикс сети
GyverHUB(char* prefix, char* name);                             // + имя в списке устройств
GyverHUB(char* prefix, char* name, char* icon);                 // + иконка в списке устройств
GyverHUB(char* prefix, char* name, char* icon, uint32_t id);    // + вручную задать ID устройства
// примечание: id нужно обязательно задавать для отличных от ESP платформ (для esp генерируется автоматически)
```
</details>

<details>
<summary>Система</summary>

```cpp
// =================== CONFIG ==================
void config(char* nprefix, char* nname, char* nicon, uint32_t nid); // Аналог конструктора
void setVersion(FSTR v);    // установить версию прошивки для отображения в Info
void begin();               // запустить
void end();                 // остановить
bool tick();                // тикер, вызывать в loop

// ================== MODULES =================
void modules.on(GHmodule_t mod);    // включить модуль
void modules.off(GHmodule_t mod);   // выключить модуль
bool modules.read(GHmodule_t mod);  // включен ли модуль

void modules.set(модули);           // настроить модули, разделитель |
// например modules.set(GH_MOD_LOCAL | GH_MOD_DOWNLOAD | GH_MOD_UPLOAD)
// модули, отвечающие за связь, нужно настраивать перед вызовом begin()!

// ==================== PIN ===================
void setPIN(uint32_t npin); // установить пин-код устройства (больше 1000, не может начинаться с 0)
uint32_t getPIN();          // прочитать пин-код

// =================== BUILD ==================
void onBuild(f);        // подключить сборщик интерфейса. Функция вида void f()
void refresh();         // обновить веб-интерфейс. Вызывать внутри обработчика build

// полные свойства
GHbuild getBuild();     // получить свойства текущего билда. Вызывать внутри обработчика build В САМОМ НАЧАЛЕ

// более простой доступ
bool buildRead();           // true - если билдер вызван для set или read операций
const char* actionName();    // вернёт имя компонента в текущем действии
const char* actionValue();   // вернёт значение компонента в текущем действии

// ================== STATUS ==================
void onStatus(f);       // подключить обработчик изменения статуса. Функция вида void f(GHstatus status)
bool running();         // вернёт true, если система запущена
bool focused();         // true - интерфейс устройства сейчас открыт на сайте или в приложении

// =================== CLI ===================
void onCLI(f);          // подключить обработчик входящих сообщений с веб-консоли. Функция вида void f(String& s)
void print(String s);   // отправить текст в веб-консоль
void print(String s, uint32_t color);    // + цвет

// =================== SEND ===================
void sendPush(String text);                     // отправить уведомление
void sendUpdate(String name, String value);     // отправить update вручную с указанием значения

// отправить update по имени компонента (значение будет прочитано в build)
// имена можно передать списком через запятую
void sendUpdate(String name);

// ================== MQTT ==================
// настроить MQTT (только TCP)
void setupMQTT(char* host, uint16_t port);
void setupMQTT(char* host, uint16_t port, char* login, const char* pass);
void setupMQTT(char* host, uint16_t port, char* login, const char* pass, uint8_t qos, bool retained);
// для игнорирования login/pass нужно установить 0

void turnOn();      // отправить MQTT LWT команду на включение
void turnOff();     // отправить MQTT LWT команду на выключение

void sendGetAuto(bool v);       // автоматически отправлять новое состояние на get-топик при изменении через set (умолч. false)
void sendGet(String name, String value);    // отправить имя-значение на get-топик (MQTT)

// отправить значение по имени компонента на get-топик (MQTT) (значение будет прочитано в build)
// имена можно передать списком через запятую
void sendGet(String name);

// ============== MANUAL BUILD ==============
void onManual(f);       // подключить обработчик запроса при ручном соединении. Функция вида void f(const String& s)
void parse(char* url, GH_MANUAL);               // парсить команду вида PREFIX/ID/HUB_ID/CMD/NAME=VALUE
void parse(char* url, char* value, GH_MANUAL);  // парсить команду вида PREFIX/ID/HUB_ID/CMD/NAME, value отдельно

// установить размер буфера строки для сборки интерфейса в режиме MANUAL
// 0 - интерфейс будет собран и отправлен цельной строкой, иначе пакет будет отправляться частями размером с буфер
void setBufferSize(uint16_t size);
```
</details>

<details>
<summary>Компиляция</summary>

Библиотека определяет, на какой платформе компилируется. Для ESP8266/ESP32 предусмотрен отдельный флаг, 
включюащий сетевые возможности в компиляцию. Для создания кросс-платформенного кода можно "прятать" 
код для ESPxx внутри условной конструкции

```cpp
#ifdef GH_ESP_BUILD
// этот код будет компилироваться только для esp
#endif
```

где `GH_ESP_BUILD` определяется библиотекой
</details>

<details>
<summary>Дефайны настроек</summary>

```cpp
// Вводятся до подключения библиотеки

#define ATOMIC_FS_UPDATE  // OTA обновление GZIP файлом
#define GH_ASYNC          // использовать ASYNC библиотеки

// полное отключение модулей из программы
#define GH_NO_SERIAL
#define GH_NO_LOCAL
#define GH_NO_MQTT
#define GH_NO_FS
#define GH_NO_INFO
#define GH_NO_OTA
```
</details>

## Конструктор
<details>
<summary>Компоненты</summary>

```cpp
```
</details>

<details>
<summary>Цвет</summary>

```cpp
GH_RED      // 0xcb2839
GH_ORANGE   // 0xd55f30
GH_YELLOW   // 0xd69d27
GH_GREEN    // 0x37A93C
GH_MINT     // 0x25b18f
GH_AQUA     // 0x2ba1cd
GH_BLUE     // 0x297bcd
GH_VIOLET   // 0x825ae7
GH_PINK     // 0xc8589a
```
</details>

## Вспомогательные классы
<details>
<summary>GHbuild</summary>

```cpp
GHbuild_t type;     // тип билда
GHhub hub;          // данные клиента
GHaction action;    // действие
```
</details>

<details>
<summary>GHhub</summary>

```cpp
GHconn_t conn;      // тип соединения
char id[9];         // id клиента
```
</details>

<details>
<summary>GHaction</summary>

```cpp
GHaction_t type;    // тип действия 
const char* name;   // имя компонента
const char* value;  // значение компонента
```
</details>

## Типы данных
<details>
<summary>GHdata_t (тип переменной)</summary>

```cpp
GH_NULL

GH_STR
GH_CSTR

GH_BOOL
GH_INT8
GH_UINT8
GH_INT16
GH_UINT16
GH_INT32
GH_UINT32

GH_FLOAT
GH_DOUBLE

GH_COLOR
GH_FLAGS
GH_STAMP
```
</details>

<details>
<summary>GHbuild_t (тип билда)</summary>

```cpp
GH_BUILD_NONE       // нет
GH_BUILD_ACTION     // действие GHaction_t
GH_BUILD_COUNT      // измерение размера пакета
GH_BUILD_READ       // чтение значения по имени (get, update)
GH_BUILD_UI         // сборка интерфейса для отправки
GH_BUILD_TG         // сборка для Telegram
```
</details>

<details>
<summary>GHaction_t (тип действия)</summary>

```cpp
GH_ACTION_NONE      // нет
GH_ACTION_SET       // установка значения
GH_ACTION_PRESS     // кнопка нажата
GH_ACTION_RELEASE   // кнопка отпущена
```
</details>

<details>
<summary>GHconn_t (тип подключения)</summary>

```cpp
GH_MANUAL
GH_SERIAL
GH_WS
GH_MQTT
GH_HTTP     // обнаружение HTTP
GH_SYSTEM   // системное сообщение
```
</details>

<details>
<summary>GHmodule_t (отключаемые модули)</summary>

```cpp
GH_MOD_MANUAL   // использовать MANUAL подключение
GH_MOD_SERIAL   // использовать SERIAL подключение
GH_MOD_LOCAL    // использовать LOCAL подключение
GH_MOD_MQTT     // использовать MQTT подключение
GH_MOD_INFO     // использовать вкладку инфо
GH_MOD_FSBR     // использовать вкладку менеджера файлов
GH_MOD_DOWNLOAD // использовать скачивание
GH_MOD_UPLOAD   // использовать загрузку
GH_MOD_OTA      // использовать ОТА
GH_MOD_OTA_URL  // использовать ОТА по URL
GH_MOD_REBOOT   // использовать перезагрузку из инфо
GH_MOD_SET      // использовать установку значений
GH_MOD_CLICK    // использовать клики
GH_MOD_READ     // использовать чтение
GH_MOD_DELETE   // использовать удаление файлов
GH_MOD_RENAME   // использовать переименование файлов
```
</details>

<details>
<summary>GHstate_t (статус системы)</summary>

```cpp
GH_IDLE
GH_START
GH_STOP

GH_CONNECTING
GH_CONNECTED
GH_DISCONNECTED
GH_ERROR

GH_UNKNOWN
GH_DISCOVER_ALL
GH_DISCOVER
GH_FOCUS
GH_UNFOCUS

GH_SET
GH_CLICK
GH_CLI
GH_PING

GH_READ_HOOK
GH_SET_HOOK
GH_INFO
GH_REBOOT
GH_FSBR
GH_DELETE
GH_RENAME

GH_DOWNLOAD
GH_DOWNLOAD_CHUNK
GH_DOWNLOAD_ERROR
GH_DOWNLOAD_ABORTED
GH_DOWNLOAD_FINISH

GH_UPLOAD
GH_UPLOAD_CHUNK
GH_UPLOAD_ERROR
GH_UPLOAD_ABORTED
GH_UPLOAD_FINISH

GH_OTA
GH_OTA_CHUNK
GH_OTA_ERROR
GH_OTA_ABORTED
GH_OTA_FINISH

GH_OTA_URL
```
</details>