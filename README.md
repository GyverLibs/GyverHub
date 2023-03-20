[![latest](https://img.shields.io/github/v/release/GyverLibs/GyverHUB.svg?color=brightgreen)](https://github.com/GyverLibs/GyverHUB/releases/latest/download/GyverHUB.zip)
[![Foo](https://img.shields.io/badge/Website-AlexGyver.ru-blue.svg?style=flat-square)](https://alexgyver.ru/)
[![Foo](https://img.shields.io/badge/%E2%82%BD$%E2%82%AC%20%D0%9D%D0%B0%20%D0%BF%D0%B8%D0%B2%D0%BE-%D1%81%20%D1%80%D1%8B%D0%B1%D0%BA%D0%BE%D0%B9-orange.svg?style=flat-square)](https://alexgyver.ru/support_alex/)
[![Foo](https://img.shields.io/badge/README-ENGLISH-blueviolet.svg?style=flat-square)](https://github-com.translate.goog/GyverLibs/GyverHUB?_x_tr_sl=ru&_x_tr_tl=en)  

[![Foo](https://img.shields.io/badge/ПОДПИСАТЬСЯ-НА%20ОБНОВЛЕНИЯ-brightgreen.svg?style=social&logo=telegram&color=blue)](https://t.me/GyverLibs)

# GyverHUB
Система взаимодействия с проектом на esp8266/esp32 при помощи приложения и веб-сервиса GyverHUB:
- Обнаружение устройств в локальной сети (TCP) и через Интернет (MQTT)
- Дальнейшее управление устройством из интерфейса приложения или с сайта
- Простая сборка интерфейса управления прямо в скетче - приложение выведет то, что отправит устройство
- Удобный парсинг действий с приложения (парсер из библиотеки GyverPortal)
- 10 активных элементов интерфейса (кнопки, слайдеры, выбор...) + оформление и навигация
- 1000 FontAwesome иконок оформления для кнопок и списка устройств

### Совместимость
ESP8266, ESP32

### Зависимости
- [GyverPortal v3.6.3+](https://github.com/GyverLibs/GyverPortal)
- [PubSubClient](https://github.com/knolleary/pubsubclient)

## Как это работает?
*Подключить библиотеку -> Добавить в свой проект -> Скачать приложение -> Пользоваться*  

1. Каждому устройству задаётся "ID сети" - текстовый идентификатор

- Список бесплатных MQTT брокеров https://kotyara12.ru/iot/cloud_services/

## Содержание
- [Установка](#install)
- [Инициализация](#init)
- [Документация](#docs)
- [Использование](#usage)
- [Версии](#versions)
- [Баги и обратная связь](#feedback)

<a id="install"></a>
## Установка
> Внимание! Для работы **GyverHUB** требуются библиотеки **GyverPortal v3.6.3+** и **PubSubClient**. *При установке GyverHUB через менеджер библиотек нужные библиотеки установятся автоматически!*

- Библиотеку можно найти по названию **GyverHUB** и установить через менеджер библиотек в:
    - Arduino IDE
    - Arduino IDE v2
    - PlatformIO
- [Скачать библиотеку](https://github.com/GyverLibs/GyverHUB/archive/refs/heads/main.zip) .zip архивом для ручной установки:
    - Распаковать и положить в *C:\Program Files (x86)\Arduino\libraries* (Windows x64)
    - Распаковать и положить в *C:\Program Files\Arduino\libraries* (Windows x32)
    - Распаковать и положить в *Документы/Arduino/libraries/*
    - (Arduino IDE) автоматическая установка из .zip: *Скетч/Подключить библиотеку/Добавить .ZIP библиотеку…* и указать скачанный архив
- Читай более подробную инструкцию по установке библиотек [здесь](https://alexgyver.ru/arduino-first/#%D0%A3%D1%81%D1%82%D0%B0%D0%BD%D0%BE%D0%B2%D0%BA%D0%B0_%D0%B1%D0%B8%D0%B1%D0%BB%D0%B8%D0%BE%D1%82%D0%B5%D0%BA)

### Обновление
- Рекомендую всегда обновлять библиотеку: в новых версиях исправляются ошибки и баги, а также проводится оптимизация и добавляются новые фичи
- Через менеджер библиотек IDE: найти библиотеку как при установке и нажать "Обновить"
- Вручную: **удалить папку со старой версией**, а затем положить на её место новую. "Замену" делать нельзя: иногда в новых версиях удаляются файлы, которые останутся при замене и могут привести к ошибкам!

<a id="init"></a>
## Инициализация
```cpp
GyverHUB device;
GyverHUB device(имя сети);                      // с указанием имени сети
GyverHUB device(имя сети, название);            // + название устройства
GyverHUB device(имя сети, название, иконка);    // + икнока

// иконки Font Awesome v5 Solid, бесплатный пак
// https://fontawesome.com/v5/cheatsheet/free/solid - список иконок
// https://fontawesome.com/v5/search?o=r&m=free&s=solid - поиск иконок
// вставлять САМ СИМВОЛ в "строку", например "" , "" , ""
```

<a id="docs"></a>
## Документация
```cpp
// ====================== DEVICE ======================
// настроить девайс (сеть, название, иконку)
void config(char* ID);
void config(char* ID, char* name);
void config(char* ID, char* name, char* icon);

void setupTCP(uint16_t port);                   // настроить TCP порт (умолч. 50000)
void setupMQTT();                               // отключить MQTT
void setupMQTT(char* host, uint16_t port);      // настроить MQTT (хост, порт)
void setupMQTT(char* host, uint16_t port, char* login, char* pass);     // + логин, пароль

void usePortal(bool v);     // открывать веб-интерфейс в браузере по IP устройства по долгому клику (умолч. false)
void begin();               // начать работу
void stop();                // остановить работу
void attachBuild(функция);  // подключить функцию для сборки интерфейса
void attach(функция);       // подключить функцию-обработчик действий с приложения
void refresh();             // отправить интерфейс (вызывать внутри обработчика действия)

bool tick();                // тикер, вызывать в loop

bool online();              // проверка онлайн (mqtt подключен?)
bool running();             // запущен?
bool statusChanged();       // статус изменился
uint8_t status();           // получить статус (см. коды статусов)

// коды статусов
GH_IDLE         0
GH_TCP_UNKNOWN  1
GH_TCP_FIND     2
GH_TCP_CLICK    3
GH_TCP_UPDATE   4
GH_MQ_CONNECTED 5
GH_MQ_ERROR     6
GH_MQ_UNKNOWN   7
GH_MQ_FIND      8
GH_MQ_CLICK     9
GH_MQ_UPDATE    10
GH_START        11
GH_STOP         12

// ======================== APP ========================
// далее по тексту:
// имя/текст/подпись: строка в любом виде ("строка", F("строка"), String, char*)
// имя: УНИКАЛЬНОЕ внутреннее имя компонента в рамках данного интерфейса
// текст: текст на компоненте
// подпись: подпись слева от компонента
// тип данных Т: любой (например у слайдера и текста)
// список: строка, разделитель - запятая, без пробелов вокруг запятой, например "значение 1,имя2,кнопка 2,STOP"

// ОФОРМЛЕНИЕ И ВЁРСТКА

void addStatus(текст);                      // добавить статус (отображается под сигналом WiFi)

void addTabs(имя, список текстов, int выбранный);   // вкладки
void addSpacer();                           // отступ по вертикали
void addSpacer(int размер);                 // + высота (умолч. 20)
void addLine();                             // добавить полосу-разделитель

void addTitle(текст);                       // заголовок
void addLabel(имя, подпись, текст);         // информационный лейбл
void addLabel(имя, подпись, текст, цвет);   // цвет GPcolor или из GyverPortal вида GP_RED, GP_GREEN...

void addLED(имя, подпись, цвет);
void addLED(имя, подпись, цвет, иконка)     // + иконка "строкой"
// цвет GPcolor или из GyverPortal вида GP_RED, GP_GREEN...

// КНОПКИ

void addButton(имя, текст);                 // добавить кнопку
void addButtons(имена, тексты);             // добавить кнопки - список надписей на кнопках

void addButtonIcon(имя, текст);             // добавить кнопку с иконкой
void addButtonsIcons(имя, текст, размер);   // + размер (умолч. 45)

void addButtonsIcons(имена, тексты);        // добавить кнопки с иконками - список иконок (например ",,")
void addButtonsIcons(имена, тексты, размер);// + размер (умолч. 45)

// ВВОД

void addInput(имя, подпись, T текст);       // ввод текста
void addPass(имя, подпись, текст);          // ввод пароля

// НАСТРОЙКА И ВЫБОР

void addSlider(имя, подпись, T значение, T мин, T макс);    // слайдер
void addSwitch(имя, подпись, bool значение);                // выключатель
void addDate(имя, подпись, GPdate значение);                // выбор даты
void addTime(имя, подпись, GPtime значение);                // выбор времени
void addSelect(имя, подпись, список, int выбранный);        // выбор из списка

void addUpdate(const String& names, uint16_t prd = 1000);
//label как угодно (updateInt(), updateString()...)
//switch как updateBool()
//LED как updateColor()

// кастом
void addCustom(строка);
```

<a id="usage"></a>
## Использование


<a id="versions"></a>
## Версии
- v1.0


<a id="feedback"></a>
## Баги и обратная связь
При нахождении багов создавайте **Issue**, а лучше сразу пишите на почту [alex@alexgyver.ru](mailto:alex@alexgyver.ru)  
Библиотека открыта для доработки и ваших **Pull Request**'ов!

При сообщении о багах или некорректной работе библиотеки нужно обязательно указывать:
- Версия библиотеки
- Какой используется МК
- Версия SDK (для ESP)
- Версия Arduino IDE
- Корректно ли работают ли встроенные примеры, в которых используются функции и конструкции, приводящие к багу в вашем коде
- Какой код загружался, какая работа от него ожидалась и как он работает в реальности
- В идеале приложить минимальный код, в котором наблюдается баг. Не полотно из тысячи строк, а минимальный код
