#pragma once
#include <Arduino.h>
#include <Stamp.h>

#include "canvas.h"
#include "config.hpp"
#include "macro.hpp"
#include "utils/build.h"
#include "utils/color.h"
#include "utils/datatypes.h"
#include "utils/log.h"

class HubBuilder {
   public:
    // ========================== WIDGET ==========================
    void BeginWidgets(int height = 0) {
        if (_isUI()) {
            tab_width = 100;
            _add(F("{'type':'widget_b','height':"));
            *sptr += height;
            _end();
        }
    }
    void EndWidgets() {
        tab_width = 0;
        if (_isUI()) {
            _add(F("{'type':'widget_e'"));
            _end();
        }
    }
    void WidgetSize(int width) {
        tab_width = width;
    }

    // ========================== DUMMY ===========================
    bool Dummy(FSTR name, void* value = nullptr, GHdata_t type = GH_NULL) {
        return _dummy(true, name, value, type);
    }
    bool Dummy(CSREF name, void* value = nullptr, GHdata_t type = GH_NULL) {
        return _dummy(false, name.c_str(), value, type);
    }

    bool _dummy(bool fstr, VSPTR name, void* value, GHdata_t type) {
        if (_isRead()) {
            if (_checkName(name, fstr)) GHtypeToStr(sptr, value, type);
        } else if (bptr->type == GH_BUILD_ACTION) {
            return bptr->parseSet(name, value, type, fstr);
        }
        return 0;
    }

    // ========================== BUTTON ==========================
    bool Button(FSTR name, bool* value = nullptr, FSTR label = nullptr, uint32_t color = GH_DEFAULT, int size = 22) {
        return _button(true, F("button"), name, value, label, color, size);
    }
    bool Button(CSREF name, bool* value = nullptr, CSREF label = "", uint32_t color = GH_DEFAULT, int size = 22) {
        return _button(false, F("button"), name.c_str(), value, label.c_str(), color, size);
    }

    bool ButtonIcon(FSTR name, bool* value = nullptr, FSTR label = nullptr, uint32_t color = GH_DEFAULT, int size = 50) {
        return _button(true, F("button_i"), name, value, label, color, size);
    }
    bool ButtonIcon(CSREF name, bool* value = nullptr, CSREF label = "", uint32_t color = GH_DEFAULT, int size = 50) {
        return _button(false, F("button_i"), name.c_str(), value, label.c_str(), color, size);
    }

    bool _button(bool fstr, FSTR tag, VSPTR name, bool* value, VSPTR label, uint32_t color, int size) {
        if (_isUI()) {
            _begin(tag);
            _name(name, fstr);
            _label(label, fstr);
            _color(color);
            _size(size);
            _tabw();
            _end();
        } else if (bptr->type == GH_BUILD_ACTION) {
            return bptr->parseClick(name, value, fstr);
        }
        return 0;
    }

    // ========================== LABEL ==========================
    void Label(FSTR name, const String& value = "", FSTR label = nullptr, uint32_t color = GH_DEFAULT, int size = 40) {
        _label(true, name, value, label, color, size);
    }
    void Label(CSREF name, const String& value = "", CSREF label = "", uint32_t color = GH_DEFAULT, int size = 40) {
        _label(false, name.c_str(), value, label.c_str(), color, size);
    }
    void _label(bool fstr, VSPTR name, const String& value, VSPTR label, uint32_t color, int size) {
        if (_isUI()) {
            _begin(F("label"));
            _name(name, fstr);
            _value();
            _quot();
            *sptr += value;
            _quot();
            _label(label, fstr);
            _color(color);
            _size(size);
            _tabw();
            _end();
        } else if (_isRead()) {
            if (_checkName(name, fstr)) *sptr += value;
        }
    }

    // ========================== TITLE ==========================
    void Title(FSTR label) {
        _title(true, label);
    }
    void Title(CSREF label) {
        _title(false, label.c_str());
    }
    void _title(bool fstr, VSPTR label) {
        if (_isUI()) {
            _begin(F("title"));
            _label(label, fstr);
            _end();
        }
    }

    // ========================== LOG ==========================
    void Log(FSTR name, GHlog* log, FSTR label = nullptr) {
        _log(true, name, log, label);
    }
    void Log(CSREF name, GHlog* log, CSREF label = "") {
        _log(false, name.c_str(), log, label.c_str());
    }
    void _log(bool fstr, VSPTR name, GHlog* log, VSPTR label) {
        if (_isUI()) {
            _begin(F("log"));
            _name(name, fstr);
            _text();
            _quot();
            log->read(sptr);
            _quot();
            _label(label, fstr);
            _tabw();
            _end();
        } else if (_isRead()) {
            if (_checkName(name, fstr)) _escape(sptr, log->read());
        }
    }

    // ========================== DISPLAY ==========================
    void Display(FSTR name, const String& value = "", FSTR label = nullptr, uint32_t color = GH_DEFAULT, int rows = 2, int size = 40) {
        _display(true, name, value, label, color, rows, size);
    }
    void Display(CSREF name, const String& value = "", CSREF label = "", uint32_t color = GH_DEFAULT, int rows = 2, int size = 40) {
        _display(true, name.c_str(), value, label.c_str(), color, rows, size);
    }
    void _display(bool fstr, VSPTR name, const String& value, VSPTR label, uint32_t color, int rows, int size) {
        if (_isUI()) {
            _begin(F("display"));
            _name(name, fstr);
            _value();
            _quot();
            _escape(sptr, value);
            _quot();
            _label(label, fstr);
            _color(color);
            _tag(F("rows"));
            *sptr += rows;
            _size(size);
            _tabw();
            _end();
        } else if (_isRead()) {
            if (_checkName(name, fstr)) _escape(sptr, value);
        }
    }

    // ========================== HTML ==========================
    void HTML(FSTR name, const String& value = "", FSTR label = nullptr) {
        _html(true, name, value, label);
    }
    void HTML(CSREF name, const String& value = "", CSREF label = "") {
        _html(false, name.c_str(), value, label.c_str());
    }
    void _html(bool fstr, VSPTR name, const String& value, VSPTR label) {
        if (_isUI()) {
            _begin(F("html"));
            _name(name, fstr);
            _value();
            _quot();
            _escape(sptr, value);
            _quot();
            _label(label, fstr);
            _tabw();
            _end();
        } else if (_isRead()) {
            if (_checkName(name, fstr)) _escape(sptr, value);
        }
    }

    // =========================== JS ===========================
    void JS(const String& value = "") {
        if (_isUI()) {
            _begin(F("js"));
            _value();
            _quot();
            _escape(sptr, value);
            _quot();
            _end();
        }
    }

    // ========================== INPUT ==========================
    bool Input(FSTR name, void* value = nullptr, GHdata_t type = GH_NULL, FSTR label = nullptr, int maxv = 0, uint32_t color = GH_DEFAULT) {
        return _input(true, F("input"), name, value, type, label, maxv, color);
    }
    bool Input(CSREF name, void* value = nullptr, GHdata_t type = GH_NULL, CSREF label = "", int maxv = 0, uint32_t color = GH_DEFAULT) {
        return _input(false, F("input"), name.c_str(), value, type, label.c_str(), maxv, color);
    }

    // ========================== PASS ==========================
    bool Pass(FSTR name, void* value = nullptr, GHdata_t type = GH_NULL, FSTR label = nullptr, int maxv = 0, uint32_t color = GH_DEFAULT) {
        return _input(true, F("pass"), name, value, type, label, maxv, color);
    }
    bool Pass(CSREF name, void* value = nullptr, GHdata_t type = GH_NULL, CSREF label = "", int maxv = 0, uint32_t color = GH_DEFAULT) {
        return _input(false, F("pass"), name.c_str(), value, type, label.c_str(), maxv, color);
    }

    bool _input(bool fstr, FSTR tag, VSPTR name, void* value, GHdata_t type, VSPTR label, int maxv, uint32_t color) {
        if (_isUI()) {
            _begin(tag);
            _name(name, fstr);
            _value();
            _quot();
            GHtypeToStr(sptr, value, type);
            _quot();
            _label(label, fstr);
            if (maxv) _maxv((long)maxv);
            _color(color);
            _tabw();
            _end();
        } else if (_isRead()) {
            if (_checkName(name, fstr)) GHtypeToStr(sptr, value, type);
        } else if (bptr->type == GH_BUILD_ACTION) {
            return bptr->parseSet(name, value, type, fstr);
        }
        return 0;
    }

    // ========================== SLIDER ==========================
    bool Slider(FSTR name, void* value = nullptr, GHdata_t type = GH_NULL, FSTR label = nullptr, float minv = 0, float maxv = 100, float step = 1, uint32_t color = GH_DEFAULT) {
        return _spinner(true, F("slider"), name, value, type, label, minv, maxv, step, color);
    }
    bool Slider(CSREF name, void* value = nullptr, GHdata_t type = GH_NULL, CSREF label = "", float minv = 0, float maxv = 100, float step = 1, uint32_t color = GH_DEFAULT) {
        return _spinner(false, F("slider"), name.c_str(), value, type, label.c_str(), minv, maxv, step, color);
    }

    // ========================== SPINNER ==========================
    bool Spinner(FSTR name, void* value = nullptr, GHdata_t type = GH_NULL, FSTR label = nullptr, float minv = 0, float maxv = 100, float step = 1, uint32_t color = GH_DEFAULT) {
        return _spinner(true, F("spinner"), name, value, type, label, minv, maxv, step, color);
    }
    bool Spinner(CSREF name, void* value = nullptr, GHdata_t type = GH_NULL, CSREF label = "", float minv = 0, float maxv = 100, float step = 1, uint32_t color = GH_DEFAULT) {
        return _spinner(false, F("spinner"), name.c_str(), value, type, label.c_str(), minv, maxv, step, color);
    }

    bool _spinner(bool fstr, FSTR tag, VSPTR name, void* value, GHdata_t type, VSPTR label, float minv, float maxv, float step, uint32_t color) {
        if (_isUI()) {
            _begin(tag);
            _name(name, fstr);
            _value();
            GHtypeToStr(sptr, value, type);
            _label(label, fstr);
            _minv(minv);
            _maxv(maxv);
            _step(step);
            _color(color);
            _tabw();
            _end();
        } else if (_isRead()) {
            if (_checkName(name, fstr)) GHtypeToStr(sptr, value, type);
        } else if (bptr->type == GH_BUILD_ACTION) {
            return bptr->parseSet(name, value, type, fstr);
        }
        return 0;
    }

    // ========================== GAUGE ===========================
    void Gauge(FSTR name, float value = 0, FSTR text = nullptr, FSTR label = nullptr, float minv = 0, float maxv = 100, float step = 1, uint32_t color = GH_DEFAULT) {
        _gauge(true, name, value, text, label, minv, maxv, step, color);
    }
    void Gauge(CSREF name, float value = 0, CSREF text = "", CSREF label = "", float minv = 0, float maxv = 100, float step = 1, uint32_t color = GH_DEFAULT) {
        _gauge(true, name.c_str(), value, text.c_str(), label.c_str(), minv, maxv, step, color);
    }

    void _gauge(bool fstr, VSPTR name, float value, VSPTR text, VSPTR label, float minv, float maxv, float step, uint32_t color) {
        if (_isUI()) {
            _begin(F("gauge"));
            _name(name, fstr);
            _value();
            *sptr += value;
            _text(text, fstr);
            _label(label, fstr);
            _minv(minv);
            _maxv(maxv);
            _step(step);
            _color(color);
            _tabw();
            _end();
        } else if (_isRead()) {
            if (_checkName(name, fstr)) *sptr += value;
        }
    }

    // ========================== SWITCH ==========================
    bool Switch(FSTR name, bool* value = nullptr, FSTR label = nullptr, uint32_t color = GH_DEFAULT) {
        return _switch(true, F("switch"), name, value, label, color, nullptr);
    }
    bool Switch(CSREF name, bool* value = nullptr, CSREF label = "", uint32_t color = GH_DEFAULT) {
        return _switch(false, F("switch"), name.c_str(), value, label.c_str(), color, nullptr);
    }

    bool SwitchIcon(FSTR name, bool* value = nullptr, FSTR label = nullptr, FSTR text = nullptr, uint32_t color = GH_DEFAULT) {
        return _switch(true, F("switch_i"), name, value, label, color, text);
    }
    bool SwitchIcon(CSREF name, bool* value = nullptr, CSREF label = "", CSREF text = "", uint32_t color = GH_DEFAULT) {
        return _switch(false, F("switch_i"), name.c_str(), value, label.c_str(), color, text.c_str());
    }

    bool SwitchText(FSTR name, bool* value = nullptr, FSTR label = nullptr, FSTR text = nullptr, uint32_t color = GH_DEFAULT) {
        return _switch(true, F("switch_t"), name, value, label, color, text);
    }
    bool SwitchText(CSREF name, bool* value = nullptr, CSREF label = "", CSREF text = "", uint32_t color = GH_DEFAULT) {
        return _switch(false, F("switch_t"), name.c_str(), value, label.c_str(), color, text.c_str());
    }

    bool _switch(bool fstr, FSTR tag, VSPTR name, bool* value, VSPTR label, uint32_t color, VSPTR text) {
        if (_isUI()) {
            _begin(tag);
            _name(name, fstr);
            _value();
            GHtypeToStr(sptr, value, GH_BOOL);
            _label(label, fstr);
            _color(color);
            _text(text, fstr);
            _tabw();
            _end();
        } else if (_isRead()) {
            if (_checkName(name, fstr)) GHtypeToStr(sptr, value, GH_BOOL);
        } else if (bptr->type == GH_BUILD_ACTION) {
            return bptr->parseSet(name, value, GH_BOOL, fstr);
        }
        return 0;
    }

    // ========================== DATETIME ==========================
    bool Date(FSTR name, Stamp* value, FSTR label = nullptr, uint32_t color = GH_DEFAULT) {
        return _date(true, F("date"), name, value, label, color);
    }
    bool Date(CSREF name, Stamp* value, CSREF label = "", uint32_t color = GH_DEFAULT) {
        return _date(false, F("date"), name.c_str(), value, label.c_str(), color);
    }

    bool Time(FSTR name, Stamp* value, FSTR label = nullptr, uint32_t color = GH_DEFAULT) {
        return _date(true, F("time"), name, value, label, color);
    }
    bool Time(CSREF name, Stamp* value, CSREF label = "", uint32_t color = GH_DEFAULT) {
        return _date(false, F("time"), name.c_str(), value, label.c_str(), color);
    }

    bool DateTime(FSTR name, Stamp* value, FSTR label = nullptr, uint32_t color = GH_DEFAULT) {
        return _date(true, F("datetime"), name, value, label, color);
    }
    bool DateTime(CSREF name, Stamp* value, CSREF label = "", uint32_t color = GH_DEFAULT) {
        return _date(false, F("datetime"), name.c_str(), value, label.c_str(), color);
    }

    bool _date(bool fstr, FSTR tag, VSPTR name, Stamp* value, VSPTR label, uint32_t color) {
        if (_isUI()) {
            _begin(tag);
            _name(name, fstr);
            _label(label, fstr);
            _value();
            GHtypeToStr(sptr, value, GH_STAMP);
            _color(color);
            _tabw();
            _end();
        } else if (_isRead()) {
            if (_checkName(name, fstr)) GHtypeToStr(sptr, value, GH_STAMP);
        } else if (bptr->type == GH_BUILD_ACTION) {
            return bptr->parseSet(name, value, GH_STAMP, fstr);
        }
        return 0;
    }

    // ========================== SELECT ==========================
    bool Select(FSTR name, uint8_t* value, FSTR text, FSTR label = nullptr, uint32_t color = GH_DEFAULT) {
        return _select(true, name, value, text, label, color);
    }
    bool Select(CSREF name, uint8_t* value, CSREF text, CSREF label = "", uint32_t color = GH_DEFAULT) {
        return _select(false, name.c_str(), value, text.c_str(), label.c_str(), color);
    }

    bool _select(bool fstr, VSPTR name, uint8_t* value, VSPTR text, VSPTR label, uint32_t color) {
        if (_isUI()) {
            _begin(F("select"));
            _name(name, fstr);
            _value();
            GHtypeToStr(sptr, value, GH_UINT8);
            _text(text, fstr);
            _label(label, fstr);
            _color(color);
            _tabw();
            _end();
        } else if (_isRead()) {
            if (_checkName(name, fstr)) GHtypeToStr(sptr, value, GH_UINT8);
        } else if (bptr->type == GH_BUILD_ACTION) {
            return bptr->parseSet(name, value, GH_UINT8, fstr);
        }
        return 0;
    }

    // ========================== FLAGS ==========================
    bool Flags(FSTR name, GHflags* value = nullptr, FSTR text = nullptr, FSTR label = nullptr, uint32_t color = GH_DEFAULT) {
        return _flags(true, name, value, text, label, color);
    }
    bool Flags(CSREF name, GHflags* value = nullptr, CSREF text = "", CSREF label = "", uint32_t color = GH_DEFAULT) {
        return _flags(false, name.c_str(), value, text.c_str(), label.c_str(), color);
    }

    bool _flags(bool fstr, VSPTR name, GHflags* value, VSPTR text, VSPTR label, uint32_t color) {
        if (_isUI()) {
            _begin(F("flags"));
            _name(name, fstr);
            _value();
            GHtypeToStr(sptr, value, GH_FLAGS);
            _text(text, fstr);
            _label(label, fstr);
            _color(color);
            _tabw();
            _end();
        } else if (_isRead()) {
            if (_checkName(name, fstr)) GHtypeToStr(sptr, value, GH_FLAGS);
        } else if (bptr->type == GH_BUILD_ACTION) {
            return bptr->parseSet(name, value, GH_FLAGS, fstr);
        }
        return 0;
    }

    // ========================== COLOR ==========================
    bool Color(FSTR name, GHcolor* value = nullptr, FSTR label = nullptr) {
        return _color(true, name, value, label);
    }
    bool Color(CSREF name, GHcolor* value = nullptr, CSREF label = "") {
        return _color(false, name.c_str(), value, label.c_str());
    }

    bool _color(bool fstr, VSPTR name, GHcolor* value, VSPTR label) {
        if (_isUI()) {
            _begin(F("color"));
            _name(name, fstr);
            _value();
            GHtypeToStr(sptr, value, GH_COLOR);
            _label(label, fstr);
            _tabw();
            _end();
        } else if (_isRead()) {
            if (_checkName(name, fstr)) GHtypeToStr(sptr, value, GH_COLOR);
        } else if (bptr->type == GH_BUILD_ACTION) {
            return bptr->parseSet(name, value, GH_COLOR, fstr);
        }
        return 0;
    }

    // ========================== LED ==========================
    void LED(FSTR name, bool value = 0, FSTR label = nullptr, FSTR icon = nullptr) {
        _led(true, name, value, label, icon);
    }
    void LED(CSREF name, bool value = 0, CSREF label = "", CSREF icon = "") {
        _led(false, name.c_str(), value, label.c_str(), icon.c_str());
    }

    void _led(bool fstr, VSPTR name, bool value, VSPTR label, VSPTR text) {
        if (_isUI()) {
            _begin(F("led"));
            _name(name, fstr);
            _value();
            *sptr += value;
            _label(label, fstr);
            _text(text, fstr);
            _tabw();
            _end();
        } else if (_isRead()) {
            if (_checkName(name, fstr)) *sptr += value;
        }
    }

    // ========================== SPACE ==========================
    void Space(int height = 0) {
        if (_isUI()) {
            _begin(F("spacer"));
            _height(height);
            _tabw();
            _end();
        }
    }

    // ========================== TABS ==========================
    bool Tabs(FSTR name, uint8_t* value, FSTR text, FSTR label = nullptr) {
        return _tabs(true, name, value, text, label);
    }
    bool Tabs(CSREF name, uint8_t* value, CSREF text, CSREF label = "") {
        return _tabs(false, name.c_str(), value, text.c_str(), label.c_str());
    }

    bool _tabs(bool fstr, VSPTR name, uint8_t* value, VSPTR text, VSPTR label) {
        if (_isUI()) {
            _begin(F("tabs"));
            _name(name, fstr);
            _value();
            *sptr += *value;
            _text(text, fstr);
            _label(label, fstr);
            _tabw();
            _end();
        } else if (bptr->type == GH_BUILD_ACTION) {
            bool act = bptr->parseSet(name, value, GH_UINT8, fstr);
            if (act) refresh();
            return act;
        }
        return 0;
    }

    // ========================= CANVAS =========================
    void Canvas(FSTR name, int size = 500, GHcanvas* cv = nullptr, FSTR label = nullptr) {
        _canvas(true, name, size, cv, label, false);
    }
    void Canvas(CSREF name, int size = 500, GHcanvas* cv = nullptr, CSREF label = "") {
        _canvas(false, name.c_str(), size, cv, label.c_str(), false);
    }
    void BeginCanvas(FSTR name, int size = 500, GHcanvas* cv = nullptr, FSTR label = nullptr) {
        _canvas(false, name, size, cv, label, true);
    }
    void BeginCanvas(CSREF name, int size = 500, GHcanvas* cv = nullptr, CSREF label = "") {
        _canvas(false, name.c_str(), size, cv, label.c_str(), true);
    }
    void EndCanvas() {
        if (_isUI()) {
            *sptr += ']';
            _tabw();
            _end();
        }
    }

    void _canvas(bool fstr, VSPTR name, int size, GHcanvas* cv, VSPTR label, bool begin) {
        if (_isUI()) {
            _begin(F("canvas"));
            _name(name, fstr);
            _size(size);
            _label(label, fstr);
            _value();
            *sptr += '[';
            if (begin && cv) cv->extBuffer(sptr);
            else EndCanvas();
        } else {
            cv->extBuffer(nullptr);
        }
    }

   protected:
    String* sptr = nullptr;
    GHbuild* bptr = nullptr;
    virtual void _afterComponent() = 0;
    virtual void refresh() = 0;

    // ========================== PRIVATE ==========================
   private:
    int tab_width = 0;

    void _escape(String* s, const String& v) {
        for (uint16_t i = 0; i < v.length(); i++) GH_escapeChar(s, v[i]);
    }
    bool _checkName(VSPTR name, bool fstr = true) {
        if (fstr ? (!strcmp_P(bptr->action.name, (PGM_P)name)) : (!strcmp(bptr->action.name, (PGM_P)name))) {
            bptr->type = GH_BUILD_NONE;
            return true;
        }
        return false;
    }
    bool _isUI() {
        return (bptr && sptr && (bptr->type == GH_BUILD_UI || bptr->type == GH_BUILD_COUNT));
    }
    bool _isRead() {
        return (bptr && sptr && bptr->type == GH_BUILD_READ);
    }

    // ================
    void _add(FSTR arg) {
        *sptr += arg;
    }
    void _begin(FSTR type) {
        *sptr += F("{'type':'");
        *sptr += type;
        _quot();
    }
    void _end() {
        *sptr += '}';
        _afterComponent();
        *sptr += ',';
    }
    void _quot() {
        *sptr += '\'';
    }
    void _tabw() {
        if (tab_width) {
            *sptr += F(",'tab_w':");
            *sptr += tab_width;
        }
    }

    // ================
    void _value() {
        *sptr += F(",'value':");
    }
    void _value(VSPTR value, bool fstr = true) {
        _value();
        _quot();
        if (value) {
            if (fstr) *sptr += (FSTR)value;
            else *sptr += (PGM_P)value;
        }
        _quot();
    }

    // ================
    void _name(VSPTR name, bool fstr = true) {
        *sptr += F(",'name':'");
        if (name) {
            if (fstr) *sptr += (FSTR)name;
            else *sptr += (PGM_P)name;
        }
        _quot();
    }
    void _label(VSPTR label, bool fstr = true) {
        *sptr += F(",'label':'");
        if (label) {
            if (fstr) *sptr += (FSTR)label;
            else *sptr += (PGM_P)label;
        }
        _quot();
    }

    // ================
    void _text() {
        *sptr += F(",'text':");
    }
    void _text(VSPTR text, bool fstr = true) {
        _text();
        _quot();
        if (text) {
            if (fstr) *sptr += (FSTR)text;
            else *sptr += (PGM_P)text;
        }
        _quot();
    }

    // ================
    void _height(int& height) {
        *sptr += F(",'height':");
        *sptr += height;
    }
    void _color(uint32_t& col) {
        if (col == GH_DEFAULT) return;
        *sptr += F(",'color':");
        *sptr += col;
    }
    void _size(int& val) {
        *sptr += F(",'size':");
        *sptr += val;
    }

    // ================
    void _minv(float val) {
        *sptr += F(",'min':");
        *sptr += val;
    }

    void _maxv(float val) {
        *sptr += F(",'max':");
        *sptr += val;
    }

    void _step(float val) {
        *sptr += F(",'step':");
        *sptr += val;
    }

    // ================
    void _tag(FSTR tag) {
        *sptr += F(",'");
        *sptr += tag;
        *sptr += F("':");
    }
};