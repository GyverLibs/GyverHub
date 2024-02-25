#pragma once

// HTML Canvas API
// https://www.w3schools.com/tags/ref_canvas.asp
// https://processing.org/reference/

#include <Arduino.h>
#include <StringUtils.h>

#include "hub_macro.hpp"
#include "core/packet.h"

enum class CV : uint8_t {
    BUTT,
    ROUND,
    SQUARE,
    PROJECT,
    BEVEL,
    MITER,
    START,
    END,
    CENTER,
    LEFT,
    RIGHT,
    ALPHABETIC,
    TOP,
    HANGING,
    MIDDLE,
    IDEOGRAPHIC,
    BOTTOM,
    SRC_OVER,
    SRC_ATOP,
    SRC_IN,
    SRC_OUT,
    DST_OVER,
    DST_ATOP,
    DST_IN,
    DST_OUT,
    LIGHTER,
    COPY,
    XOR,
    TXT_TOP,
    TXT_BOTTOM,
    TXT_CENTER,
    TXT_BASELINE,
    CORNER,
    CORNERS,
    RADIUS,
};

namespace gh {

class Canvas {
   public:
    // подключить внешний буфер
    void setBuffer(ghc::Packet* pp) {
        p = pp;
    }

    // добавить строку кода на js
    Canvas& custom(GHTREF text) {
        if (!p) return *this;
        _checkFirst();
        p->quotes();
        p->addText(text);
        p->quotes();
        return *this;
    }

    // =====================================================
    // =============== PROCESSING-LIKE API =================
    // =====================================================

    // =================== BACKGROUND ======================
    // очистить полотно
    Canvas& clear() {
        clearRect(0, 0, -1, -1);
        beginPath();
        return *this;
    }

    // залить полотно установленным в fill() цветом
    Canvas& background() {
        fillRect(0, 0, -1, -1);
        return *this;
    }

    // залить полотно указанным цветом (цвет, прозрачность)
    Canvas& background(uint32_t hex, uint8_t a = 255) {
        fillStyle(hex, a);
        background();
        return *this;
    }

    // ======================== FILL =======================
    // выбрать цвет заливки (цвет, прозрачность)
    Canvas& fill(uint32_t hex, uint8_t a = 255) {
        fillStyle(hex, a);
        _fillF = 1;
        return *this;
    }

    // отключить заливку
    Canvas& noFill() {
        _fillF = 0;
        return *this;
    }

    // ===================== STROKE ====================
    // выбрать цвет обводки (цвет, прозрачность)
    Canvas& stroke(uint32_t hex, uint8_t a = 255) {
        strokeStyle(hex, a);
        _strokeF = 1;
        return *this;
    }

    // отключить обводку
    Canvas& noStroke() {
        _strokeF = 0;
        return *this;
    }

    // толщина обводки, px
    Canvas& strokeWeight(int v) {
        lineWidth(v);
        return *this;
    }

    // соединение линий: CV::MITER (умолч), CV::BEVEL, CV::ROUND
    Canvas& strokeJoin(CV v) {
        lineJoin(v);
        return *this;
    }

    // края линий: CV::PROJECT (умолч), CV::ROUND, CV::SQUARE
    Canvas& strokeCap(CV v) {
        lineCap(v);
        return *this;
    }

    // ===================== PRIMITIVES ====================
    // окружность (x, y, радиус), px
    Canvas& circle(int x, int y, int r) {
        beginPath();
        switch (_eMode) {
            case CV::CORNER:
                arc(x + r, y + r, r);
                break;
            default:
                arc(x, y, r);
                break;
        }
        if (_strokeF) stroke();
        if (_fillF) fill();
        return *this;
    }

    // линия (координаты начала и конца)
    Canvas& line(int x1, int y1, int x2, int y2) {
        beginPath();
        moveTo(x1, y1);
        lineTo(x2, y2);
        stroke();
        return *this;
    }

    // точка
    Canvas& point(int x, int y) {
        beginPath();
        fillRect(x, y, 1, 1);
        return *this;
    }

    // четырёхугольник (координаты углов)
    Canvas& quadrangle(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4) {
        beginPath();
        moveTo(x1, y1);
        lineTo(x2, y2);
        lineTo(x3, y3);
        lineTo(x4, y4);
        closePath();
        if (_strokeF) stroke();
        if (_fillF) fill();
        return *this;
    }

    // треугольник (координаты углов)
    Canvas& triangle(int x1, int y1, int x2, int y2, int x3, int y3) {
        beginPath();
        moveTo(x1, y1);
        lineTo(x2, y2);
        lineTo(x3, y3);
        closePath();
        if (_strokeF) stroke();
        if (_fillF) fill();
        return *this;
    }

    // прямоугольник
    Canvas& rect(int x, int y, int w, int h, int tl = -1, int tr = -1, int br = 0, int bl = 0) {
        beginPath();
        int X = x, Y = y, W = w, H = h;
        switch (_rMode) {
            case CV::CORNER:
                break;
            case CV::CORNERS:
                W = w - x;
                H = h - y;
                break;
            case CV::CENTER:
                X = x - w / 2;
                Y = y - h / 2;
                break;
            case CV::RADIUS:
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
        if (_strokeF) stroke();
        if (_fillF) fill();
        return *this;
    }

    // квадрат
    Canvas& square(int x, int y, int w) {
        rect(x, y, w, w);
        return *this;
    }

    // режим окружности: CV::CENTER (умолч), CV::CORNER
    Canvas& ellipseMode(CV mode) {
        _eMode = mode;
        return *this;
    }

    // режим прямоугольника: CV::CORNER (умолч), CV::CORNERS, CV::CENTER, CV::RADIUS
    Canvas& rectMode(CV mode) {
        _rMode = mode;
        return *this;
    }

    // ======================= TEXT ========================
    // шрифт
    Canvas& textFont(const char* name) {
        _fname = name;
        _font();
        return *this;
    }

    // размер шрифта, px
    Canvas& textSize(int size) {
        _fsize = size;
        _font();
        return *this;
    }

    // вывести текст, опционально макс длина
    Canvas& text(const String& text, int x, int y, int w = 0) {
        if (_strokeF) strokeText(text, x, y, w);
        if (_fillF) fillText(text, x, y, w);
        return *this;
    }

    // выравнивание текста
    // CV::LEFT, CV::CENTER, CV::RIGHT
    // TXT_TOP, TXT_BOTTOM, TXT_CENTER, TXT_BASELINE
    Canvas& textAlign(CV h, CV v) {
        textAlign(h);
        textBaseline(v);
        return *this;
    }

    // сохранить конфигурацию полотна
    Canvas& push() {
        save();
        return *this;
    }

    // восстановить конфигурацию полотна
    Canvas& pop() {
        restore();
        return *this;
    }

    // ======================================================
    // ================== HTML CANVAS API ===================
    // ======================================================

    // цвет заполнения
    Canvas& fillStyle(uint32_t hex, uint8_t a = 255) {
        if (!p) return *this;
        _cmd(0);
        p->colon();
        _color(hex, a);
        p->quotes();
        return *this;
    }

    // цвет обводки
    Canvas& strokeStyle(uint32_t hex, uint8_t a = 255) {
        if (!p) return *this;
        _cmd(1);
        p->colon();
        _color(hex, a);
        p->quotes();
        return *this;
    }

    // цвет тени
    Canvas& shadowColor(uint32_t hex, uint8_t a = 255) {
        if (!p) return *this;
        _cmd(2);
        p->colon();
        _color(hex, a);
        p->quotes();
        return *this;
    }

    // размытость тени, px
    Canvas& shadowBlur(int v) {
        if (!p) return *this;
        _cmd(3);
        _addP(v);
        return *this;
    }

    // отступ тени, px
    Canvas& shadowOffsetX(int v) {
        if (!p) return *this;
        _cmd(4);
        _addP(v);
        return *this;
    }

    // отступ тени, px
    Canvas& shadowOffsetY(int v) {
        if (!p) return *this;
        _cmd(5);
        _addP(v);
        return *this;
    }

    // края линий: CV::BUTT (умолч), CV::ROUND, CV::SQUARE
    // https://www.w3schools.com/tags/canvas_linecap.asp
    Canvas& lineCap(CV v) {
        if (!p) return *this;
        _cmd(11);
        _addP((uint8_t)v);
        return *this;
    }

    // соединение линий: CV::MITER (умолч), CV::BEVEL, CV::ROUND
    // https://www.w3schools.com/tags/canvas_linejoin.asp
    Canvas& lineJoin(CV v) {
        if (!p) return *this;
        _cmd(12);
        _addP((uint8_t)v);
        return *this;
    }

    // ширина линий, px
    Canvas& lineWidth(int v) {
        if (!p) return *this;
        _cmd(6);
        _addP(v);
        return *this;
    }

    // длина соединения CV::MITER, px
    // https://www.w3schools.com/tags/canvas_miterlimit.asp
    Canvas& miterLimit(int v) {
        if (!p) return *this;
        _cmd(7);
        _addP(v);
        return *this;
    }

    // шрифт: "30px Arial"
    // https://www.w3schools.com/tags/canvas_font.asp
    Canvas& font(GHTREF v) {
        if (!p) return *this;
        _cmd(8);
        p->colon();
        p->addText(v);
        p->quotes();
        return *this;
    }

    // выравнивание текста: CV::START (умолч), CV::END, CV::CENTER, CV::LEFT, CV::RIGHT
    // https://www.w3schools.com/tags/canvas_textalign.asp
    Canvas& textAlign(CV v) {
        if (!p) return *this;
        _cmd(9);
        _addP((uint8_t)v);
        return *this;
    }

    // позиция текста: CV::ALPHABETIC (умолч), CV::TOP, CV::HANGING, CV::MIDDLE, CV::IDEOGRAPHIC, CV::BOTTOM
    // https://www.w3schools.com/tags/canvas_textbaseline.asp
    Canvas& textBaseline(CV v) {
        if (!p) return *this;
        _cmd(10);
        _addP((uint8_t)v);
        return *this;
    }

    // прозрачность рисовки, 0.0-1.0
    Canvas& globalAlpha(float v) {
        if (!p) return *this;
        _cmd(14);
        _addP(v);
        return *this;
    }

    // тип наложения графики: CV::SRC_OVER (умолч), CV::SRC_ATOP, CV::SRC_IN, CV::SRC_OUT, CV::DST_OVER, CV::DST_ATOP, CV::DST_IN, CV::DST_OUT, CV::LIGHTER, CV::COPY, CV::XOR
    // https://www.w3schools.com/tags/canvas_globalcompositeoperation.asp
    Canvas& globalCompositeOperation(CV v) {
        if (!p) return *this;
        _cmd(13);
        _addP((uint8_t)v);
        return *this;
    }

    // прямоугольник
    Canvas& drawRect(int x, int y, int w, int h) {
        if (!p) return *this;
        _cmd(17);
        p->colon();
        _params(4, x, y, w, h);
        p->quotes();
        return *this;
    }

    // скруглённый прямоугольник
    Canvas& roundRect(int x, int y, int w, int h, int tl = 0, int tr = -1, int br = -1, int bl = -1) {
        if (!p) return *this;
        _cmd(31);
        p->colon();
        if (tr < 0) _params(5, x, y, w, h, tl);
        else if (br < 0) _params(6, x, y, w, h, tl, tr);
        else _params(8, x, y, w, h, tl, tr, br, bl);
        p->quotes();
        return *this;
    }

    // закрашенный прямоугольник
    Canvas& fillRect(int x, int y, int w, int h) {
        if (!p) return *this;
        _cmd(18);
        p->colon();
        _params(4, x, y, w, h);
        p->quotes();
        return *this;
    }

    // обведённый прямоугольник
    Canvas& strokeRect(int x, int y, int w, int h) {
        if (!p) return *this;
        _cmd(19);
        p->colon();
        _params(4, x, y, w, h);
        p->quotes();
        return *this;
    }

    // очистить область
    Canvas& clearRect(int x, int y, int w, int h) {
        if (!p) return *this;
        _cmd(20);
        p->colon();
        _params(4, x, y, w, h);
        p->quotes();
        return *this;
    }

    // залить
    Canvas& fill() {
        if (!p) return *this;
        _cmd(32);
        p->quotes();
        return *this;
    }

    // обвести
    Canvas& stroke() {
        if (!p) return *this;
        _cmd(33);
        p->quotes();
        return *this;
    }

    // начать путь
    Canvas& beginPath() {
        if (!p) return *this;
        _cmd(34);
        p->quotes();
        return *this;
    }

    // переместить курсор
    Canvas& moveTo(int x, int y) {
        if (!p) return *this;
        _cmd(21);
        p->colon();
        _params(2, x, y);
        p->quotes();
        return *this;
    }

    // завершить путь (провести линию на начало)
    Canvas& closePath() {
        if (!p) return *this;
        _cmd(35);
        p->quotes();
        return *this;
    }

    // нарисовать линию от курсора
    Canvas& lineTo(int x, int y) {
        if (!p) return *this;
        _cmd(22);
        p->colon();
        _params(2, x, y);
        p->quotes();
        return *this;
    }

    // ограничить область рисования
    // https://www.w3schools.com/tags/canvas_clip.asp
    Canvas& clip() {
        if (!p) return *this;
        _cmd(36);
        p->quotes();
        return *this;
    }

    // провести кривую
    // https://www.w3schools.com/tags/canvas_quadraticcurveto.asp
    Canvas& quadraticCurveTo(int cpx, int cpy, int x, int y) {
        if (!p) return *this;
        _cmd(23);
        p->colon();
        _params(4, cpx, cpy, x, y);
        p->quotes();
        return *this;
    }

    // провести кривую Безье
    // https://www.w3schools.com/tags/canvas_beziercurveto.asp
    Canvas& bezierCurveTo(int cp1x, int cp1y, int cp2x, int cp2y, int x, int y) {
        if (!p) return *this;
        _cmd(24);
        p->colon();
        _params(6, cp1x, cp1y, cp2x, cp2y, x, y);
        p->quotes();
        return *this;
    }

    // провести дугу (радианы)
    // https://www.w3schools.com/tags/canvas_arc.asp
    Canvas& arc(int x, int y, int r, float sa = 0, float ea = TWO_PI, bool ccw = 0) {
        if (!p) return *this;
        _cmd(27);
        p->colon();
        _params(3, x, y, r);
        p->comma();
        p->s += sa;
        p->comma();
        p->s += ea;
        p->s += ccw;
        p->quotes();
        return *this;
    }

    // скруглить
    // https://www.w3schools.com/tags/canvas_arcto.asp
    Canvas& arcTo(int x1, int y1, int x2, int y2, int r) {
        if (!p) return *this;
        _cmd(26);
        p->colon();
        _params(5, x1, y1, x2, y2, r);
        p->quotes();
    }

    // масштабировать область рисования
    // https://www.w3schools.com/tags/canvas_scale.asp
    Canvas& scale(int sw, int sh) {
        if (!p) return *this;
        _cmd(15);
        p->colon();
        _params(2, sw, sh);
        p->quotes();
        return *this;
    }

    // вращать область рисования (в радианах)
    // https://www.w3schools.com/tags/canvas_rotate.asp
    Canvas& rotate(float v) {
        if (!p) return *this;
        _cmd(16);
        _addP(v);
        return *this;
    }

    // перемещать область рисования
    // https://www.w3schools.com/tags/canvas_translate.asp
    Canvas& translate(int x, int y) {
        if (!p) return *this;
        _cmd(25);
        p->colon();
        _params(2, x, y);
        p->quotes();
        return *this;
    }

    // вывести закрашенный текст, опционально макс. длина
    Canvas& fillText(GHTREF text, int x, int y, int w = 0) {
        if (!p) return *this;
        _cmd(28);
        p->colon();
        p->quotes();
        p->addStringEsc(text);
        p->quotes();
        p->comma();
        _params(3, x, y, w);
        p->quotes();
        return *this;
    }

    // вывести обведённый текст, опционально макс. длина
    Canvas& strokeText(GHTREF text, int x, int y, int w = 0) {
        if (!p) return *this;
        _cmd(29);
        p->colon();
        p->quotes();
        p->addStringEsc(text);
        p->quotes();
        p->comma();
        _params(3, x, y, w);
        p->quotes();
        return *this;
    }

    // вывести картинку
    // https://www.w3schools.com/tags/canvas_drawimage.asp
    Canvas& drawImage(GHTREF img, int x, int y) {
        if (!p) return *this;
        _cmd(30);
        p->colon();
        p->addText(img);
        p->comma();
        _params(2, x, y);
        p->quotes();
        return *this;
    }
    Canvas& drawImage(GHTREF img, int x, int y, int w) {
        if (!p) return *this;
        _cmd(30);
        p->colon();
        p->addText(img);
        p->comma();
        _params(3, x, y, w);
        p->quotes();
        return *this;
    }
    Canvas& drawImage(GHTREF img, int x, int y, int w, int h) {
        if (!p) return *this;
        _cmd(30);
        p->colon();
        p->addText(img);
        p->comma();
        _params(4, x, y, w, h);
        p->quotes();
        return *this;
    }
    Canvas& drawImage(GHTREF img, int sx, int sy, int sw, int sh, int x, int y, int w, int h) {
        if (!p) return *this;
        _cmd(30);
        p->colon();
        p->addText(img);
        p->comma();
        _params(8, sx, sy, sw, sh, x, y, w, h);
        p->quotes();
        return *this;
    }

    // сохранить конфигурацию полотна
    Canvas& save() {
        if (!p) return *this;
        _cmd(37);
        p->quotes();
        return *this;
    }

    // восстановить конфигурацию полотна
    Canvas& restore() {
        if (!p) return *this;
        _cmd(38);
        p->quotes();
        return *this;
    }

   private:
    void _checkFirst() {
        if (!p) return;
        if (_first) _first = 0;
        else p->comma();
    }
    void _cmd(int cmd) {
        if (!p) return;
        _checkFirst();
        p->quotes();
        p->s += cmd;
    }
    void _params(int num, ...) {
        va_list valist;
        va_start(valist, num);
        for (int i = 0; i < num; i++) {
            p->s += va_arg(valist, int);
            if (i < num - 1) p->comma();
        }
        va_end(valist);
    }

    void _color(uint32_t hex, uint8_t a = 255) {
        p->s += ((uint32_t)hex << 8) | a;
    }
    void _font() {
        if (!p) return;
        _cmd(10);
        p->colon();
        p->s += '\'';
        p->s += _fsize;
        p->s += F("px ");
        p->s += _fname;
        p->s += '\'';
        p->quotes();
    }

    void _addP(int val) {
        p->colon();
        p->s += val;
        p->quotes();
    }

    ghc::Packet* p = nullptr;
    bool _first = 1;
    bool _strokeF = 1;
    bool _fillF = 1;
    const char* _fname = "Arial";
    uint16_t _fsize = 20;
    CV _eMode = CV::RADIUS;
    CV _rMode = CV::CORNER;
};

}  // namespace gh