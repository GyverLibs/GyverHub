#pragma once

#define GH_UNUSED __attribute__((unused))
#define VSPTR const void*
#define CSREF const String&
#define GH_PGM(name, str) static const char name[] PROGMEM = str
#define GH_PGM_LIST(name, ...) const char* const name[] PROGMEM = {__VA_ARGS__};

#ifdef ESP32
#define FSTR PGM_P
#else
#define FSTR const __FlashStringHelper*
#endif