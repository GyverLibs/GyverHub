#pragma once
#include <Arduino.h>
#include <StringUtils.h>

#ifndef GH_NO_PAIRS
#include <Pairs.h>
#endif

#include "anyptr.h"
#include "packet.h"
#include "types.h"
#include "ui/button.h"
#include "ui/color.h"
#include "ui/flags.h"
#include "ui/log.h"

namespace ghc {

void __attribute__((weak)) strToVar(GHTREF str, const AnyPtr& data) {
    if (!data.ptr || data.type == gh::Type::NULL_T) return;

    switch (data.type) {
        case gh::Type::STR_T:
            str.toString(*(String*)data.ptr);
            break;
        case gh::Type::CSTR_T:
            str.toStr((char*)data.ptr);
            break;

        case gh::Type::BOOL_T:
            *(bool*)data.ptr = str.toBool();
            break;

        case gh::Type::INT8_T:
            *(int8_t*)data.ptr = str.toInt16();
            break;

        case gh::Type::UINT8_T:
            *(uint8_t*)data.ptr = str.toInt16();
            break;

        case gh::Type::INT16_T:
            *(int16_t*)data.ptr = str.toInt16();
            break;
        case gh::Type::UINT16_T:
            *(uint16_t*)data.ptr = str.toInt16();
            break;

        case gh::Type::INT32_T:
            *(int32_t*)data.ptr = str.toInt32();
            break;
        case gh::Type::UINT32_T:
            *(uint32_t*)data.ptr = str.toInt32();
            break;

#ifdef GH_ESP_BUILD
        case gh::Type::INT64_T:
            *(int32_t*)data.ptr = str.toInt64();
            break;
        case gh::Type::UINT64_T:
            *(uint64_t*)data.ptr = str.toInt64();
            break;
#endif

        case gh::Type::FLOAT_T:
            *(float*)data.ptr = str.toFloat();
            break;
        case gh::Type::DOUBLE_T:
            *(double*)data.ptr = str.toFloat();
            break;

        case gh::Type::COLOR_T:
            ((gh::Color*)data.ptr)->setHEX(str.toInt32());
            break;
        case gh::Type::FLAGS_T:
            ((gh::Flags*)data.ptr)->flags = str.toInt16();
            break;
        case gh::Type::POS_T: {
            uint32_t xy = str.toInt32();
            ((gh::Pos*)data.ptr)->_changed = true;
            ((gh::Pos*)data.ptr)->x = xy >> 16;
            ((gh::Pos*)data.ptr)->y = xy & 0xffff;
        } break;
        case gh::Type::BTN_T:
            if (str[0] == '2') {
                ((gh::Button*)data.ptr)->_changed = 0;
                ((gh::Button*)data.ptr)->_clicked = 1;
            } else {
                ((gh::Button*)data.ptr)->_changed = 1;
                ((gh::Button*)data.ptr)->_clicked = 0;
                ((gh::Button*)data.ptr)->_state = str[0] & 0xF;
            }
            break;

        default:
            break;
    }
}

void __attribute__((weak)) varToStr(Packet& p, const AnyPtr& data) {
    if (!data.ptr || data.type == gh::Type::NULL_T) return;

    switch (data.type) {
        case gh::Type::STR_T:
            p.addTextEsc(*(String*)data.ptr);
            break;
        case gh::Type::CSTR_T:
            p.addTextEsc((const char*)data.ptr);
            break;
        case gh::Type::TEXT_T:
            p.addTextEsc(*(const GHTXT*)data.ptr);
            break;

        case gh::Type::BOOL_T:
            p.addIntRaw(*(bool*)data.ptr);
            break;

        case gh::Type::INT8_T:
            p.addIntRaw(*(int8_t*)data.ptr);
            break;

        case gh::Type::UINT8_T:
            p.addIntRaw(*(uint8_t*)data.ptr);
            break;

        case gh::Type::INT16_T:
            p.addIntRaw(*(int16_t*)data.ptr);
            break;
        case gh::Type::UINT16_T:
            p.addIntRaw(*(uint16_t*)data.ptr);
            break;

        case gh::Type::INT32_T:
            p.addIntRaw(*(int32_t*)data.ptr);
            break;
        case gh::Type::UINT32_T:
            p.addIntRaw(*(uint32_t*)data.ptr);
            break;

#ifdef GH_ESP_BUILD
        case gh::Type::INT64_T:
            p.addIntRaw(*(int64_t*)data.ptr);
            break;
        case gh::Type::UINT64_T:
            p.addIntRaw(*(uint64_t*)data.ptr);
            break;
#endif

        case gh::Type::FLOAT_T:
            p.addIntRaw(*(float*)data.ptr);
            break;
        case gh::Type::DOUBLE_T:
            p.addIntRaw(*(double*)data.ptr);
            break;

        case gh::Type::COLOR_T:
            p.addIntRaw(((gh::Color*)data.ptr)->getHEX());
            break;
        case gh::Type::FLAGS_T:
            p.addIntRaw(((gh::Flags*)data.ptr)->flags);
            break;

        case gh::Type::LOG_T:
            ((gh::Log*)data.ptr)->read(&(p.s), true);
            break;

#ifndef GH_NO_PAIRS
        case gh::Type::PAIR_T:
            ((Pair*)data.ptr)->addString(p.s);
            break;
#endif

        default:
            break;
    }
}

}  // namespace ghc