#pragma once
#include <Arduino.h>

#define GH_UNUSED __attribute__((unused))
#define CSREF const String&
#define GH_PGM(name, str) static const char name[] PROGMEM = str
#define GH_PGM_LIST(name, ...) static const char* const name[] PROGMEM = {__VA_ARGS__};
#define FSTR const __FlashStringHelper*
// #define GHTREF const ghc::Text&
// #define GHTXT ghc::Text
#define GHTREF const sutil::AnyText&
#define GHTXT sutil::AnyText
#define GHVREF const sutil::AnyValue&
#define GHVAL sutil::AnyValue
#define GHDELPTR(x) do {delete x; x = nullptr;} while (0);

#define GH_NUMBERS F("^\\d+$")
#define GH_LETTERS F("^[a-zA-Z]+$")
#define GH_LETTERS_S F("^[a-z]+$")
#define GH_LETTERS_C F("^[A-Z]+$")

#if (defined(ESP8266) || defined(ESP32))
#define GH_ESP_BUILD
#else
#define GH_NO_FS
#endif

// LOG
#define GH_LOG(x)                             \
    do {                                      \
        Serial.print(F(">> "));               \
        Serial.print(x);                      \
        Serial.print(F(" in "));              \
        Serial.print(__FUNCTION__);           \
        Serial.print(F(" [" __FILE__ " : ")); \
        Serial.print(__LINE__);               \
        Serial.println(']');                  \
    } while (0);

// MCU family
#if CONFIG_IDF_TARGET_ESP32C3
#define GH_PLATFORM "ESP32-C3"
#elif CONFIG_IDF_TARGET_ESP32S2
#define GH_PLATFORM "ESP32-S2"
#elif CONFIG_IDF_TARGET_ESP32S3
#define GH_PLATFORM "ESP32-S3"
#elif CONFIG_IDF_TARGET_ESP32
#define GH_PLATFORM "ESP32"
#elif defined(ESP8266)
#define GH_PLATFORM "ESP8266"
#else
#define GH_PLATFORM "Arduino"
#endif