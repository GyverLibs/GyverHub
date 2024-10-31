#pragma once
#include <Arduino.h>
#include <StringUtils.h>

#include "builder_class.h"
#include "core/anyptr.h"
#include "core/build.h"
#include "core/datatypes.h"
#include "core/packet.h"
#include "core/types.h"
#include "flag.h"
#include "hub_macro.hpp"
#include "update_class.h"

namespace ghc {

class Widget {
    friend class gh::Builder;
    friend class gh::Update;

   public:
    Widget(ghc::Packet& p, gh::Build* build = nullptr) : p(p), _build(build) {
        // update or ui
        _allowed = (!_build || build->action == gh::Action::UI);
    }

    // ===================== CUSTOM ====================
    // int/string/bool поле
    Widget& param(GHTREF key, const sutil::AnyValue& val) {
        if (_ok() && val.valid()) {
            if (val.type() == sutil::AnyText::Type::value) p.addInt(key, val);
            else p.addStringEsc(key, val);
        }
        return *this;
    }

    // float поле
    Widget& param(GHTREF key, double val, uint8_t dec) {
        if (_ok()) p.addFloat(key, val, dec);
        return *this;
    }

    // ===================== VALUE =====================
    // привязать переменную как значение
    Widget& valueVar(const ghc::AnyPtr& data) {
        if (_ok() && data.ptr) {
            p.addKey(Tag::value);
            p.quotes();
            ghc::varToStr(p, data);
            p.quotes();
            p.comma();
        }
        return *this;
    }

    // int/string/bool значение
    Widget& value(const sutil::AnyValue& val) {
        return param(Tag::value, val);
    }

    // float значение
    Widget& value(double val, uint8_t dec) {
        return param(Tag::value, val, dec);
    }

    // ===================== TEXT =====================
    // текст
    Widget& text(GHTREF str) {
        return param(Tag::text, str);
    }

    // иконка (glyph или unicode) https://fontawesome.com/v5/search?o=r&m=free&s=solid
    Widget& icon(GHTREF str) {
        return param(Tag::icon, str);
    }

    // максимальная длина текста
    Widget& maxLen(uint16_t len) {
        return param(Tag::maxlen, len);
    }

    // ===================== MISC =====================
    // количество строк поля текста
    Widget& rows(uint16_t rows) {
        return param(Tag::rows, rows);
    }

    // regex для Input и Pass
    Widget& regex(GHTREF str) {
        return param(Tag::regex, str);
    }

    // выравнивание текста для label/title
    Widget& align(gh::Align align) {
        return param(Tag::align, (uint8_t)align);
    }

    // минимум, максимум и шаг изменения значения (float)
    Widget& range(double min, double max, double step, uint8_t dec = 2) {
        if (_ok()) {
            p.addFloat(Tag::min, min, dec);
            p.addFloat(Tag::max, max, dec);
            p.addFloat(Tag::step, step, dec);
            p.addInt(Tag::dec, dec);
        }
        return *this;
    }

    // минимум, максимум и шаг изменения значения (целые числа)
    template <typename T>
    Widget& range(T min, T max, T step) {
        if (_ok()) {
            p.addInt(Tag::min, min);
            p.addInt(Tag::max, max);
            p.addInt(Tag::step, step);
            p.addInt(Tag::dec, 0);
        }
        return *this;
    }

    // единицы измерения
    Widget& unit(GHTREF str) {
        return param(Tag::unit, str);
    }

    // размер шрифта/кнопки
    Widget& fontSize(uint16_t size) {
        return param(Tag::fsize, size);
    }

    // цвет uint32_t 24 бит
    Widget& color(uint32_t color) {
        if (color != 0xffffffff) return param(Tag::color, color);
        return *this;
    }

    // цвет gh::Colors
    Widget& color(gh::Colors col) {
        return color((uint32_t)col);
    }

    // Действие (обновить файл, вызвать Confirm/Prompt)
    Widget& action(bool act = 1) {
        return param(Tag::action, act);
    }

    // ===================== WIDGET =====================
    // Ширина (относительная) и высота (px) виджета
    Widget& size(uint16_t width, uint16_t height = 0) {
        if (_ok()) {
            p.addInt(Tag::wwidth, width);
            if (height) p.addInt(Tag::wheight, height);
        }
        return *this;
    }

    // Заголовок виджета
    Widget& label(GHTREF str) {
        return param(Tag::label, str);
    }

    // Убрать заголовок виджета
    Widget& noLabel(bool nolabel = true) {
        return param(Tag::nolabel, nolabel);
    }

    // Дополнительный заголовок виджета справа
    Widget& suffix(GHTREF str) {
        return param(Tag::suffix, str);
    }

    // Убрать задний фон виджета
    Widget& noTab(bool notab = true) {
        return param(Tag::notab, notab);
    }

    // Сделать виджет квадратным
    Widget& square(bool square = true) {
        return param(Tag::square, square);
    }

    // Отключить виджет
    Widget& disabled(bool disable = true) {
        return param(Tag::disable, disable);
    }

    // Подсказка виджета. Пустая строка - убрать подсказку
    Widget& hint(GHTREF str) {
        return param(Tag::hint, str);
    }

    // ===================== ACTION =====================
    // Проверка на клик по виджету
    bool click() {
        return _click;
    }

    // ===================== ATTACH =====================
    // Подключить функцию вида void f()
    Widget& attach(std::function<void()> cb) {
        if (_click && cb) cb();
        return *this;
    }

    // Подключить функцию вида void f(gh::Build& build)
    Widget& attach(std::function<void(gh::Build& build)> cb) {
        if (_click && cb && _build) cb(*_build);
        return *this;
    }

    // Подключить функцию вида void f(gh::Builder& build)
    Widget& attach(std::function<void(gh::Builder& build)> cb, gh::Builder& b) {
        if (_click && cb && _build) cb(b);
        return *this;
    }

    // Подключить gh::Flag* флаг
    Widget& attach(gh::Flag* flag) {
        if (_click) flag->_changed = true;
        return *this;
    }

    // Подключить bool* флаг
    Widget& attach(bool* flag) {
        if (_click) *flag = true;
        return *this;
    }

   protected:
    ghc::Packet& p;
    gh::Build* _build;
    bool _enabled = true;
    bool _allowed = false;
    bool _click = false;

    bool _ok() {
        return _enabled && _allowed;
    }

    Widget& param(Tag key, const sutil::AnyValue& val) {
        if (_ok() && val.valid()) {
            if (val.type() == sutil::AnyText::Type::value) p.addInt(key, val);
            else p.addStringEsc(key, val);
        }
        return *this;
    }
    Widget& param(Tag key, double val, uint8_t dec) {
        if (_ok()) p.addFloat(key, val, dec);
        return *this;
    }
};

}  // namespace ghc
