#pragma once
#include <Arduino.h>
#include <StringUtils.h>

#ifndef GH_NO_PAIRS
#include <Pairs.h>
#endif

#include "builder_class.h"
#include "builder_macro.h"
#include "button.h"
#include "canvas.h"
#include "core/build.h"
#include "core/client.h"
#include "core/core_class.h"
#include "core/datatypes.h"
#include "core/hooks.h"
#include "core/hub_class.h"
#include "core/packet.h"
#include "core/types.h"
#include "flags.h"
#include "geo.h"
#include "hub_macro.hpp"
#include "log.h"
#include "namer.h"
#include "pos.h"
#include "widget.h"

namespace gh {
class Builder {
    friend class ghc::HubCore;
    friend class ::GyverHub;

   public:
    Build& build;
    ghc::Widget widget;

    Builder(Build& build,
            ghc::Packet* p = nullptr) : build(build),
                                        widget(*p, &build),
                                        p(p),
                                        _namer(build.name) {}

    // ========================== SYSTEM ==========================

    // обновить панель управления (по действию с виджета)
    void refresh() {
        _refresh = true;
    }

    // был ли запрос на обновление панели управления
    bool isRefresh() {
        return _refresh;
    }

    // включить/выключить вывод виджетов (только для запроса виджетов)
    bool show(bool en = true) {
        widget._enabled = en;
        return true;
    }

    // было изменено значение виджета (сигнал на сохранение)
    bool changed() {
        return _changed;
    }

    // был клик по предыдущему виджету
    bool click() {
        return widget.click();
    }

    // номер текущего пункта меню
    uint8_t menu() {
        return _menu ? *_menu : 0;
    }

    operator ghc::Packet*() {
        return (build.action == Action::UI) ? p : nullptr;
    }

    // ======================== CONTAINER ========================

    // горизонтальный контейнер
    GH_BUILD_CONTAINER(Row, ghc::Tag::container, ghc::Tag::row);

    // вертикальный контейнер
    GH_BUILD_CONTAINER(Col, ghc::Tag::container, ghc::Tag::col);

    // горизонтальный спойлер
    GH_BUILD_CONTAINER(SpoilerRow, ghc::Tag::spoiler, ghc::Tag::row);

    // вертикальный спойлер
    GH_BUILD_CONTAINER(SpoilerCol, ghc::Tag::spoiler, ghc::Tag::col);

    // ======================== CUSTOM ========================

    // начать кастомный контейнер
    bool beginRow(GHTREF wtype, uint16_t width = 0, GHTREF label = GHTXT(), gh::Color color = gh::Colors::Default) {
        return _beginContainer(ghc::Tag::none, wtype, ghc::Tag::row, width, label, color);
    }
    bool beginCol(GHTREF wtype, uint16_t width = 0, GHTREF label = GHTXT(), gh::Color color = gh::Colors::Default) {
        return _beginContainer(ghc::Tag::none, wtype, ghc::Tag::col, width, label, color);
    }

    // следующий кастомный контейнер
    bool nextRow(GHTREF wtype, uint16_t width = 0, GHTREF label = GHTXT(), gh::Color color = gh::Colors::Default) {
        return _nextContainer(ghc::Tag::none, wtype, ghc::Tag::row, width, label, color);
    }
    bool nextCol(GHTREF wtype, uint16_t width = 0, GHTREF label = GHTXT(), gh::Color color = gh::Colors::Default) {
        return _nextContainer(ghc::Tag::none, wtype, ghc::Tag::col, width, label, color);
    }

    // закончить любой контейнер
    void endContainer() {
        if (!p || !_allowContainer()) return;
        if (_checkFirst()) p->endObj();
        p->endArr();
        _first = false;
    }

    // ======================== SPACE ========================

    // Пустой виджет. Параметры: size, square
    ghc::Widget& Space(uint16_t width = 1, uint16_t height = 1) {
        _namer.check();
        if (build.action == Action::UI && widget._enabled) {
            _beginName();
            _type(ghc::Tag::space);
            widget.size(width, height);
        }
        return widget;
    }

    // ========================= WIDGETS =========================

    // параметры виджета: noTab, label, noLabel, size, square, hint, suffix

    // =========== ВВОД ============

    // Поле ввода текста. Параметры: value (текст), color, regex, maxLen, disabled, attach, click + параметры виджета
    GH_BUILD_VAR(Input, ghc::Tag::input);

    // Поле ввода текста. Параметры: value (текст), rows, maxLen, disabled, attach, click + параметры виджета
    GH_BUILD_VAR(InputArea, ghc::Tag::area);

    // Поле ввода пароля. Параметры: value (текст), color, regex, maxLen, disabled, attach, click + параметры виджета
    GH_BUILD_VAR(Pass, ghc::Tag::pass);

    // Дата. Параметры: value(stamp), color, disabled, attach, click + параметры виджета
    GH_BUILD_VAR(Date, ghc::Tag::date);

    // Время. Параметры: value(stamp), color, disabled, attach, click + параметры виджета
    GH_BUILD_VAR(Time, ghc::Tag::time);

    // Дата и время. Параметры: value(stamp), color, disabled, attach, click + параметры виджета
    GH_BUILD_VAR(DateTime, ghc::Tag::datetime);

    // Слайдер. Параметры: value (значение), color, range, unit, disabled, attach, click + параметры виджета
    GH_BUILD_VAR(Slider, ghc::Tag::slider);

    // Спиннер. Параметры: value (значение), range, unit, disabled, attach, click + параметры виджета
    GH_BUILD_VAR(Spinner, ghc::Tag::spinner);

    // Выпадающий список. Параметры: value (номер пункта), text (список), color, disabled, attach, click + параметры виджета
    GH_BUILD_VAR(Select, ghc::Tag::select);

    // Цвет. Параметры: value (цвет), disabled, attach, click + параметры виджета
    GH_BUILD_VAR(Color, ghc::Tag::color);

    // Выключатель. Параметры: value (состояние), color, disabled, attach, click + параметры виджета
    GH_BUILD_VAR(Switch, ghc::Tag::switch_t);

    // Выключатель. Параметры: value (состояние), icon, color, fontSize, disabled, attach, click + параметры виджета
    GH_BUILD_VAR(SwitchIcon, ghc::Tag::switch_i);

    // Вкладки. Параметры: value (выбранная), text (список), color, disabled, attach, click + параметры виджета. text не обновляется
    GH_BUILD_VAR(Tabs, ghc::Tag::tabs);

    // Всплывающее окно подтверждения. Параметры: text (подпись), attach, click. Для активации отправь обновление action()
    GH_BUILD_VAR(Confirm, ghc::Tag::confirm);

    // Всплывающее окно ввода. Параметры: value (значение), text (подпись), attach, click. Для активации отправь обновление action()
    GH_BUILD_VAR(Prompt, ghc::Tag::prompt);

    // Кнопка. Параметры: icon, color, fontSize, disabled, attach, click + параметры виджета
    ghc::Widget& Button(gh::Button* btn = nullptr) {
        return Button_(GHTXT(), btn);
    }

    ghc::Widget& Button_(GHTREF name, gh::Button* btn = nullptr) {
        _namer.check(name);
        switch (build.action) {
            case Action::UI:
                if (!widget._enabled) break;
                _beginName(name);
                _type(ghc::Tag::button);
                break;

            case Action::Set:
                widget._click = 0;
                if (_namer.equals(name)) {
                    widget._click = (build.value[0] == '0');
                    _needs_update = false;

                    if (btn) {
                        btn->_change();
                        btn->_state = !widget._click;
                        btn->_click = widget._click;
                    }
                }
                break;
            default:
                break;
        }
        return widget;
    }

    // Джойстик. keep - возвращать в центр, exp - экспоненциальные значения. Параметры: color + параметры виджета
    ghc::Widget& Joystick_(GHTREF name = GHTXT(), Pos* pos = nullptr, bool keep = 0, bool exp = 0) {
        return _joy_pad(ghc::Tag::joy, name, pos, keep, exp);
    }
    ghc::Widget& Joystick(Pos* pos = nullptr, bool keep = 0, bool exp = 0) {
        return Joystick_(GHTXT(), pos, keep, exp);
    }

    // Крестовина. Параметры: color + параметры виджета
    ghc::Widget& Dpad_(GHTREF name = GHTXT(), Pos* pos = nullptr) {
        return _joy_pad(ghc::Tag::dpad, name, pos);
    }
    ghc::Widget& Dpad(Pos* pos = nullptr) {
        return Dpad_(GHTXT(), pos);
    }

    // ======== ПАССИВНЫЕ ========

    // Заголовок. Параметры: value (текст), icon, color, align, fontSize + size виджета
    GH_BUILD_VAL(Title, ghc::Tag::title);

    // Надпись. Параметры: value (текст), icon, color, align, fontSize + параметры виджета
    GH_BUILD_VAL(Label, ghc::Tag::label);

    // Окно с текстом. Параметры: value (текст), rows + параметры виджета
    GH_BUILD_VAL(Text, ghc::Tag::text);

    // Окно с текстом. Параметры: value (путь), rows + параметры виджета
    GH_BUILD_VAL(TextFile, ghc::Tag::text_f);

    // Дисплей. Параметры: value (текст), color, fontSize, rows + параметры виджета
    GH_BUILD_VAL(Display, ghc::Tag::display);

    // Картинка. Параметры: value (путь) + параметры виджета
    GH_BUILD_VAL(Image, ghc::Tag::image);

    // Светодиод. Параметры: value (состояние 1/0), color + параметры виджета
    GH_BUILD_VAR(LED, ghc::Tag::led);

    // Светодиод-иконка. Параметры: value (состояние 1/0), icon, fontSize, color + параметры виджета
    GH_BUILD_VAR(Icon, ghc::Tag::icon);

    // Индикаторная шкала. Параметры: value (значение), range, unit, color + параметры виджета
    GH_BUILD_VAR(Gauge, ghc::Tag::gauge);

    // Индикаторная шкала круглая. Параметры: value (значение), range, unit, color + параметры виджета
    GH_BUILD_VAR(GaugeRound, ghc::Tag::gauge_r);

    // Индикаторная шкала линейная. Параметры: value (значение), icon, range, unit, color + параметры виджета
    GH_BUILD_VAR(GaugeLinear, ghc::Tag::gauge_l);

    // Лог. value(текст), rows + параметры виджета
    GH_BUILD_VAR_TYPE(Log, gh::Log*, ghc::Tag::log);

    // Кнопки выбора. Параметры: value (флаги), text (список), color, disabled, attach, click + параметры виджета
    GH_BUILD_VAR_TYPE(Flags, gh::Flags*, ghc::Tag::flags);

    // Таблица. Параметры: value (текст или путь) + параметры виджета
    // text: таблица в формате CSV - разделитель столбцов ; разделитель строк \n
    // width: ширина, список чисел в процентах (например "30;30;50")
    // align: выравнивание, список из left | center | right (например "left;right")
    ghc::Widget& Table_(GHTREF name, GHTREF text, GHTREF width = GHTXT(), GHTREF align = GHTXT()) {
        _varAndType(ghc::Tag::table, name, &text);
        if (width.valid()) widget.param(ghc::Tag::width, width);
        if (align.valid()) widget.param(ghc::Tag::align, align);
        return widget;
    }
    ghc::Widget& Table(GHTREF text, GHTREF width = GHTXT(), GHTREF align = GHTXT()) {
        return Table_(GHTXT(), text, width, align);
    }

    // httpd стрим, без параметров
    // TODO
    void Stream(uint16_t port = 82) {
        _namer.check();
        switch (build.action) {
            case Action::UI:
                if (!widget._enabled) break;
                _beginName();
                _type(ghc::Tag::stream);
                widget.param(ghc::Tag::port, port);
                break;

            default:
                break;
        }
    }

    // =========================== MAP ===========================
    // Карта. Параметры: disabled, attach, click + параметры виджета
    ghc::Widget& Map(gh::Geo* geo = nullptr, gh::Layer layer = gh::Layer::OSM) {
        return _map(GHTXT(), geo, layer);
    }
    ghc::Widget& Map_(GHTREF name, gh::Geo* geo = nullptr, gh::Layer layer = gh::Layer::OSM) {
        return _map(name, geo, layer);
    }

    ghc::Widget& _map(GHTREF name, gh::Geo* geo = nullptr, gh::Layer layer = gh::Layer::OSM) {
        _namer.check(name);

        switch (build.action) {
            case Action::UI:
                if (!widget._enabled) break;
                _beginName(name);
                _type(ghc::Tag::map);
                if (geo) {
                    widget.value(*geo);
                    widget.param(ghc::Tag::active, 1);
                }
                if (layer != gh::Layer::OSM) widget.param(ghc::Tag::layer, (uint8_t)layer);
                break;

            case Action::Set:
                widget._click = _namer.equals(name);
                if (widget._click) {
                    _needs_update = false;
                    if (geo) *geo = gh::Geo(build.value, true);
                }
                break;

            default:
                break;
        }
        return widget;
    }

    // ========================= CANVAS =========================
    // Холст, рисование. Ширина, длина. Только араметры виджета
    ghc::Widget& Canvas(uint16_t width = 400, uint16_t height = 300, Pos* pos = nullptr) {
        return _canvas(GHTXT(), width, height, pos);
    }
    ghc::Widget& Canvas_(GHTREF name = GHTXT(), uint16_t width = 400, uint16_t height = 300, Pos* pos = nullptr) {
        return _canvas(name, width, height, pos);
    }

    ghc::Widget& _canvas(GHTREF name = GHTXT(), uint16_t width = 400, uint16_t height = 300, Pos* pos = nullptr) {
        _namer.check(name);

        switch (build.action) {
            case Action::UI:
                if (!widget._enabled) break;
                _beginName(name);
                _type(ghc::Tag::canvas);
                widget.param(ghc::Tag::width, width);
                widget.param(ghc::Tag::height, height);
                if (pos) widget.param(ghc::Tag::active, 1);
                break;

            case Action::Set:
                widget._click = _namer.equals(name);
                if (widget._click) {
                    _needs_update = false;
                    if (pos) *pos = Pos(build.value, true);
                }
                break;

            default:
                break;
        }
        return widget;
    }

    // ========= СИСТЕМА =========

    // Меню. Передать пункты меню списком "пункт1;пункт2" + attach, click
    ghc::Widget& Menu(GHTREF text) {
        widget._click = 0;
        if (_menu) {
            if (_varAndType(ghc::Tag::menu, F("_menu"), _menu).text(text).click()) {
                refresh();
                _needs_update = false;
            }
        }
        return widget;
    }

    // ============== КАСТОМ ==============

    // невидимый компонент для mqtt. attach, click
    GH_BUILD_VAR(Dummy, ghc::Tag::dummy);

    // HTML код в виджет. Параметры: (код или /путь.html) + параметры виджета
    GH_BUILD_VAL(HTML, ghc::Tag::html);

    // плагин в виде js кода или путь к файлу
    void Plugin(GHTREF wtype, GHTREF text) {
        _namer.check();
        if (build.action == Action::UI && widget._enabled) {
            _beginName();
            _type(ghc::Tag::plugin);
            widget.param(ghc::Tag::wtype, wtype);
            widget.param(ghc::Tag::value, text);
        }
    }

    // кастомный виджет, указать тип из класса виджета. Можно использовать любые параметры виджета
    ghc::Widget& Widget(GHTREF wtype, GHVREF text = GHVAL()) {
        return _varAndType(ghc::Tag::none, GHTXT(), &text, wtype);
    }
    ghc::Widget& Widget(GHTREF wtype, ghc::AnyPtr ptr) {
        return _varAndType(ghc::Tag::none, GHTXT(), ptr, wtype);
    }
    ghc::Widget& Widget_(GHTREF name, GHTREF wtype, GHVREF text = GHVAL()) {
        return _varAndType(ghc::Tag::none, name, &text, wtype);
    }
    ghc::Widget& Widget_(GHTREF name, GHTREF wtype, ghc::AnyPtr ptr) {
        return _varAndType(ghc::Tag::none, name, ptr, wtype);
    }

    /*
        // подключить переменную к виджету, созданному из JSON. Можно навесить attach и click на взаимодействие. Update соответственно виджету
        ghc::Widget& Hook_(GHTREF name, const ghc::AnyPtr& data, GHTREF func = GHTXT()) {
            return _varAndType(ghc::Tag::hook, name, data, func);
        }

        // добавить виджеты из JSON строки
        void addJSON(GHTREF text) {
            if (p && build.action == Action::UI) {
                if (_checkFirst()) p->endObj();
                p->sendText(text);
                p->comma();
                p->beginObj();
            }
        }

        // добавить виджеты из JSON строки PROGMEM
        void addJSON_P(PGM_P text) {
            addJSON((FSTR)text);
        }

        // добавить виджеты из JSON из файла
        void addJSON_File(GHTREF path) {
            _namer.check();
            switch (build.action) {
                case Action::UI:
                    _beginName();
                    _type(ghc::Tag::ui_file);
                    widget.param(ghc::Tag::value, path);
                    break;

                default:
                    break;
            }
        }
    */
    // =========================== CORE ===========================
   private:
    ghc::Widget& _varAndType(ghc::Tag type, GHTREF name, const ghc::AnyPtr& data, GHTREF wtype = GHTXT()) {
        _namer.check(name);
        switch (build.action) {
            case Action::UI:
                if (!widget._enabled) break;
                _beginName(name);
                if (type != ghc::Tag::none) _type(type);
                else if (wtype.valid()) p->addString(ghc::Tag::type, wtype);
                if (data.ptr && data.type == Type::PAIRS_T) {
#ifndef GH_NO_PAIRS
                    Pair pair = ((Pairs*)data.ptr)->get(name);
                    if (pair.valid()) widget.valueVar(ghc::AnyPtr(&pair));
#endif
                } else {
                    widget.valueVar(data);
                }
                break;

            case Action::Get:
                if (name.valid() && data.ptr) {
                    p->addKey(name);
                    p->quotes();
                    _writeValue(name, data);
                    p->quotes();
                    p->comma();
                }
                break;

            case Action::Read:
                if (_namer.equals(name) && data.ptr) _writeValue(name, data);
                break;

            case Action::Set:
                if (data.type == Type::TEXT_T) break;
                widget._click = _namer.equals(name);
                if (widget._click) {
                    if (data.ptr && data.type == Type::PAIRS_T) {
#ifndef GH_NO_PAIRS
                        ((Pairs*)data.ptr)->set(name, build.value);
#endif
                    } else {
                        ghc::strToVar(build.value, data);
                    }
                    _changed = true;
                }

                break;

            default:
                break;
        }
        return widget;
    }
    ghc::Widget& _joy_pad(ghc::Tag wtype, GHTREF name, Pos* pos = nullptr, bool keep = 0, bool exp = 0) {
        _namer.check(name);
        switch (build.action) {
            case Action::UI:
                if (!widget._enabled) break;
                _beginName(name);
                _type(wtype);
                if (keep) widget.param(ghc::Tag::keep, 1);
                if (exp) widget.param(ghc::Tag::exp, 1);
                break;

            case Action::Set:
                widget._click = _namer.equals(name);
                if (widget._click) {
                    _needs_update = false;
                    if (pos) *pos = Pos(build.value, true);
                }
                break;
            default:
                break;
        }
        return widget;
    }
    // =========================== SYS ===========================
    ghc::Packet* p;
    ghc::Namer _namer;
    bool _first = true;
    bool _refresh = false;
    bool _changed = false;
    bool _needs_update = true;
    uint8_t* _menu = nullptr;

    // system
    void _type(const ghc::Tag& wtype) {
        p->addString(ghc::Tag::type, wtype);
    }
    bool _checkFirst() {
        if (_first) return _first = false;
        return 1;
    }
    void _writeValue(GHTREF name, const ghc::AnyPtr& data) {
        if (data.type == Type::PAIRS_T) {
#ifndef GH_NO_PAIRS
            Pair pair = ((Pairs*)data.ptr)->get(name);
            if (!pair.valid()) pair = ((Pairs*)data.ptr)->add(name, "");
            if (pair.valid()) ghc::varToStr(*p, ghc::AnyPtr(&pair));
#endif
        } else {
            ghc::varToStr(*p, data);
        }
    }

    // named widget
    void _beginName(GHTREF name = GHTXT()) {
        if (_checkFirst()) p->endObj();
        p->beginObj();
        p->addKey(ghc::Tag::id);
        p->quotes();
        if (name.length()) {
            name.addString(*p);
        } else {
            _namer.addIndex(p->s);
        }
        p->quotes();
        p->comma();
    }

    // container (wtype, row/col)
    bool _beginContainer(ghc::Tag type, GHTREF wtype, ghc::Tag rowcol, uint16_t width = 0, GHTREF label = GHTXT(), gh::Color color = gh::Colors::Default) {
        if (!p || !_allowContainer()) return true;
        if (_checkFirst()) p->endObj();
        p->beginObj();
        if (type != ghc::Tag::none) p->addString(ghc::Tag::type, type);
        else if (wtype.valid()) p->addString(ghc::Tag::type, wtype);
        p->addString(ghc::Tag::rowcol, rowcol);
        if (width) p->addInt(ghc::Tag::wwidth, width);
        if (label.valid()) p->addString(ghc::Tag::label, label);
        if (!color.isDefault()) p->addString(ghc::Tag::color, su::Value((uint32_t)color, HEX));
        p->beginArr(ghc::Tag::data);
        _first = true;
        return true;
    }
    bool _nextContainer(ghc::Tag type, GHTREF wtype, ghc::Tag rowcol, uint16_t width = 0, GHTREF label = GHTXT(), gh::Color color = gh::Colors::Default) {
        if (!p || !_allowContainer()) return true;
        endContainer();
        _beginContainer(type, wtype, rowcol, width, label, color);
        return true;
    }
    bool _allowContainer() {
        return build.action == Action::UI && widget._enabled;
    }
};

}  // namespace gh