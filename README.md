[![latest](https://img.shields.io/github/v/release/GyverLibs/GyverHub.svg?color=brightgreen)](https://github.com/GyverLibs/GyverHub/releases/latest/download/GyverHub.zip)
[![Foo](https://img.shields.io/badge/Website-AlexGyver.ru-blue.svg?style=flat-square)](https://alexgyver.ru/)
[![Foo](https://img.shields.io/badge/%E2%82%BD$%E2%82%AC%20%D0%9D%D0%B0%20%D0%BF%D0%B8%D0%B2%D0%BE-%D1%81%20%D1%80%D1%8B%D0%B1%D0%BA%D0%BE%D0%B9-orange.svg?style=flat-square)](https://alexgyver.ru/support_alex/)
[![Foo](https://img.shields.io/badge/README-ENGLISH-blueviolet.svg?style=flat-square)](https://github-com.translate.goog/GyverLibs/GyverHub?_x_tr_sl=ru&_x_tr_tl=en)  

[![Foo](https://img.shields.io/badge/ПОДПИСАТЬСЯ-НА%20ОБНОВЛЕНИЯ-brightgreen.svg?style=social&logo=telegram&color=blue)](https://t.me/GyverLibs)

# GyverHub [beta]
Панель управления для esp8266, esp32 и других Arduino. Конструктор интерфейса, интеграция в умный дом

> <br>**Документация находится в [Wiki репозитория](https://github.com/GyverLibs/GyverHub/wiki). English docs - [translate](https://github-com.translate.goog/GyverLibs/GyverHub/wiki?_x_tr_sl=ru&_x_tr_tl=en)**<br><br>

> <br>**Внимание! Перед началом работы обязательны к прочтению главы по [установке библиотеки](https://github.com/GyverLibs/GyverHub/blob/main/docs/2.start.md) и [настройке приложения](https://github.com/GyverLibs/GyverHub/blob/main/docs/3.app.md)**<br><br>

|⚠️⚠️⚠️<br>**Это новая версия библиотеки! Если вам нужна старая, то она находится в [dev-ветке](https://github.com/GyverLibs/GyverHub/tree/old)**<br>⚠️⚠️⚠️|
| --- |

|⚠️⚠️⚠️<br>**Бета-версия! Проект пока что тестируется, следите за коммитами. [Обсуждение тут](https://github.com/GyverLibs/GyverHub/issues/44)**<br>⚠️⚠️⚠️|
| --- |

## О проекте
![promo](/docs/promo.png)

GyverHub - это платформа, состоящая из библиотеки для *устройства* (Arduino-совместимого микроконтроллера) и *приложения* для управления. Приложение написано на JavaScript и может работать:
- Как онлайн версия на официальном сайте **hub.gyver.ru**: [HTTP](http://hub.gyver.ru/) и [HTTPS](https://hub.gyver.ru/) версия, есть [HTTPS](https://gyverlibs.github.io/GyverHub) зеркало на GitHub Pages
- Как веб-приложение (оффлайн PWA): официальный сайт можно установить на любое устройство (Android/iOS/Linux/Windows/Mac...)
- Как [телеграм-приложение](https://t.me/GyverHUB_bot)
- Как нативное приложение:
  - Android: [Google Play](https://play.google.com/store/apps/details?id=ru.alexgyver.GyverHub), [скачать APK](https://github.com/GyverLibs/GyverHub-app/releases/latest)
  - iOS: [App Store](https://apps.apple.com/kz/app/gyverhub/id6474273925)
  - Desktop (Windows/Linux/Mac): [скачать](https://github.com/neko-neko-nyan/gyverhub-desktop/releases/latest)
- Как автономная вебморда из Flash памяти ESP8266/ESP32 - [инструкция](https://github.com/GyverLibs/GyverHub/blob/main/docs/3.app.md#%D0%B2%D0%B5%D0%B1%D0%BC%D0%BE%D1%80%D0%B4%D0%B0-esp)

Устройством может быть:
- ESP8266 и ESP32 - библиотека нативно поддерживает их WiFi функции для работы по HTTP/WS/MQTT
- Любые Arduino - работают в [ручном режиме](https://github.com/GyverLibs/GyverHub/blob/main/docs/9.bridge.md) по любому способу связи
- Компьютер (персональный, одноплатный) - [библиотека сервера](https://github.com/neko-neko-nyan/pygyverhubd) на Python (в разработке)

Устройство и приложение обмениваются данными по своему протоколу через различные интерфейсы связи: 
- **MQTT** - через Интернет (свой или сторонний MQTT брокер)
- **HTTP + WebSocket** - в локальной WiFi сети
- **Serial** - по USB или Bluetooth UART
- **Bluetooth** - через Bluetooth BLE
- **Telegram** - через телеграм-ботов

Основные моменты:
- Приложение может обнаруживать устройства в своей "сети"
- В прошивке для устройства "собирается" графический интерфейс, который отправляется в приложение
- Можно управлять устройством в приложении или с сайта, даже через Интернет
- Библиотека позволяет максимально просто добавить устройство в систему умного дома по MQTT (Алиса, Home Assistant...)
- Полностью автоматический парсинг данных с приложения и умного дома
- Более 30 виджетов панели управления (кнопки, слайдеры, выключатели...)
- Полная кастомизация: можно создавать свои виджеты, подключать скрипты и стили в приложение
- Доп. модули: сенсорный графический дисплей, менеджер файлов, информация об устройстве, OTA обновление, проверка обновлений на GitHub, защита паролем, командная строка...
- 1000 FontAwesome иконок оформления для кнопок и списка устройств
- Тёмная и светлая темы, несколько цветовых схем, мультиязычный интерфейс
- Не имеет своего сервера и "личного кабинета", работает полностью автономно на ПК/смартфоне
- Открытая [база пользовательских проектов](https://github.com/GyverLibs/GyverHub-projects)
- OTA обновление проектов с GitHub, прошивка проектов из браузера по USB

В отличие от аналогов (RemoteXY, Blynk) GyverHub:
- Полностью бесплатный
- Не имеет ограничений
- Не привязан к серверам разработчика
- Не нуждается в локальном сервере
- Может работать без приложения и без доступа к Интернет

## Благодарности
- **neko-neko-nyan** - идеи, советы и доработки И МНОГО МНОГО ВСЕГО ЕЩЁ!
- **ZMain**, **DiTheRX** - [мобильное приложение](https://github.com/GyverLibs/GyverHub-app) (Android, iOS)
- **neko-neko-nyan** - [десктопное приложение](https://github.com/neko-neko-nyan/gyverhub-desktop) (Windows)
- **Serega88kos** - тестирование, идеи
- **DenysChuhlib** - идеи
- **IliaSalaur** - идеи

## Версии
- beta

## Обновление
- Рекомендую всегда обновлять библиотеку: в новых версиях исправляются ошибки и баги, а также проводится оптимизация и добавляются новые фичи
- Через менеджер библиотек IDE: найти библиотеку как при установке и нажать "Обновить"
- Вручную: **удалить папку со старой версией**, а затем положить на её место новую. "Замену" делать нельзя: иногда в новых версиях удаляются файлы, которые останутся при замене и могут привести к ошибкам!

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
