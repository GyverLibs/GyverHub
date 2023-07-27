[![latest](https://img.shields.io/github/v/release/GyverLibs/GyverHub.svg?color=brightgreen)](https://github.com/GyverLibs/GyverHub/releases/latest/download/GyverHub.zip)
[![Foo](https://img.shields.io/badge/Website-AlexGyver.ru-blue.svg?style=flat-square)](https://alexgyver.ru/)
[![Foo](https://img.shields.io/badge/%E2%82%BD$%E2%82%AC%20%D0%9D%D0%B0%20%D0%BF%D0%B8%D0%B2%D0%BE-%D1%81%20%D1%80%D1%8B%D0%B1%D0%BA%D0%BE%D0%B9-orange.svg?style=flat-square)](https://alexgyver.ru/support_alex/)
[![Foo](https://img.shields.io/badge/README-ENGLISH-blueviolet.svg?style=flat-square)](https://github-com.translate.goog/GyverLibs/GyverHub?_x_tr_sl=ru&_x_tr_tl=en)  

[![Foo](https://img.shields.io/badge/ПОДПИСАТЬСЯ-НА%20ОБНОВЛЕНИЯ-brightgreen.svg?style=social&logo=telegram&color=blue)](https://t.me/GyverLibs)

# GyverHub
Панель управления для esp8266, esp32 и других Arduino. Конструктор интерфейса, интеграция в умный дом

> <br>**Документация находится в [Wiki репозитория](https://github.com/GyverLibs/GyverHub/wiki). English docs - [translate](https://github-com.translate.goog/GyverLibs/GyverHub/wiki?_x_tr_sl=ru&_x_tr_tl=en)**<br><br>

> <br>**Внимание! Перед началом работы обязательны к прочтению главы по [установке библиотеки](https://github.com/GyverLibs/GyverHub/wiki/2.-%D0%9D%D0%B0%D1%87%D0%B0%D0%BB%D0%BE-%D1%80%D0%B0%D0%B1%D0%BE%D1%82%D1%8B) и [настройке приложения](https://github.com/GyverLibs/GyverHub/wiki/3.-%D0%9F%D1%80%D0%B8%D0%BB%D0%BE%D0%B6%D0%B5%D0%BD%D0%B8%D0%B5)**<br><br>

|⚠️⚠️⚠️<br>**Бета-версия! Проект пока что тестируется, следите за коммитами. Известные проблемы, особенности версии, обсуждение, предложения, баги - всё [вот тут](https://github.com/GyverLibs/GyverHub/issues/6)**<br>⚠️⚠️⚠️|
| --- |

|☕☕☕<br>**Библиотека разработана благодаря народной поддержке [на Boosty](https://boosty.to/alexgyvershow). Также есть [другие способы](https://alexgyver.ru/support_alex/) поддержать автора**<br>☕☕☕|
| --- |

## Совместимость
Все Arduino
- **НЕ РАБОТАЕТ НА ESP32 SDK 2.0.8**

## Установка
- Зависимости: [Stamp](https://github.com/GyverLibs/Stamp), [PubSubClient](https://github.com/knolleary/pubsubclient), [arduinoWebSockets](https://github.com/Links2004/arduinoWebSockets), [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer), [ESPAsyncTCP](https://github.com/me-no-dev/ESPAsyncTCP), [AsyncTCP](https://github.com/me-no-dev/AsyncTCP), [async-mqtt-client](https://github.com/marvinroger/async-mqtt-client)
- Подробнее читай [в документации](https://github.com/GyverLibs/GyverHub/wiki/2.-%D0%9D%D0%B0%D1%87%D0%B0%D0%BB%D0%BE-%D1%80%D0%B0%D0%B1%D0%BE%D1%82%D1%8B)

## О проекте
![promo](/docs/promo.jpg)

GyverHub - это платформа, включающая в себя библиотеку для устройства и приложение для управления. Приложение написано на JavaScript и по сути является сайтом, который доступен:
- С официального сайта [hub.gyver.ru](http://hub.gyver.ru/), а также зеркала на [GitHub Pages](https://gyverlibs.github.io/GyverHub) (только HTTPS)
- Android приложение: [Google Play](https://play.google.com/store/apps/details?id=ru.alexgyver.GyverHub), [скачать APK](https://github.com/GyverLibs/GyverHub/raw/main/app/GyverHub.apk) - [особенности](https://github.com/GyverLibs/GyverHub/wiki/3.-%D0%9F%D1%80%D0%B8%D0%BB%D0%BE%D0%B6%D0%B5%D0%BD%D0%B8%D0%B5#android-%D0%BF%D1%80%D0%B8%D0%BB%D0%BE%D0%B6%D0%B5%D0%BD%D0%B8%D0%B5)
- Сайт можно [установить](https://github.com/GyverLibs/GyverHub/wiki/3.-%D0%9F%D1%80%D0%B8%D0%BB%D0%BE%D0%B6%D0%B5%D0%BD%D0%B8%D0%B5#%D0%B2%D0%B5%D0%B1-%D0%BF%D1%80%D0%B8%D0%BB%D0%BE%D0%B6%D0%B5%D0%BD%D0%B8%D0%B5-pwa) как веб-приложение (PWA) на любое устройство (Android/iOS/Windows...)
- Локально с компьютера, для этого нужно [скачать сайт](https://github.com/GyverLibs/GyverHub/raw/main/web/local/GyverHub.html) (правой кнопкой - *сохранить ссылку как*) и открыть его в браузере
- [Из памяти ESP8266/32](https://github.com/GyverLibs/GyverHub/wiki/3.-%D0%9F%D1%80%D0%B8%D0%BB%D0%BE%D0%B6%D0%B5%D0%BD%D0%B8%D0%B5#%D0%B7%D0%B0%D0%BF%D1%83%D1%81%D0%BA-%D0%B8%D0%B7-esp)
- Из [Телеграм-бота](https://t.me/GyverHUB_bot)

Устройством может быть:
- ESP8266 и ESP32 - библиотека нативно поддерживает их WiFi функции
- Любые Arduino - подключаются к WiFi удобным способом и работают в [ручном режиме](https://github.com/GyverLibs/GyverHub/wiki/8.-%D0%A0%D1%83%D1%87%D0%BD%D0%BE%D0%B9-%D1%80%D0%B5%D0%B6%D0%B8%D0%BC)
- Компьютер (персональный, одноплатный) - [библиотека сервера](https://github.com/neko-neko-nyan/pygyverhubd) на Python (в разработке)

Устройство и приложение обмениваются данными по своему протоколу (см. [API](https://github.com/GyverLibs/GyverHub/wiki#api)) через различные интерфейсы связи: 
- MQTT (через Интернет)
- WebSocket (в локальной сети)
- Serial (в разработке)
- Bluetooth (в разработке)

Основные моменты:
- Приложение может обнаруживать устройства в своей "сети"
- В прошивке для устройства "собирается" графический интерфейс, который отправляется в приложение
- Можно управлять устройством в приложении или с сайта, даже через Интернет
- Библиотека позволяет максимально просто добавить устройство в систему умного дома по MQTT (Алиса, Home Assistant...)
- Открытая база пользовательских проектов (вкладка Projects в приложении). https://github.com/GyverLibs/GyverHub-projects)

Особенности:
- Библиотека невероятно проста в использовании
- Удобный парсинг действий с приложения и умного дома
- Библиотека асинхронная (опционально для ESP8266/ESP32)
- Оптимальное использование памяти для работы на слабых МК
- Более 20 активных компонентов интерфейса (кнопки, слайдеры, выбор...) + блоки оформления и навигации
- Доп. модули: "беспроводной" сенсорный дисплей, менеджер файлов, информация об устройстве, OTA обновление, проверка обновлений на GitHub, защита паролем, интерфейс командной строки...
- 1000 FontAwesome иконок оформления для кнопок и списка устройств
- Два стиля отображения компонентов, тёмная и светлая темы, несколько цветовых схем в приложении

Аналоги:
- RemoteXY
- Blynk

В отличие от них GyverHub полностью бесплатный, безопасный, более удобный, не имеет ограничений, не нуждается в локальном сервере и может работать без приложения и без подключения к Интернет.

## Обновление
- Рекомендую всегда обновлять библиотеку: в новых версиях исправляются ошибки и баги, а также проводится оптимизация и добавляются новые фичи
- Через менеджер библиотек IDE: найти библиотеку как при установке и нажать "Обновить"
- Вручную: **удалить папку со старой версией**, а затем положить на её место новую. "Замену" делать нельзя: иногда в новых версиях удаляются файлы, которые останутся при замене и могут привести к ошибкам!

## Версии
- v1.0

## Благодарности
- @neko-neko-nyan
- @IliaSalaur

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
