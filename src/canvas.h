#pragma once

// HTML Canvas API
// https://www.w3schools.com/tags/ref_canvas.asp
// https://processing.org/reference/

#include <Arduino.h>

#include "macro.hpp"
#include "utils/misc.h"

enum GHmode_t {
    CV_BUTT,
    CV_ROUND,
    CV_SQUARE,
    CV_PROJECT,
    CV_BEVEL,
    CV_MITER,
    CV_START,
    CV_END,
    CV_CENTER,
    CV_LEFT,
    CV_RIGHT,
    CV_ALPHABETIC,
    CV_TOP,
    CV_HANGING,
    CV_MIDDLE,
    CV_IDEOGRAPHIC,
    CV_BOTTOM,
    CV_SRC_OVER,
    CV_SRC_ATOP,
    CV_SRC_IN,
    CV_SRC_OUT,
    CV_DST_OVER,
    CV_DST_ATOP,
    CV_DST_IN,
    CV_DST_OUT,
    CV_LIGHTER,
    CV_COPY,
    CV_XOR,
    TXT_TOP,
    TXT_BOTTOM,
    TXT_CENTER,
    TXT_BASELINE,
    CV_CORNER,
    CV_CORNERS,
    CV_RADIUS,
};

class GHcanvas {
   public:
    GHcanvas() {
        ps = &buf;
    }

    // буфер
    String buf;

    // подключить внешний буфер
    void extBuffer(String* sptr) {
        ps = sptr;
    }

    // очистить буфер (внутренний)
    void clearBuffer() {
        first = 1;
        buf = "";
    }

    // добавить строку кода на js
    void custom(const String& s) {
        if (!ps) return;
        _checkFirst();
        _dquot();
        _add(s.c_str());
        _dquot();
    }
    void custom(FSTR s) {
        if (!ps) return;
        _checkFirst();
        _dquot();
        _add(s);
        _dquot();
    }

    // =====================================================
    // =============== PROCESSING-LIKE API =================
    // =====================================================

    // =================== BACKGROUND ======================
    // очистить полотно
    void clear() {
        clearRect(0, 0, -1, -1);
        beginPath();
    }

    // залить полотно установленным в fill() цветом
    void background() {
        fillRect(0, 0, -1, -1);
    }

    // залить полотно указанным цветом (цвет, прозрачность)
    void background(uint32_t hex, uint8_t a = 255) {
        fillStyle(hex, a);
        background();
    }

    // ======================== FILL =======================
    // выбрать цвет заливки (цвет, прозрачность)
    void fill(uint32_t hex, uint8_t a = 255) {
        fillStyle(hex, a);
        fillF = 1;
    }

    // отключить заливку
    void noFill() {
        fillF = 0;
    }

    // ===================== STROKE ====================
    // выбрать цвет обводки (цвет, прозрачность)
    void stroke(uint32_t hex, uint8_t a = 255) {
        strokeStyle(hex, a);
        strokeF = 1;
    }

    // отключить обводку
    void noStroke() {
        strokeF = 0;
    }

    // толщина обводки, px
    void strokeWeight(int v) {
        lineWidth(v);
    }

    // соединение линий: CV_MITER (умолч), CV_BEVEL, CV_ROUND
    void strokeJoin(GHmode_t v) {
        lineJoin(v);
    }

    // края линий: CV_PROJECT (умолч), CV_ROUND, CV_SQUARE
    void strokeCap(GHmode_t v) {
        lineCap(v);
    }

    // ===================== PRIMITIVES ====================
    // окружность (x, y, радиус), px
    void circle(int x, int y, int r) {
        beginPath();
        switch (eMode) {
            case CV_CORNER:
                arc(x + r, y + r, r);
                break;
            default:
                arc(x, y, r);
                break;
        }
        if (strokeF) stroke();
        if (fillF) fill();
    }

    // линия (координаты начала и конца)
    void line(int x1, int y1, int x2, int y2) {
        beginPath();
        moveTo(x1, y1);
        lineTo(x2, y2);
        stroke();
    }

    // точка
    void point(int x, int y) {
        beginPath();
        fillRect(x, y, 1, 1);
    }

    // четырёхугольник (координаты углов)
    void quadrangle(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4) {
        beginPath();
        moveTo(x1, y1);
        lineTo(x2, y2);
        lineTo(x3, y3);
        lineTo(x4, y4);
        closePath();
        if (strokeF) stroke();
        if (fillF) fill();
    }

    // треугольник (координаты углов)
    void triangle(int x1, int y1, int x2, int y2, int x3, int y3) {
        beginPath();
        moveTo(x1, y1);
        lineTo(x2, y2);
        lineTo(x3, y3);
        closePath();
        if (strokeF) stroke();
        if (fillF) fill();
    }

    // прямоугольник
    void rect(int x, int y, int w, int h, int tl = -1, int tr = -1, int br = 0, int bl = 0) {
        beginPath();
        int X = x, Y = y, W = w, H = h;
        switch (rMode) {
            case CV_CORNER:
                break;
            case CV_CORNERS:
                W = w - x;
                H = h - y;
                break;
            case CV_CENTER:
                X = x - w / 2;
                Y = y - h / 2;
                break;
            case CV_RADIUS:
                X = x - w;
                Y = y - h;
                W = w * 2;
                H = h * 2;
                break;
            default:
                break;
        }
        if (tl < 0) drawRect(X, Y, W, H);
        else if (tr < 0) roundRect(X, Y, W, H, tl);
        else roundRect(X, Y, W, H, tl, tr, br, bl);
        if (strokeF) stroke();
        if (fillF) fill();
    }

    // квадрат
    void square(int x, int y, int w) {
        rect(x, y, w, w);
    }

    // режим окружности: CV_CENTER (умолч), CV_CORNER
    void ellipseMode(GHmode_t mode) {
        eMode = mode;
    }

    // режим прямоугольника: CV_CORNER (умолч), CV_CORNERS, CV_CENTER, CV_RADIUS
    void rectMode(GHmode_t mode) {
        rMode = mode;
    }

    // ======================= TEXT ========================
    // шрифт
    void textFont(const char* name) {
        fname = name;
        _font();
    }

    // размер шрифта, px
    void textSize(int size) {
        fsize = size;
        _font();
    }

    // вывести текст, опционально макс длина
    void text(const String& text, int x, int y, int w = 0) {
        if (strokeF) strokeText(text, x, y, w);
        if (fillF) fillText(text, x, y, w);
    }

    // выравнивание текста
    // CV_LEFT, CV_CENTER, CV_RIGHT
    // TXT_TOP, TXT_BOTTOM, TXT_CENTER, TXT_BASELINE
    void textAlign(GHmode_t h, GHmode_t v) {
        textAlign(h);
        textBaseline(v);
    }

    // сохранить конфигурацию полотна
    void push() {
        save();
    }

    // восстановить конфигурацию полотна
    void pop() {
        restore();
    }

    // ======================================================
    // ================== HTML CANVAS API ===================
    // ======================================================

    // цвет заполнения
    void fillStyle(uint32_t hex, uint8_t a = 255) {
        addCmd(0);
        _div();
        _color(hex, a);
        _dquot();
    }

    // цвет обводки
    void strokeStyle(uint32_t hex, uint8_t a = 255) {
        addCmd(1);
        _div();
        _color(hex, a);
        _dquot();
    }

    // цвет тени
    void shadowColor(uint32_t hex, uint8_t a = 255) {
        addCmd(2);
        _div();
        _color(hex, a);
        _dquot();
    }

    // размытость тени, px
    void shadowBlur(int v) {
        addCmd(3);
        _div();
        _add(v);
        _dquot();
    }

    // отступ тени, px
    void shadowOffsetX(int v) {
        addCmd(4);
        _div();
        _add(v);
        _dquot();
    }

    // отступ тени, px
    void shadowOffsetY(int v) {
        addCmd(5);
        _div();
        _add(v);
        _dquot();
    }

    // края линий: CV_BUTT (умолч), CV_ROUND, CV_SQUARE
    // https://www.w3schools.com/tags/canvas_linecap.asp
    void lineCap(GHmode_t v) {
        addCmd(11);
        _div();
        _add(v);
        _dquot();
    }

    // соединение линий: CV_MITER (умолч), CV_BEVEL, CV_ROUND
    // https://www.w3schools.com/tags/canvas_linejoin.asp
    void lineJoin(GHmode_t v) {
        addCmd(12);
        _div();
        _add(v);
        _dquot();
    }

    // ширина линий, px
    void lineWidth(int v) {
        addCmd(6);
        _div();
        _add(v);
        _dquot();
    }

    // длина соединения CV_MITER, px
    // https://www.w3schools.com/tags/canvas_miterlimit.asp
    void miterLimit(int v) {
        addCmd(7);
        _div();
        _add(v);
        _dquot();
    }

    // шрифт: "30px Arial"
    // https://www.w3schools.com/tags/canvas_font.asp
    void font(const String& v) {
        addCmd(8);
        _div();
        _dquot();
        _add(v);
        _dquot();
        _dquot();
    }

    // выравнивание текста: CV_START (умолч), CV_END, CV_CENTER, CV_LEFT, CV_RIGHT
    // https://www.w3schools.com/tags/canvas_textalign.asp
    void textAlign(GHmode_t v) {
        addCmd(9);
        _div();
        _add(v);
        _dquot();
    }

    // позиция текста: CV_ALPHABETIC (умолч), CV_TOP, CV_HANGING, CV_MIDDLE, CV_IDEOGRAPHIC, CV_BOTTOM
    // https://www.w3schools.com/tags/canvas_textbaseline.asp
    void textBaseline(GHmode_t v) {
        addCmd(10);
        _div();
        _add(v);
        _dquot();
    }

    // прозрачность рисовки, 0.0-1.0
    void globalAlpha(float v) {
        addCmd(14);
        _div();
        _add(v);
        _dquot();
    }

    // тип наложения графики: CV_SRC_OVER (умолч), CV_SRC_ATOP, CV_SRC_IN, CV_SRC_OUT, CV_DST_OVER, CV_DST_ATOP, CV_DST_IN, CV_DST_OUT, CV_LIGHTER, CV_COPY, CV_XOR
    // https://www.w3schools.com/tags/canvas_globalcompositeoperation.asp
    void globalCompositeOperation(GHmode_t v) {
        addCmd(13);
        _div();
        _add(v);
        _dquot();
    }

    // прямоугольник
    void drawRect(int x, int y, int w, int h) {
        addCmd(17);
        _div();
        _params(4, x, y, w, h);
        _dquot();
    }

    // скруглённый прямоугольник
    void roundRect(int x, int y, int w, int h, int tl = 0, int tr = -1, int br = -1, int bl = -1) {
        addCmd(31);
        _div();
        if (tr < 0) _params(5, x, y, w, h, tl);
        else if (br < 0) _params(6, x, y, w, h, tl, tr);
        else _params(8, x, y, w, h, tl, tr, br, bl);
        _dquot();
    }

    // закрашенный прямоугольник
    void fillRect(int x, int y, int w, int h) {
        addCmd(18);
        _div();
        _params(4, x, y, w, h);
        _dquot();
    }

    // обведённый прямоугольник
    void strokeRect(int x, int y, int w, int h) {
        addCmd(19);
        _div();
        _params(4, x, y, w, h);
        _dquot();
    }

    // очистить область
    void clearRect(int x, int y, int w, int h) {
        addCmd(20);
        _div();
        _params(4, x, y, w, h);
        _dquot();
    }

    // залить
    void fill() {
        addCmd(32);
        _dquot();
    }

    // обвести
    void stroke() {
        addCmd(33);
        _dquot();
    }

    // начать путь
    void beginPath() {
        addCmd(34);
        _dquot();
    }

    // переместить курсор
    void moveTo(int x, int y) {
        addCmd(21);
        _div();
        _params(2, x, y);
        _dquot();
    }

    // завершить путь (провести линию на начало)
    void closePath() {
        addCmd(35);
        _dquot();
    }

    // нарисовать линию от курсора
    void lineTo(int x, int y) {
        addCmd(22);
        _div();
        _params(2, x, y);
        _dquot();
    }

    // ограничить область рисования
    // https://www.w3schools.com/tags/canvas_clip.asp
    void clip() {
        addCmd(36);
        _dquot();
    }

    // провести кривую
    // https://www.w3schools.com/tags/canvas_quadraticcurveto.asp
    void quadraticCurveTo(int cpx, int cpy, int x, int y) {
        addCmd(23);
        _div();
        _params(4, cpx, cpy, x, y);
        _dquot();
    }

    // провести кривую Безье
    // https://www.w3schools.com/tags/canvas_beziercurveto.asp
    void bezierCurveTo(int cp1x, int cp1y, int cp2x, int cp2y, int x, int y) {
        addCmd(24);
        _div();
        _params(6, cp1x, cp1y, cp2x, cp2y, x, y);
        _dquot();
    }

    // провести дугу (радианы)
    // https://www.w3schools.com/tags/canvas_arc.asp
    void arc(int x, int y, int r, float sa = 0, float ea = TWO_PI, bool ccw = 0) {
        addCmd(27);
        _div();
        _params(3, x, y, r);
        _comma();
        _add(sa);
        _comma();
        _add(ea);
        _add(ccw);
        _dquot();
    }

    // скруглить
    // https://www.w3schools.com/tags/canvas_arcto.asp
    void arcTo(int x1, int y1, int x2, int y2, int r) {
        addCmd(26);
        _div();
        _params(5, x1, y1, x2, y2, r);
        _dquot();
    }

    // масштабировать область рисования
    // https://www.w3schools.com/tags/canvas_scale.asp
    void scale(int sw, int sh) {
        addCmd(15);
        _div();
        _params(2, sw, sh);
        _dquot();
    }

    // вращать область рисования (в радианах)
    // https://www.w3schools.com/tags/canvas_rotate.asp
    void rotate(float v) {
        addCmd(16);
        _div();
        _add(v);
        _dquot();
    }

    // перемещать область рисования
    // https://www.w3schools.com/tags/canvas_translate.asp
    void translate(int x, int y) {
        addCmd(25);
        _div();
        _params(2, x, y);
        _dquot();
    }

    // вывести закрашенный текст, опционально макс. длина
    void fillText(const String& text, int x, int y, int w = 0) {
        addCmd(28);
        _div();
        _dquot();
        //_add(text);
        if (ps) GH_addEsc(ps, text.c_str());
        _dquot();
        _comma();
        _params(3, x, y, w);
        _dquot();
    }

    // вывести обведённый текст, опционально макс. длина
    void strokeText(const String& text, int x, int y, int w = 0) {
        addCmd(29);
        _div();
        _dquot();
        //_add(text);
        if (ps) GH_addEsc(ps, text.c_str());
        _dquot();
        _comma();
        _params(3, x, y, w);
        _dquot();
    }

    // вывести картинку
    // https://www.w3schools.com/tags/canvas_drawimage.asp
    void drawImage(const String& img, int x, int y) {
        addCmd(30);
        _div();
        _add(img);
        _comma();
        _params(2, x, y);
        _dquot();
    }
    void drawImage(const String& img, int x, int y, int w, int h) {
        addCmd(30);
        _div();
        _add(img);
        _comma();
        _params(4, x, y, w, h);
        _dquot();
    }
    void drawImage(const String& img, int sx, int sy, int sw, int sh, int x, int y, int w, int h) {
        addCmd(30);
        _div();
        _add(img);
        _comma();
        _params(8, sx, sy, sw, sh, x, y, w, h);
        _dquot();
    }

    // сохранить конфигурацию полотна
    void save() {
        addCmd(37);
        _dquot();
    }

    // восстановить конфигурацию полотна
    void restore() {
        addCmd(38);
        _dquot();
    }

   private:
    template <typename T>
    void _add(T v) {
        if (ps) *ps += v;
    }
    void _checkFirst() {
        if (first) first = 0;
        else _add(',');
    }
    void addCmd(int cmd) {
        _checkFirst();
        _dquot();
        _add(cmd);
    }
    void _div() {
        _add(':');
    }
    void _quot() {
        _add('\'');
    }
    void _dquot() {
        _add('\"');
    }
    void _comma() {
        _add(',');
    }
    void _params(int num, ...) {
        va_list valist;
        va_start(valist, num);
        for (int i = 0; i < num; i++) {
            _add(va_arg(valist, int));
            if (i < num - 1) _comma();
        }
        va_end(valist);
    }

    void _color(uint32_t hex, uint8_t a = 255) {
        hex = ((uint32_t)hex << 8) | a;
        _add(hex);
    }
    void _font() {
        addCmd(10);
        _div();
        _quot();
        _add(fsize);
        _add(F("px "));
        _add(fname);
        _quot();
        _dquot();
    }

    String* ps = nullptr;
    bool first = 1;
    bool strokeF = 1;
    bool fillF = 1;
    const char* fname = "Arial";
    int fsize = 20;
    GHmode_t eMode = CV_RADIUS;
    GHmode_t rMode = CV_CORNER;
};