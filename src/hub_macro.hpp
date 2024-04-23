#pragma once
#include <Arduino.h>

#define GH_UNUSED __attribute__((unused))
#define CSREF const String&
#define GH_PGM(name, str) static const char name[] PROGMEM = str
#define GH_PGM_LIST(name, ...) static const char* const name[] PROGMEM = {__VA_ARGS__};
#define FSTR const __FlashStringHelper*
#define GHTREF const su::Text&
#define GHTXT su::Text
#define GHVREF const su::Value&
#define GHVAL su::Value
#define GHDELPTR(x)  \
    do {             \
        delete x;    \
        x = nullptr; \
    } while (0);

#define GH_BROAD_ID 0xffffffff
#define GH_BROAD_ID_STR "ffffffff"
#define GH_NUMBERS F("^\\d+$")
#define GH_LETTERS F("^[a-zA-Z]+$")
#define GH_LETTERS_S F("^[a-z]+$")
#define GH_LETTERS_C F("^[A-Z]+$")

// LOG
#define GH_LOG(x)                               \
    do {                                        \
        Serial.print(F("> "));                  \
        Serial.print(x);                        \
        Serial.print(F(" in "));                \
        Serial.print(__FUNCTION__);             \
        Serial.print(F("() [" __FILE__ " : ")); \
        Serial.print(__LINE__);                 \
        Serial.println(']');                    \
    } while (0);

// check esp
#if (defined(ESP8266) || defined(ESP32))
#define GH_ESP_BUILD 1
#define GH_BUILD_ESP 1
#else
#define GH_NO_FS 1
#endif

// MCU family
#if CONFIG_IDF_TARGET_ESP32
#define GH_PLATFORM "ESP32"
#define GH_BUILD_ESP32 1
#elif CONFIG_IDF_TARGET_ESP32S2
#define GH_PLATFORM "ESP32-S2"
#define GH_BUILD_ESP32S2 1
#elif CONFIG_IDF_TARGET_ESP32S3
#define GH_PLATFORM "ESP32-S3"
#define GH_BUILD_ESP32S3 1
#elif CONFIG_IDF_TARGET_ESP32C3
#define GH_PLATFORM "ESP32-C3"
#define GH_BUILD_ESP32C3 1
#elif CONFIG_IDF_TARGET_ESP32C6
#define GH_PLATFORM "ESP32-C6"
#define GH_BUILD_ESP32C6 1
#elif CONFIG_IDF_TARGET_ESP32H2
#define GH_PLATFORM "ESP32-H2"
#define GH_BUILD_ESP32H2 1
#elif defined(ESP8266)
#define GH_PLATFORM "ESP8266"
#define GH_BUILD_ESP8266 1
#else
#define GH_PLATFORM "Arduino"
#define GH_BUILD_ARDUINO 1
#endif