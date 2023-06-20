#pragma once

#define GH_UNUSED __attribute__((unused))
#define VSPTR const void*
#define CSREF const String&
#define GH_PGM(name, str) static const char name[] PROGMEM = str
#define GH_PGM_LIST(name, ...) const char* const name[] PROGMEM = {__VA_ARGS__};
#define FSTR const __FlashStringHelper*
#define GH_NO_LABEL F("_no")
#define GH_NUMBERS F("^\\d+$")
#define GH_LETTERS F("^[a-zA-Z]+$")
#define GH_LETTERS_S F("^[a-z]+$")
#define GH_LETTERS_C F("^[A-Z]+$")