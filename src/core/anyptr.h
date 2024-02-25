#pragma once

#include <Arduino.h>
#include <StringUtils.h>
#include <limits.h>

#ifndef GH_NO_PAIRS
#include <Pairs.h>
#endif

#include "hub_macro.hpp"
#include "types.h"
#include "ui/button.h"
#include "ui/color.h"
#include "ui/flags.h"
#include "ui/log.h"
#include "ui/pos.h"

namespace ghc {

class AnyPtr {
   public:
    AnyPtr() {}
    AnyPtr(void* ptr, gh::Type type) : ptr(ptr), type(type) {}

    AnyPtr(bool* ptr) : ptr(ptr), type(gh::Type::BOOL_T) {}

    AnyPtr(signed char* ptr) : ptr(ptr), type(gh::Type::INT8_T) {}
    AnyPtr(unsigned char* ptr) : ptr(ptr), type(gh::Type::UINT8_T) {}

    AnyPtr(short* ptr) : ptr(ptr), type(gh::Type::INT16_T) {}
    AnyPtr(unsigned short* ptr) : ptr(ptr), type(gh::Type::UINT16_T) {}

#if (UINT_MAX == UINT32_MAX)
    AnyPtr(int* ptr) : ptr(ptr), type(gh::Type::INT32_T) {}
    AnyPtr(unsigned int* ptr) : ptr(ptr), type(gh::Type::UINT32_T) {}
#else
    AnyPtr(int* ptr) : ptr(ptr), type(gh::Type::INT16_T) {}
    AnyPtr(unsigned int* ptr) : ptr(ptr), type(gh::Type::UINT16_T) {}
#endif

    AnyPtr(long* ptr) : ptr(ptr), type(gh::Type::INT32_T) {}
    AnyPtr(unsigned long* ptr) : ptr(ptr), type(gh::Type::UINT32_T) {}

#ifdef GH_ESP_BUILD
    AnyPtr(long long* ptr) : ptr(ptr), type(gh::Type::INT64_T) {}
    AnyPtr(unsigned long long* ptr) : ptr(ptr), type(gh::Type::UINT64_T) {}
#endif

    AnyPtr(float* ptr) : ptr(ptr), type(gh::Type::FLOAT_T) {}
    AnyPtr(double* ptr) : ptr(ptr), type(gh::Type::DOUBLE_T) {}

    AnyPtr(char* ptr) : ptr(ptr), type(gh::Type::CSTR_T) {}
    AnyPtr(String* ptr) : ptr(ptr), type(gh::Type::STR_T) {}

    AnyPtr(gh::Color* ptr) : ptr(ptr), type(gh::Type::COLOR_T) {}
    AnyPtr(gh::Pos* ptr) : ptr(ptr), type(gh::Type::POS_T) {}
    AnyPtr(gh::Flags* ptr) : ptr(ptr), type(gh::Type::FLAGS_T) {}
    AnyPtr(gh::Log* ptr) : ptr(ptr), type(gh::Type::LOG_T) {}
    AnyPtr(gh::Button* ptr) : ptr(ptr), type(gh::Type::BTN_T) {}
    AnyPtr(const GHTXT* ptr) {
        if (ptr && ptr->valid()) {
            this->ptr = ptr;
            type = gh::Type::TEXT_T;
        }
    }

#ifndef GH_NO_PAIRS
    AnyPtr(Pairs* ptr) : ptr(ptr), type(gh::Type::PAIRS_T) {}
    AnyPtr(Pair* ptr) : ptr(ptr), type(gh::Type::PAIR_T) {}
#endif

    const void* ptr = nullptr;
    gh::Type type = gh::Type::NULL_T;
};

}  // namespace ghc