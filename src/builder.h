#pragma once
#include <Arduino.h>

#include "canvas.h"
#include "config.hpp"
#include "macro.hpp"
#include "utils/build.h"
#include "utils/button.h"
#include "utils/color.h"
#include "utils/datatypes.h"
#include "utils/log.h"
#include "utils/misc.h"
#include "utils/pos.h"

#define GH_ROW(h) h.BeginRow();
#define GH_ROW_END(h) h.EndRow();

class HubBuilder {
   public:
    // ========================== WIDGET ==========================
    void BeginRow(int height = 0) {
        if (_isUI()) {
            tab_width = 100;
            _add(F("{\"type\":\"row_b\",\"height\":"));
            *sptr += height;
            _end();
        }
    }
    void BeginWidgets(int height = 0) {
        BeginRow(height);
    }

    void EndRow() {
        tab_width = 0;
        if (_isUI()) {
            _add(F("{\"type\":\"row_e\""));
            _end();
        }
    }
    void EndWidgets() {
        EndRow();
    }

    void WidgetSize(int width) {
        tab_width = width;
    }

    // ========================== DUMMY ===========================
    bool Dummy_(FSTR name, void* var = nullptr, GHdata_t type = GH_NULL) {
        return _dummy(true, name, var, type);
    }
    bool Dummy_(CSREF name, void* var = nullptr, GHdata_t type = GH_NULL) {
        return _dummy(false, name.c_str(), var, type);
    }
    bool Dummy(void* var = nullptr, GHdata_t type = GH_NULL) {
        return Dummy_(0, var, type);
    }

    bool _dummy(bool fstr, VSPTR name, void* var, GHdata_t type) {
        if (_nameAuto(name, fstr)) name = nullptr;
        if (_isRead()) {
            if (_checkName(name, fstr)) GHtypeToStr(sptr, var, type);
        } else if (bptr->type == GH_BUILD_ACTION) {
            return bptr->parse(name, var, type, fstr);
        }
        return 0;
    }

    // ========================== BUTTON ==========================
    bool Button_(FSTR name, GHbutton* var = nullptr, FSTR label = nullptr, uint32_t color = GH_DEFAULT, int size = 20) {
        return _button(true, F("button"), name, var, label, color, size);
    }
    bool Button_(CSREF name, GHbutton* var = nullptr, CSREF label = "", uint32_t color = GH_DEFAULT, int size = 20) {
        return _button(false, F("button"), name.c_str(), var, label.c_str(), color, size);
    }
    bool Button(GHbutton* var = nullptr) {
        return Button_(0, var);
    }
    bool Button(GHbutton* var, FSTR label, uint32_t color = GH_DEFAULT, int size = 20) {
        return Button_(0, var, label, color, size);
    }
    bool Button(GHbutton* var, CSREF label, uint32_t color = GH_DEFAULT, int size = 20) {
        return Button_("", var, label.c_str(), color, size);
    }

    bool ButtonIcon_(FSTR name, GHbutton* var = nullptr, FSTR label = nullptr, uint32_t color = GH_DEFAULT, int size = 50) {
        return _button(true, F("button_i"), name, var, label, color, size);
    }
    bool ButtonIcon_(CSREF name, GHbutton* var = nullptr, CSREF label = "", uint32_t color = GH_DEFAULT, int size = 50) {
        return _button(false, F("button_i"), name.c_str(), var, label.c_str(), color, size);
    }
    bool ButtonIcon(GHbutton* var = nullptr) {
        return ButtonIcon_(0, var);
    }
    bool ButtonIcon(GHbutton* var, FSTR label, uint32_t color = GH_DEFAULT, int size = 50) {
        return ButtonIcon_(0, var, label, color, size);
    }
    bool ButtonIcon(GHbutton* var, CSREF label, uint32_t color = GH_DEFAULT, int size = 50) {
        return ButtonIcon_("", var, label.c_str(), color, size);
    }

    bool _button(bool fstr, FSTR tag, VSPTR name, GHbutton* var, VSPTR label, uint32_t color, int size) {
        if (_nameAuto(name, fstr)) name = nullptr;
        if (_isUI()) {
            _begin(tag);
            _name(name, fstr);
            _label(label, fstr);
            _color(color);
            _size(size);
            _tabw();
            _end();
        } else if (bptr->type == GH_BUILD_ACTION) {
            bool act = bptr->parse(name, &(var->state), GH_BOOL, fstr);
            if (!act) return 0;
            bool click = bptr->action.value[0] == '2';
            if (var && !click) var->_changed = 1;
            return click;
        }
        return 0;
    }

    // ========================== LABEL ==========================
    void Label_(FSTR name, CSREF value = "", FSTR label = nullptr, uint32_t color = GH_DEFAULT, int size = 40) {
        _label(true, name, value, label, color, size);
    }
    void Label_(CSREF name, CSREF value = "", CSREF label = "", uint32_t color = GH_DEFAULT, int size = 40) {
        _label(false, name.c_str(), value, label.c_str(), color, size);
    }

    void Label(CSREF value = "") {
        Label_(0, value);
    }
    void Label(CSREF value, FSTR label, uint32_t color = GH_DEFAULT, int size = 40) {
        Label_(0, value, label, color, size);
    }
    void Label(CSREF value, CSREF label, uint32_t color = GH_DEFAULT, int size = 40) {
        Label_("", value, label.c_str(), color, size);
    }

    void _label(bool fstr, VSPTR name, CSREF value, VSPTR label, uint32_t color, int size) {
        if (_nameAuto(name, fstr)) name = nullptr;
        if (_isUI()) {
            _begin(F("label"));
            _name(name, fstr);
            _value(value.c_str(), 0);
            _label(label, fstr);
            _color(color);
            _size(size);
            _tabw();
            _end();
        } else if (_isRead()) {
            if (_checkName(name, fstr)) GH_addEsc(sptr, value.c_str(), 0);
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
    void Log_(FSTR name, GHlog* log, FSTR label = nullptr) {
        _log(true, name, log, label);
    }
    void Log_(CSREF name, GHlog* log, CSREF label = "") {
        _log(false, name.c_str(), log, label.c_str());
    }

    void Log(GHlog* log) {
        Log_(0, log);
    }
    void Log(GHlog* log, FSTR label) {
        Log_(0, log, label);
    }
    void Log(GHlog* log, CSREF label) {
        Log_("", log, label.c_str());
    }

    void _log(bool fstr, VSPTR name, GHlog* log, VSPTR label) {
        if (_nameAuto(name, fstr)) name = nullptr;
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
            if (_checkName(name, fstr)) log->read(sptr);
        }
    }

    // ========================== DISPLAY ==========================
    void Display_(FSTR name, FSTR value = nullptr, FSTR label = nullptr, uint32_t color = GH_DEFAULT, int rows = 2, int size = 40) {
        _display(true, name, value, label, color, rows, size);
    }
    void Display_(CSREF name, CSREF value = "", CSREF label = "", uint32_t color = GH_DEFAULT, int rows = 2, int size = 40) {
        _display(false, name.c_str(), value.c_str(), label.c_str(), color, rows, size);
    }

    void Display() {
        Display_(0);
    }
    void Display(FSTR value, FSTR label = nullptr, uint32_t color = GH_DEFAULT, int rows = 2, int size = 40) {
        Display_(0, value, label, color, rows, size);
    }
    void Display(CSREF value, CSREF label = "", uint32_t color = GH_DEFAULT, int rows = 2, int size = 40) {
        Display_("", value.c_str(), label.c_str(), color, rows, size);
    }

    void _display(bool fstr, VSPTR name, VSPTR value, VSPTR label, uint32_t color, int rows, int size) {
        if (_nameAuto(name, fstr)) name = nullptr;
        if (_isUI()) {
            _begin(F("display"));
            _name(name, fstr);
            _value(value, fstr);
            _label(label, fstr);
            _color(color);
            _add(F(",\"rows\":"));
            *sptr += rows;
            _size(size);
            _tabw();
            _end();
        } else if (_isRead()) {
            if (_checkName(name, fstr)) GH_addEsc(sptr, value, fstr);
        }
    }

    // ========================= TABLE ==========================
    void Table_(FSTR name, FSTR text = nullptr, FSTR align = nullptr, FSTR width = nullptr, FSTR label = nullptr) {
        _table(true, name, text, align, width, label);
    }
    void Table_(CSREF name, CSREF text = "", CSREF align = "", CSREF width = "", CSREF label = "") {
        _table(false, name.c_str(), text.c_str(), align.c_str(), width.c_str(), label.c_str());
    }

    void Table() {
        Table_(0);
    }
    void Table(FSTR text = nullptr, FSTR align = nullptr, FSTR width = nullptr, FSTR label = nullptr) {
        Table_(0, text, align, width, label);
    }
    void Table(CSREF text = "", CSREF align = "", CSREF width = "", CSREF label = "") {
        Table_("", text.c_str(), align.c_str(), width.c_str(), label.c_str());
    }

    void _table(bool fstr, VSPTR name, VSPTR value, VSPTR align, VSPTR width, VSPTR label) {
        if (_nameAuto(name, fstr)) name = nullptr;
        if (_isUI()) {
            _begin(F("table"));
            _name(name, fstr);
            _value(value, fstr);
            _add(F(",\"align\":\""));
            _add(align, fstr);
            _quot();
            _add(F(",\"width\":\""));
            _add(width, fstr);
            _quot();
            _label(label, fstr);
            _tabw();
            _end();
        } else if (_isRead()) {
            if (_checkName(name, fstr)) GH_addEsc(sptr, value, fstr);
        }
    }

    // ========================== HTML ==========================
    void HTML_(FSTR name, FSTR value = nullptr, FSTR label = nullptr) {
        _html(true, name, value, label);
    }
    void HTML_(CSREF name, CSREF value = "", CSREF label = "") {
        _html(false, name.c_str(), value.c_str(), label.c_str());
    }

    void HTML() {
        HTML_(0);
    }
    void HTML(FSTR value, FSTR label = nullptr) {
        HTML_(0, value, label);
    }
    void HTML(CSREF value, CSREF label = "") {
        HTML_("", value.c_str(), label.c_str());
    }

    void _html(bool fstr, VSPTR name, VSPTR value, VSPTR label) {
        if (_nameAuto(name, fstr)) name = nullptr;
        if (_isUI()) {
            _begin(F("html"));
            _name(name, fstr);
            _value(value, fstr);
            _label(label, fstr);
            _tabw();
            _end();
        } else if (_isRead()) {
            if (_checkName(name, fstr)) GH_addEsc(sptr, value, fstr);
        }
    }

    // =========================== JS ===========================
    void JS(FSTR value) {
        _js(true, value);
    }
    void JS(CSREF value) {
        _js(false, value.c_str());
    }

    void _js(bool fstr, VSPTR value) {
        if (_isUI()) {
            _begin(F("js"));
            _value(value, fstr);
            _end();
        }
    }

    // ========================== INPUT ==========================
    bool Input_(FSTR name, void* var = nullptr, GHdata_t type = GH_NULL, FSTR label = nullptr, int maxv = 0, FSTR regex = nullptr, uint32_t color = GH_DEFAULT) {
        return _input(true, F("input"), name, var, type, label, maxv, regex, color);
    }
    bool Input_(CSREF name, void* var = nullptr, GHdata_t type = GH_NULL, CSREF label = "", int maxv = 0, CSREF regex = "", uint32_t color = GH_DEFAULT) {
        return _input(false, F("input"), name.c_str(), var, type, label.c_str(), maxv, regex.c_str(), color);
    }

    bool Input(void* var = nullptr, GHdata_t type = GH_NULL) {
        return Input_(0, var, type);
    }
    bool Input(void* var, GHdata_t type, FSTR label, int maxv = 0, FSTR regex = nullptr, uint32_t color = GH_DEFAULT) {
        return Input_(0, var, type, label, maxv, regex, color);
    }
    bool Input(void* var, GHdata_t type, CSREF label, int maxv = 0, CSREF regex = "", uint32_t color = GH_DEFAULT) {
        return Input_("", var, type, label.c_str(), maxv, regex.c_str(), color);
    }

    // ========================== PASS ==========================
    bool Pass_(FSTR name, void* var = nullptr, GHdata_t type = GH_NULL, FSTR label = nullptr, int maxv = 0, uint32_t color = GH_DEFAULT) {
        return _input(true, F("pass"), name, var, type, label, maxv, nullptr, color);
    }
    bool Pass_(CSREF name, void* var = nullptr, GHdata_t type = GH_NULL, CSREF label = "", int maxv = 0, uint32_t color = GH_DEFAULT) {
        return _input(false, F("pass"), name.c_str(), var, type, label.c_str(), maxv, "", color);
    }

    bool Pass(void* var = nullptr, GHdata_t type = GH_NULL) {
        return Pass_(0, var, type);
    }
    bool Pass(void* var, GHdata_t type, FSTR label, int maxv = 0, uint32_t color = GH_DEFAULT) {
        return Pass_(0, var, type, label, maxv, color);
    }
    bool Pass(void* var, GHdata_t type, CSREF label, int maxv = 0, uint32_t color = GH_DEFAULT) {
        return Pass_("", var, type, label.c_str(), maxv, color);
    }

    bool _input(bool fstr, FSTR tag, VSPTR name, void* var, GHdata_t type, VSPTR label, int maxv, VSPTR regex, uint32_t color) {
        if (_nameAuto(name, fstr)) name = nullptr;
        if (_isUI()) {
            _begin(tag);
            _name(name, fstr);
            _value();
            _quot();
            GHtypeToStr(sptr, var, type);
            _quot();
            _label(label, fstr);
            if (maxv) _maxv((long)maxv);
            _add(F(",\"regex\":\""));
            GH_addEsc(sptr, regex, fstr);
            _quot();
            _color(color);
            _tabw();
            _end();
        } else if (_isRead()) {
            if (_checkName(name, fstr)) GHtypeToStr(sptr, var, type);
        } else if (bptr->type == GH_BUILD_ACTION) {
            return bptr->parse(name, var, type, fstr);
        }
        return 0;
    }

    // ========================== SLIDER ==========================
    bool Slider_(FSTR name, void* var = nullptr, GHdata_t type = GH_NULL, FSTR label = nullptr, float minv = 0, float maxv = 100, float step = 1, uint32_t color = GH_DEFAULT) {
        return _spinner(true, F("slider"), name, var, type, label, minv, maxv, step, color);
    }
    bool Slider_(CSREF name, void* var = nullptr, GHdata_t type = GH_NULL, CSREF label = "", float minv = 0, float maxv = 100, float step = 1, uint32_t color = GH_DEFAULT) {
        return _spinner(false, F("slider"), name.c_str(), var, type, label.c_str(), minv, maxv, step, color);
    }

    bool Slider(void* var = nullptr, GHdata_t type = GH_NULL) {
        return Slider_(0, var, type);
    }
    bool Slider(void* var, GHdata_t type, FSTR label, float minv = 0, float maxv = 100, float step = 1, uint32_t color = GH_DEFAULT) {
        return Slider_(0, var, type, label, minv, maxv, step, color);
    }
    bool Slider(void* var, GHdata_t type, CSREF label, float minv = 0, float maxv = 100, float step = 1, uint32_t color = GH_DEFAULT) {
        return Slider_("", var, type, label.c_str(), minv, maxv, step, color);
    }

    // ========================== SPINNER ==========================
    bool Spinner_(FSTR name, void* var = nullptr, GHdata_t type = GH_NULL, FSTR label = nullptr, float minv = 0, float maxv = 100, float step = 1, uint32_t color = GH_DEFAULT) {
        return _spinner(true, F("spinner"), name, var, type, label, minv, maxv, step, color);
    }
    bool Spinner_(CSREF name, void* var = nullptr, GHdata_t type = GH_NULL, CSREF label = "", float minv = 0, float maxv = 100, float step = 1, uint32_t color = GH_DEFAULT) {
        return _spinner(false, F("spinner"), name.c_str(), var, type, label.c_str(), minv, maxv, step, color);
    }

    bool Spinner(void* var = nullptr, GHdata_t type = GH_NULL) {
        return Spinner_(0, var, type);
    }
    bool Spinner(void* var, GHdata_t type, FSTR label, float minv = 0, float maxv = 100, float step = 1, uint32_t color = GH_DEFAULT) {
        return Spinner_(0, var, type, label, minv, maxv, step, color);
    }
    bool Spinner(void* var, GHdata_t type, CSREF label, float minv = 0, float maxv = 100, float step = 1, uint32_t color = GH_DEFAULT) {
        return Spinner_("", var, type, label.c_str(), minv, maxv, step, color);
    }

    bool _spinner(bool fstr, FSTR tag, VSPTR name, void* var, GHdata_t type, VSPTR label, float minv, float maxv, float step, uint32_t color) {
        if (_nameAuto(name, fstr)) name = nullptr;
        if (_isUI()) {
            _begin(tag);
            _name(name, fstr);
            _value();
            GHtypeToStr(sptr, var, type);
            _label(label, fstr);
            _minv(minv);
            _maxv(maxv);
            _step(step);
            _color(color);
            _tabw();
            _end();
        } else if (_isRead()) {
            if (_checkName(name, fstr)) GHtypeToStr(sptr, var, type);
        } else if (bptr->type == GH_BUILD_ACTION) {
            return bptr->parse(name, var, type, fstr);
        }
        return 0;
    }

    // ========================== GAUGE ===========================
    void Gauge_(FSTR name, float value = 0, FSTR text = nullptr, FSTR label = nullptr, float minv = 0, float maxv = 100, float step = 1, uint32_t color = GH_DEFAULT) {
        _gauge(true, name, value, text, label, minv, maxv, step, color);
    }
    void Gauge_(CSREF name, float value = 0, CSREF text = "", CSREF label = "", float minv = 0, float maxv = 100, float step = 1, uint32_t color = GH_DEFAULT) {
        _gauge(false, name.c_str(), value, text.c_str(), label.c_str(), minv, maxv, step, color);
    }

    void Gauge(float value = 0) {
        Gauge_(0, value);
    }
    void Gauge(float value, FSTR text, FSTR label = nullptr, float minv = 0, float maxv = 100, float step = 1, uint32_t color = GH_DEFAULT) {
        Gauge_(0, value, text, label, minv, maxv, step, color);
    }
    void Gauge(float value, CSREF text, CSREF label = "", float minv = 0, float maxv = 100, float step = 1, uint32_t color = GH_DEFAULT) {
        Gauge_("", value, text.c_str(), label.c_str(), minv, maxv, step, color);
    }

    void _gauge(bool fstr, VSPTR name, float value, VSPTR text, VSPTR label, float minv, float maxv, float step, uint32_t color) {
        if (_nameAuto(name, fstr)) name = nullptr;
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
    bool Switch_(FSTR name, bool* var = nullptr, FSTR label = nullptr, uint32_t color = GH_DEFAULT) {
        return _switch(true, F("switch"), name, var, label, color, nullptr);
    }
    bool Switch_(CSREF name, bool* var = nullptr, CSREF label = "", uint32_t color = GH_DEFAULT) {
        return _switch(false, F("switch"), name.c_str(), var, label.c_str(), color, nullptr);
    }

    bool Switch(bool* var = nullptr) {
        return Switch_(0, var);
    }
    bool Switch(bool* var, FSTR label, uint32_t color = GH_DEFAULT) {
        return Switch_(0, var, label, color);
    }
    bool Switch(bool* var, CSREF label, uint32_t color = GH_DEFAULT) {
        return Switch_("", var, label.c_str(), color);
    }

    bool SwitchIcon_(FSTR name, bool* var = nullptr, FSTR label = nullptr, FSTR text = nullptr, uint32_t color = GH_DEFAULT) {
        return _switch(true, F("switch_i"), name, var, label, color, text);
    }
    bool SwitchIcon_(CSREF name, bool* var = nullptr, CSREF label = "", CSREF text = "", uint32_t color = GH_DEFAULT) {
        return _switch(false, F("switch_i"), name.c_str(), var, label.c_str(), color, text.c_str());
    }

    bool SwitchIcon(bool* var = nullptr) {
        return SwitchIcon_(0, var);
    }
    bool SwitchIcon(bool* var, FSTR label, FSTR text = nullptr, uint32_t color = GH_DEFAULT) {
        return SwitchIcon_(0, var, label, text, color);
    }
    bool SwitchIcon(bool* var, CSREF label, CSREF text = "", uint32_t color = GH_DEFAULT) {
        return SwitchIcon_("", var, label.c_str(), text.c_str(), color);
    }

    bool SwitchText_(FSTR name, bool* var = nullptr, FSTR label = nullptr, FSTR text = nullptr, uint32_t color = GH_DEFAULT) {
        return _switch(true, F("switch_t"), name, var, label, color, text);
    }
    bool SwitchText_(CSREF name, bool* var = nullptr, CSREF label = "", CSREF text = "", uint32_t color = GH_DEFAULT) {
        return _switch(false, F("switch_t"), name.c_str(), var, label.c_str(), color, text.c_str());
    }

    bool SwitchText(bool* var = nullptr) {
        return SwitchText_(0, var);
    }
    bool SwitchText(bool* var, FSTR label, FSTR text = nullptr, uint32_t color = GH_DEFAULT) {
        return SwitchText_(0, var, label, text, color);
    }
    bool SwitchText(bool* var, CSREF label, CSREF text = "", uint32_t color = GH_DEFAULT) {
        return SwitchText_("", var, label.c_str(), text.c_str(), color);
    }

    bool _switch(bool fstr, FSTR tag, VSPTR name, bool* var, VSPTR label, uint32_t color, VSPTR text) {
        if (_nameAuto(name, fstr)) name = nullptr;
        if (_isUI()) {
            _begin(tag);
            _name(name, fstr);
            _value();
            GHtypeToStr(sptr, var, GH_BOOL);
            _label(label, fstr);
            _color(color);
            _text(text, fstr);
            _tabw();
            _end();
        } else if (_isRead()) {
            if (_checkName(name, fstr)) GHtypeToStr(sptr, var, GH_BOOL);
        } else if (bptr->type == GH_BUILD_ACTION) {
            return bptr->parse(name, var, GH_BOOL, fstr);
        }
        return 0;
    }

    // ========================== DATETIME ==========================
    bool Date_(FSTR name, void* var = nullptr, FSTR label = nullptr, uint32_t color = GH_DEFAULT) {
        return _date(true, F("date"), name, var, label, color);
    }
    bool Date_(CSREF name, void* var = nullptr, CSREF label = "", uint32_t color = GH_DEFAULT) {
        return _date(false, F("date"), name.c_str(), var, label.c_str(), color);
    }

    bool Date(void* var = nullptr) {
        return Date_(0, var);
    }
    bool Date(void* var, FSTR label, uint32_t color = GH_DEFAULT) {
        return Date_(0, var, label, color);
    }
    bool Date(void* var, CSREF label, uint32_t color = GH_DEFAULT) {
        return Date_("", var, label.c_str(), color);
    }

    bool Time_(FSTR name, void* var = nullptr, FSTR label = nullptr, uint32_t color = GH_DEFAULT) {
        return _date(true, F("time"), name, var, label, color);
    }
    bool Time_(CSREF name, void* var = nullptr, CSREF label = "", uint32_t color = GH_DEFAULT) {
        return _date(false, F("time"), name.c_str(), var, label.c_str(), color);
    }

    bool Time(void* var = nullptr) {
        return Time_(0, var);
    }
    bool Time(void* var, FSTR label, uint32_t color = GH_DEFAULT) {
        return Time_(0, var, label, color);
    }
    bool Time(void* var, CSREF label, uint32_t color = GH_DEFAULT) {
        return Time_("", var, label.c_str(), color);
    }

    bool DateTime_(FSTR name, void* var = nullptr, FSTR label = nullptr, uint32_t color = GH_DEFAULT) {
        return _date(true, F("datetime"), name, var, label, color);
    }
    bool DateTime_(CSREF name, void* var = nullptr, CSREF label = "", uint32_t color = GH_DEFAULT) {
        return _date(false, F("datetime"), name.c_str(), var, label.c_str(), color);
    }

    bool DateTime(void* var = nullptr) {
        return DateTime_(0, var);
    }
    bool DateTime(void* var, FSTR label, uint32_t color = GH_DEFAULT) {
        return DateTime_(0, var, label, color);
    }
    bool DateTime(void* var, CSREF label, uint32_t color = GH_DEFAULT) {
        return DateTime_("", var, label.c_str(), color);
    }

    bool _date(bool fstr, FSTR tag, VSPTR name, void* var, VSPTR label, uint32_t color) {
        if (_nameAuto(name, fstr)) name = nullptr;
        if (_isUI()) {
            _begin(tag);
            _name(name, fstr);
            _label(label, fstr);
            _value();
            GHtypeToStr(sptr, var, GH_UINT32);
            _color(color);
            _tabw();
            _end();
        } else if (_isRead()) {
            if (_checkName(name, fstr)) GHtypeToStr(sptr, var, GH_UINT32);
        } else if (bptr->type == GH_BUILD_ACTION) {
            return bptr->parse(name, var, GH_UINT32, fstr);
        }
        return 0;
    }

    // ========================== SELECT ==========================
    bool Select_(FSTR name, uint8_t* var, FSTR text, FSTR label = nullptr, uint32_t color = GH_DEFAULT) {
        return _select(true, name, var, text, label, color);
    }
    bool Select_(CSREF name, uint8_t* var, CSREF text, CSREF label = "", uint32_t color = GH_DEFAULT) {
        return _select(false, name.c_str(), var, text.c_str(), label.c_str(), color);
    }

    bool Select(uint8_t* var, FSTR text, FSTR label = nullptr, uint32_t color = GH_DEFAULT) {
        return Select_(0, var, text, label, color);
    }
    bool Select(uint8_t* var, CSREF text, CSREF label = "", uint32_t color = GH_DEFAULT) {
        return Select_("", var, text.c_str(), label.c_str(), color);
    }

    bool _select(bool fstr, VSPTR name, uint8_t* var, VSPTR text, VSPTR label, uint32_t color) {
        if (_nameAuto(name, fstr)) name = nullptr;
        if (_isUI()) {
            _begin(F("select"));
            _name(name, fstr);
            _value();
            GHtypeToStr(sptr, var, GH_UINT8);
            _text(text, fstr);
            _label(label, fstr);
            _color(color);
            _tabw();
            _end();
        } else if (_isRead()) {
            if (_checkName(name, fstr)) GHtypeToStr(sptr, var, GH_UINT8);
        } else if (bptr->type == GH_BUILD_ACTION) {
            return bptr->parse(name, var, GH_UINT8, fstr);
        }
        return 0;
    }

    // ========================== FLAGS ==========================
    bool Flags_(FSTR name, GHflags* var = nullptr, FSTR text = nullptr, FSTR label = nullptr, uint32_t color = GH_DEFAULT) {
        return _flags(true, name, var, text, label, color);
    }
    bool Flags_(CSREF name, GHflags* var = nullptr, CSREF text = "", CSREF label = "", uint32_t color = GH_DEFAULT) {
        return _flags(false, name.c_str(), var, text.c_str(), label.c_str(), color);
    }

    bool Flags(GHflags* var = nullptr) {
        return Flags_(0, var);
    }
    bool Flags(GHflags* var, FSTR text, FSTR label = nullptr, uint32_t color = GH_DEFAULT) {
        return Flags_(0, var, text, label, color);
    }
    bool Flags(GHflags* var, CSREF text, CSREF label = "", uint32_t color = GH_DEFAULT) {
        return Flags_("", var, text.c_str(), label.c_str(), color);
    }

    bool _flags(bool fstr, VSPTR name, GHflags* var, VSPTR text, VSPTR label, uint32_t color) {
        if (_nameAuto(name, fstr)) name = nullptr;
        if (_isUI()) {
            _begin(F("flags"));
            _name(name, fstr);
            _value();
            GHtypeToStr(sptr, var, GH_FLAGS);
            _text(text, fstr);
            _label(label, fstr);
            _color(color);
            _tabw();
            _end();
        } else if (_isRead()) {
            if (_checkName(name, fstr)) GHtypeToStr(sptr, var, GH_FLAGS);
        } else if (bptr->type == GH_BUILD_ACTION) {
            return bptr->parse(name, var, GH_FLAGS, fstr);
        }
        return 0;
    }

    // ========================== COLOR ==========================
    bool Color_(FSTR name, GHcolor* var = nullptr, FSTR label = nullptr) {
        return _color(true, name, var, label);
    }
    bool Color_(CSREF name, GHcolor* var = nullptr, CSREF label = "") {
        return _color(false, name.c_str(), var, label.c_str());
    }

    bool Color(GHcolor* var = nullptr) {
        return Color_(0, var);
    }
    bool Color(GHcolor* var, FSTR label) {
        return Color_(0, var, label);
    }
    bool Color(GHcolor* var, CSREF label) {
        return Color_("", var, label.c_str());
    }

    bool _color(bool fstr, VSPTR name, GHcolor* var, VSPTR label) {
        if (_nameAuto(name, fstr)) name = nullptr;
        if (_isUI()) {
            _begin(F("color"));
            _name(name, fstr);
            _value();
            GHtypeToStr(sptr, var, GH_COLOR);
            _label(label, fstr);
            _tabw();
            _end();
        } else if (_isRead()) {
            if (_checkName(name, fstr)) GHtypeToStr(sptr, var, GH_COLOR);
        } else if (bptr->type == GH_BUILD_ACTION) {
            return bptr->parse(name, var, GH_COLOR, fstr);
        }
        return 0;
    }

    // ========================== LED ==========================
    void LED_(FSTR name, bool value = 0, FSTR label = nullptr, FSTR icon = nullptr) {
        _led(true, name, value, label, icon);
    }
    void LED_(CSREF name, bool value = 0, CSREF label = "", CSREF icon = "") {
        _led(false, name.c_str(), value, label.c_str(), icon.c_str());
    }

    void LED(bool value = 0) {
        LED_(0, value);
    }
    void LED(bool value, FSTR label, FSTR icon = nullptr) {
        LED_(0, value, label, icon);
    }
    void LED(bool value, CSREF label, CSREF icon = "") {
        LED_("", value, label.c_str(), icon.c_str());
    }

    void _led(bool fstr, VSPTR name, bool value, VSPTR label, VSPTR text) {
        if (_nameAuto(name, fstr)) name = nullptr;
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
            _add(F(",\"height\":"));
            *sptr += height;
            _tabw();
            _end();
        }
    }

    // ========================== MENU ==========================
    bool Menu(FSTR text) {
        return _tabs(true, F("menu"), F("_menu"), &menu, text, nullptr);
    }
    bool Menu(CSREF text) {
        return _tabs(false, F("menu"), "_menu", &menu, text.c_str(), nullptr);
    }

    // ========================== TABS ==========================
    bool Tabs_(FSTR name, uint8_t* var, FSTR text, FSTR label = nullptr) {
        return _tabs(true, F("tabs"), name, var, text, label);
    }
    bool Tabs_(CSREF name, uint8_t* var, CSREF text, CSREF label = "") {
        return _tabs(false, F("tabs"), name.c_str(), var, text.c_str(), label.c_str());
    }

    bool Tabs(uint8_t* var, FSTR text, FSTR label = nullptr) {
        return Tabs_(0, var, text, label);
    }
    bool Tabs(uint8_t* var, CSREF text, CSREF label = "") {
        return Tabs_("", var, text.c_str(), label.c_str());
    }

    bool _tabs(bool fstr, FSTR tag, VSPTR name, uint8_t* var, VSPTR text, VSPTR label) {
        if (_nameAuto(name, fstr)) name = nullptr;
        if (_isUI()) {
            _begin(tag);
            _name(name, fstr);
            _value();
            *sptr += *var;
            _text(text, fstr);
            _label(label, fstr);
            _tabw();
            _end();
        } else if (bptr->type == GH_BUILD_ACTION) {
            bool act = bptr->parse(name, var, GH_UINT8, fstr);
            if (act) refresh();
            return act;
        }
        return 0;
    }

    // ========================= CANVAS =========================
    bool Canvas_(FSTR name, int width = 400, int height = 300, GHcanvas* cv = nullptr, GHpos* pos = nullptr, FSTR label = nullptr) {
        return _canvas(true, name, width, height, cv, pos, label, false);
    }
    bool Canvas_(CSREF name, int width = 400, int height = 300, GHcanvas* cv = nullptr, GHpos* pos = nullptr, CSREF label = "") {
        return _canvas(false, name.c_str(), width, height, cv, pos, label.c_str(), false);
    }

    bool Canvas(int width = 400, int height = 300, GHcanvas* cv = nullptr, GHpos* pos = nullptr) {
        return Canvas_(0, width, height, cv, pos);
    }
    bool Canvas(int width, int height, GHcanvas* cv, GHpos* pos, FSTR label) {
        return Canvas_(0, width, height, cv, pos, label);
    }
    bool Canvas(int width, int height, GHcanvas* cv, GHpos* pos, CSREF label) {
        return Canvas_("", width, height, cv, pos, label.c_str());
    }

    bool BeginCanvas_(FSTR name, int width = 400, int height = 300, GHcanvas* cv = nullptr, GHpos* pos = nullptr, FSTR label = nullptr) {
        return _canvas(true, name, width, height, cv, pos, label, true);
    }
    bool BeginCanvas_(CSREF name, int width = 400, int height = 300, GHcanvas* cv = nullptr, GHpos* pos = nullptr, CSREF label = "") {
        return _canvas(false, name.c_str(), width, height, cv, pos, label.c_str(), true);
    }

    bool BeginCanvas(int width = 400, int height = 300, GHcanvas* cv = nullptr, GHpos* pos = nullptr) {
        return BeginCanvas_(0, width, height, cv, pos);
    }
    bool BeginCanvas(int width, int height, GHcanvas* cv, GHpos* pos, FSTR label) {
        return BeginCanvas_(0, width, height, cv, pos, label);
    }
    bool BeginCanvas(int width, int height, GHcanvas* cv, GHpos* pos, CSREF label) {
        return BeginCanvas_("", width, height, cv, pos, label.c_str());
    }

    bool _canvas(bool fstr, VSPTR name, int width, int height, GHcanvas* cv, GHpos* pos, VSPTR label, bool begin) {
        if (_nameAuto(name, fstr)) name = nullptr;
        if (!_isUI() && cv) cv->extBuffer(nullptr);

        if (_isUI()) {
            _begin(F("canvas"));
            _name(name, fstr);
            _add(F(",\"width\":"));
            *sptr += width;
            _add(F(",\"height\":"));
            *sptr += height;
            _label(label, fstr);
            if (pos) _add(F(",\"active\":1"));
            _value();
            *sptr += '[';
            if (begin && cv) cv->extBuffer(sptr);
            else EndCanvas();
        } else if (bptr->type == GH_BUILD_ACTION) {
            return bptr->parse(name, pos, GH_POS, fstr);
        }
        return 0;
    }

    void EndCanvas() {
        if (_isUI()) {
            *sptr += ']';
            _tabw();
            _end();
        }
    }

    // ========================= IMAGE =========================
    void Image_(FSTR name, FSTR path, FSTR label = nullptr) {
        _image(true, name, path, label);
    }
    void Image_(CSREF name, CSREF path, CSREF label = "") {
        _image(false, name.c_str(), path.c_str(), label.c_str());
    }

    void Image(FSTR path, FSTR label = nullptr) {
        Image_(0, path, label);
    }
    void Image(CSREF path, CSREF label = "") {
        Image_("", path.c_str(), label.c_str());
    }

    void _image(bool fstr, VSPTR name, VSPTR path, VSPTR label) {
        if (_nameAuto(name, fstr)) name = nullptr;
        if (_isUI()) {
            _begin(F("image"));
            _name(name, fstr);
            _value(path, fstr);
            _label(label, fstr);
            _tabw();
            _end();
        }
    }

    // ========================= STREAM =========================
    void Stream(uint16_t port = 82) {
        if (_isUI()) {
            _begin(F("stream"));
            _add(F(",\"port\":"));
            *sptr += port;
            _tabw();
            _end();
        }
    }

    // =========================== JOY ===========================
    bool Joystick_(FSTR name, GHpos* pos = nullptr, bool autoc = 1, bool exp = 0, FSTR label = nullptr, uint32_t color = GH_DEFAULT) {
        return _joy(F("joy"), true, name, pos, autoc, exp, label, color);
    }
    bool Joystick_(CSREF name, GHpos* pos = nullptr, bool autoc = 1, bool exp = 0, CSREF label = "", uint32_t color = GH_DEFAULT) {
        return _joy(F("joy"), false, name.c_str(), pos, autoc, exp, label.c_str(), color);
    }

    bool Joystick(GHpos* pos = nullptr, bool autoc = 1, bool exp = 0) {
        return Joystick_(0, pos, autoc, exp);
    }
    bool Joystick(GHpos* pos, bool autoc, bool exp, FSTR label, uint32_t color = GH_DEFAULT) {
        return Joystick_(0, pos, autoc, exp, label, color);
    }
    bool Joystick(GHpos* pos, bool autoc, bool exp, CSREF label, uint32_t color = GH_DEFAULT) {
        return Joystick_("", pos, autoc, exp, label.c_str(), color);
    }

    // =========================== DPAD ===========================
    bool Dpad_(FSTR name, GHpos* pos = nullptr, FSTR label = nullptr, uint32_t color = GH_DEFAULT) {
        return _joy(F("dpad"), true, name, pos, 0, 0, label, color);
    }
    bool Dpad_(CSREF name, GHpos* pos = nullptr, CSREF label = "", uint32_t color = GH_DEFAULT) {
        return _joy(F("dpad"), false, name.c_str(), pos, 0, 0, label.c_str(), color);
    }

    bool Dpad(GHpos* pos = nullptr) {
        return Dpad_(0, pos);
    }
    bool Dpad(GHpos* pos, FSTR label, uint32_t color = GH_DEFAULT) {
        return Dpad_(0, pos, label, color);
    }
    bool Dpad(GHpos* pos, CSREF label, uint32_t color = GH_DEFAULT) {
        return Dpad_("", pos, label.c_str(), color);
    }

    bool _joy(FSTR tag, bool fstr, VSPTR name, GHpos* pos, bool autoc, bool exp, VSPTR label, uint32_t color) {
        if (_nameAuto(name, fstr)) name = nullptr;
        if (_isUI()) {
            _begin(tag);
            _name(name, fstr);
            if (autoc) {
                _add(F(",\"auto\":"));
                *sptr += autoc;
            }
            if (exp) {
                _add(F(",\"exp\":"));
                *sptr += exp;
            }
            _label(label, fstr);
            _color(color);
            _tabw();
            _end();
        } else if (bptr->type == GH_BUILD_ACTION) {
            bool act = bptr->parse(name, pos, GH_POS, fstr);
            if (act && pos) {
                pos->x -= 255;
                pos->y -= 255;
            }
            return act;
        }
        return 0;
    }

    // ======================= CONFIRM ========================
    bool Confirm_(FSTR name, bool* var = nullptr, FSTR label = nullptr) {
        return _confirm(true, name, var, label);
    }
    bool Confirm_(CSREF name, bool* var = nullptr, CSREF label = "") {
        return _confirm(false, name.c_str(), var, label.c_str());
    }

    bool Confirm(bool* var = nullptr) {
        return Confirm_(0, var);
    }
    bool Confirm(bool* var, FSTR label) {
        return Confirm_(0, var, label);
    }
    bool Confirm(bool* var, CSREF label) {
        return Confirm_("", var, label.c_str());
    }

    bool _confirm(bool fstr, VSPTR name, bool* var, VSPTR label) {
        if (_nameAuto(name, fstr)) name = nullptr;
        if (_isUI()) {
            _begin(F("confirm"));
            _name(name, fstr);
            _label(label, fstr);
            _end();
        } else if (bptr->type == GH_BUILD_ACTION) {
            return bptr->parse(name, var, GH_BOOL, fstr);
        }
        return 0;
    }

    // ========================= PROMPT ========================
    bool Prompt_(FSTR name, void* value = nullptr, GHdata_t type = GH_NULL, FSTR label = nullptr) {
        return _prompt(true, name, value, type, label);
    }
    bool Prompt_(CSREF name, void* value = nullptr, GHdata_t type = GH_NULL, CSREF label = "") {
        return _prompt(false, name.c_str(), value, type, label.c_str());
    }

    bool Prompt(void* value = nullptr, GHdata_t type = GH_NULL) {
        return Prompt_(0, value, type);
    }
    bool Prompt(void* value, GHdata_t type, FSTR label) {
        return Prompt_(0, value, type, label);
    }
    bool Prompt(void* value, GHdata_t type, CSREF label) {
        return Prompt_("", value, type, label.c_str());
    }

    bool _prompt(bool fstr, VSPTR name, void* value, GHdata_t type, VSPTR label) {
        if (_nameAuto(name, fstr)) name = nullptr;
        if (_isUI()) {
            _begin(F("prompt"));
            _name(name, fstr);
            _value();
            _quot();
            GHtypeToStr(sptr, value, type);
            _quot();
            _label(label, fstr);
            _end();
        } else if (bptr->type == GH_BUILD_ACTION) {
            return bptr->parse(name, value, type, fstr);
        }
        return 0;
    }

    uint8_t menu = 0;

    // ======================== PROTECTED ========================
   protected:
    String* sptr = nullptr;
    GHbuild* bptr = nullptr;
    virtual void _afterComponent() = 0;
    virtual void refresh() = 0;
    uint16_t tab_width = 0;

    // ========================= PRIVATE =========================
   private:
    bool _nameAuto(VSPTR name, bool fstr) {
        bool f;
        if (!name) f = 1;
        else if (fstr) f = !pgm_read_byte((uint8_t*)name);
        else f = !(*(uint8_t*)name);
        if (f) bptr->action.count++;
        return f;
    }
    bool _checkName(VSPTR name, bool fstr) {
        if (name) {
            if (fstr ? (!strcmp_P(bptr->action.name, (PGM_P)name)) : (!strcmp(bptr->action.name, (PGM_P)name))) {
                bptr->type = GH_BUILD_NONE;
                return true;
            }
        } else {
            if (bptr->action.autoNameEq()) {
                bptr->type = GH_BUILD_NONE;
                return true;
            }
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
    void _add(VSPTR str, bool fstr = true) {
        if (str) {
            if (fstr) *sptr += (FSTR)str;
            else *sptr += (PGM_P)str;
        }
    }
    void _begin(FSTR type) {
        _add(F("{\"type\":\""));
        *sptr += type;
        _quot();
    }
    void _end() {
        *sptr += '}';
        _afterComponent();
        *sptr += ',';
    }
    void _quot() {
        *sptr += '\"';
    }
    void _tabw() {
        if (tab_width) {
            _add(F(",\"tab_w\":"));
            *sptr += tab_width;
        }
    }

    // ================
    void _value() {
        *sptr += F(",\"value\":");
    }
    void _value(VSPTR value, bool fstr) {
        _value();
        _quot();
        GH_addEsc(sptr, value, fstr);  //_add(value, fstr);
        _quot();
    }
    void _name(VSPTR name, bool fstr) {
        _add(F(",\"name\":\""));
        if (name) _add(name, fstr);
        else {
            *sptr += F("_n");
            *sptr += bptr->action.count;
        }
        _quot();
    }
    void _label(VSPTR label, bool fstr) {
        _add(F(",\"label\":\""));
        GH_addEsc(sptr, label, fstr);  //_add(label, fstr);
        _quot();
    }
    void _text() {
        _add(F(",\"text\":"));
    }
    void _text(VSPTR text, bool fstr) {
        _text();
        _quot();
        GH_addEsc(sptr, text, fstr);  //_add(text, fstr);
        _quot();
    }

    // ================
    void _color(uint32_t& col) {
        if (col == GH_DEFAULT) return;
        _add(F(",\"color\":"));
        *sptr += col;
    }
    void _size(int& val) {
        _add(F(",\"size\":"));
        *sptr += val;
    }

    // ================
    void _minv(float val) {
        _add(F(",\"min\":"));
        *sptr += val;
    }

    void _maxv(float val) {
        _add(F(",\"max\":"));
        *sptr += val;
    }

    void _step(float val) {
        _add(F(",\"step\":"));
        if (val < 0.01) *sptr += String(val, 4);
        else *sptr += val;
    }
};