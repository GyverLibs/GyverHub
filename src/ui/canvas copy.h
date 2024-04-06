#pragma once

// HTML Canvas API
// https://www.w3schools.com/tags/ref_canvas.asp
// https://processing.org/reference/

#include <Arduino.h>
#include <StringUtils.h>

#include "core/packet.h"
#include "geo.h"
#include "hub_macro.hpp"
#include "location.h"

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
   private:
    enum class api : uint8_t {
        fillStyle,
        strokeStyle,
        shadowColor,
        shadowBlur,
        shadowOffsetX,
        shadowOffsetY,
        lineWidth,
        miterLimit,
        font,
        textAlign,
        textBaseline,
        lineCap,
        lineJoin,
        globalCompositeOperation,
        globalAlpha,
        scale,
        rotate,
        rect,
        fillRect,
        strokeRect,
        clearRect,
        moveTo,
        lineTo,
        quadraticCurveTo,
        bezierCurveTo,
        translate,
        arcTo,
        arc,
        fillText,
        strokeText,
        drawImage,
        roundRect,
        fill,
        stroke,
        beginPath,
        closePath,
        clip,
        save,
        restore,
        clear,
        pixelScale,
    };

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

    // игнорировать масштаб холста (превращает метры в пиксели для Map)
    Canvas& pixelScale(bool ignore) {
        if (!p) return *this;
        _cmd(api::pixelScale);
        _addP((uint8_t)ignore);
        return *this;
    }

    // =====================================================
    // =============== PROCESSING-LIKE API =================
    // =====================================================

    // =================== BACKGROUND ======================
    // очистить полотно
    Canvas& clear() {
        // clearRect(0, 0, -1, -1);
        if (!p) return *this;
        _cmd(api::clear);
        p->quotes();

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
    Canvas& circle(long x, long y, long r) {
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
    Canvas& circle(const Geo& geo, long r) {
        return circle(geo._latE6(), geo._lonE6(), r);
    }

    // линия (координаты начала и конца)
    Canvas& line(long x1, long y1, long x2, long y2) {
        beginPath();
        moveTo(x1, y1);
        lineTo(x2, y2);
        stroke();
        return *this;
    }
    Canvas& line(const Geo& geo1, const Geo& geo2) {
        return line(geo1._latE6(), geo1._lonE6(), geo2._latE6(), geo2._lonE6());
    }

    // точка
    Canvas& point(long x, long y) {
        beginPath();
        fillRect(x, y, 1, 1);
        return *this;
    }
    Canvas& point(const Geo& geo) {
        return point(geo._latE6(), geo._lonE6());
    }

    // четырёхугольник (координаты углов)
    Canvas& quadrangle(long x1, long y1, long x2, long y2, long x3, long y3, long x4, long y4) {
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
    Canvas& quadrangle(const Geo& geo1, const Geo& geo2, const Geo& geo3, const Geo& geo4) {
        return quadrangle(geo1._latE6(), geo1._lonE6(), geo2._latE6(), geo2._lonE6(), geo3._latE6(), geo3._lonE6(), geo4._latE6(), geo4._lonE6());
    }

    // треугольник (координаты углов)
    Canvas& triangle(long x1, long y1, long x2, long y2, long x3, long y3) {
        beginPath();
        moveTo(x1, y1);
        lineTo(x2, y2);
        lineTo(x3, y3);
        closePath();
        if (_strokeF) stroke();
        if (_fillF) fill();
        return *this;
    }
    Canvas& triangle(const Geo& geo1, const Geo& geo2, const Geo& geo3) {
        return triangle(geo1._latE6(), geo1._lonE6(), geo2._latE6(), geo2._lonE6(), geo3._latE6(), geo3._lonE6());
    }

    // прямоугольник
    Canvas& rect(long x, long y, long w, long h, long tl = -1, long tr = -1, long br = 0, long bl = 0) {
        beginPath();
        long X = x, Y = y, W = w, H = h;
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
    Canvas& rect(const Geo& geo, long w, long h, long tl = -1, long tr = -1, long br = 0, long bl = 0) {
        rectMode(CV::CORNER);
        return rect(geo._latE6(), geo._lonE6(), w, h, tl, tr, br, bl);
    }

    // квадрат
    Canvas& square(long x, long y, long w) {
        rect(x, y, w, w);
        return *this;
    }
    Canvas& square(const Geo& geo, long w) {
        return rect(geo, w, w);
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
    Canvas& text(GHTREF txt, long x, long y, long w = 0) {
        if (_strokeF) strokeText(txt, x, y, w);
        if (_fillF) fillText(txt, x, y, w);
        return *this;
    }
    Canvas& text(GHTREF txt, const Geo& geo, long w = 0) {
        return text(txt, geo._latE6(), geo._lonE6(), w);
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
        _cmd(api::fillStyle);
        p->colon();
        _color(hex, a);
        p->quotes();
        return *this;
    }

    // цвет обводки
    Canvas& strokeStyle(uint32_t hex, uint8_t a = 255) {
        if (!p) return *this;
        _cmd(api::strokeStyle);
        p->colon();
        _color(hex, a);
        p->quotes();
        return *this;
    }

    // цвет тени
    Canvas& shadowColor(uint32_t hex, uint8_t a = 255) {
        if (!p) return *this;
        _cmd(api::shadowColor);
        p->colon();
        _color(hex, a);
        p->quotes();
        return *this;
    }

    // размытость тени, px
    Canvas& shadowBlur(int v) {
        if (!p) return *this;
        _cmd(api::shadowBlur);
        _addP(v);
        return *this;
    }

    // отступ тени, px
    Canvas& shadowOffsetX(int v) {
        if (!p) return *this;
        _cmd(api::shadowOffsetX);
        _addP(v);
        return *this;
    }

    // отступ тени, px
    Canvas& shadowOffsetY(int v) {
        if (!p) return *this;
        _cmd(api::shadowOffsetY);
        _addP(v);
        return *this;
    }

    // края линий: CV::BUTT (умолч), CV::ROUND, CV::SQUARE
    // https://www.w3schools.com/tags/canvas_linecap.asp
    Canvas& lineCap(CV v) {
        if (!p) return *this;
        _cmd(api::lineCap);
        _addP((uint8_t)v);
        return *this;
    }

    // соединение линий: CV::MITER (умолч), CV::BEVEL, CV::ROUND
    // https://www.w3schools.com/tags/canvas_linejoin.asp
    Canvas& lineJoin(CV v) {
        if (!p) return *this;
        _cmd(api::lineJoin);
        _addP((uint8_t)v);
        return *this;
    }

    // ширина линий, px
    Canvas& lineWidth(int v) {
        if (!p) return *this;
        _cmd(api::lineWidth);
        _addP(v);
        return *this;
    }

    // длина соединения CV::MITER, px
    // https://www.w3schools.com/tags/canvas_miterlimit.asp
    Canvas& miterLimit(int v) {
        if (!p) return *this;
        _cmd(api::miterLimit);
        _addP(v);
        return *this;
    }

    // шрифт: "30px Arial"
    // https://www.w3schools.com/tags/canvas_font.asp
    Canvas& font(GHTREF v) {
        if (!p) return *this;
        _cmd(api::font);
        p->colon();
        p->addText(v);
        p->quotes();
        return *this;
    }

    // выравнивание текста: CV::START (умолч), CV::END, CV::CENTER, CV::LEFT, CV::RIGHT
    // https://www.w3schools.com/tags/canvas_textalign.asp
    Canvas& textAlign(CV v) {
        if (!p) return *this;
        _cmd(api::textAlign);
        _addP((uint8_t)v);
        return *this;
    }

    // позиция текста: CV::ALPHABETIC (умолч), CV::TOP, CV::HANGING, CV::MIDDLE, CV::IDEOGRAPHIC, CV::BOTTOM
    // https://www.w3schools.com/tags/canvas_textbaseline.asp
    Canvas& textBaseline(CV v) {
        if (!p) return *this;
        _cmd(api::textBaseline);
        _addP((uint8_t)v);
        return *this;
    }

    // прозрачность рисовки, 0.0-1.0
    Canvas& globalAlpha(float v) {
        if (!p) return *this;
        _cmd(api::globalAlpha);
        _addP(v);
        return *this;
    }

    // тип наложения графики: CV::SRC_OVER (умолч), CV::SRC_ATOP, CV::SRC_IN, CV::SRC_OUT, CV::DST_OVER, CV::DST_ATOP, CV::DST_IN, CV::DST_OUT, CV::LIGHTER, CV::COPY, CV::XOR
    // https://www.w3schools.com/tags/canvas_globalcompositeoperation.asp
    Canvas& globalCompositeOperation(CV v) {
        if (!p) return *this;
        _cmd(api::globalCompositeOperation);
        _addP((uint8_t)v);
        return *this;
    }

    // прямоугольник
    Canvas& drawRect(long x, long y, long w, long h) {
        if (!p) return *this;
        _cmd(api::rect);
        p->colon();
        _params(4, x, y, w, h);
        p->quotes();
        return *this;
    }
    Canvas& drawRect(const Geo& geo, long w, long h) {
        return drawRect(geo._latE6(), geo._lonE6(), w, h);
    }

    // скруглённый прямоугольник
    Canvas& roundRect(long x, long y, long w, long h, long tl = 0, long tr = -1, long br = -1, long bl = -1) {
        if (!p) return *this;
        _cmd(api::roundRect);
        p->colon();
        if (tr < 0) _params(5, x, y, w, h, tl);
        else if (br < 0) _params(6, x, y, w, h, tl, tr);
        else _params(8, x, y, w, h, tl, tr, br, bl);
        p->quotes();
        return *this;
    }
    Canvas& roundRect(const Geo& geo, long w, long h, long tl = 0, long tr = -1, long br = -1, long bl = -1) {
        return roundRect(geo._latE6(), geo._lonE6(), w, h, tl, tr, br, bl);
    }

    // закрашенный прямоугольник
    Canvas& fillRect(long x, long y, long w, long h) {
        if (!p) return *this;
        _cmd(api::fillRect);
        p->colon();
        _params(4, x, y, w, h);
        p->quotes();
        return *this;
    }
    Canvas& fillRect(const Geo& geo, long w, long h) {
        return fillRect(geo._latE6(), geo._lonE6(), w, h);
    }

    // обведённый прямоугольник
    Canvas& strokeRect(long x, long y, long w, long h) {
        if (!p) return *this;
        _cmd(api::strokeRect);
        p->colon();
        _params(4, x, y, w, h);
        p->quotes();
        return *this;
    }
    Canvas& strokeRect(const Geo& geo, long w, long h) {
        return strokeRect(geo._latE6(), geo._lonE6(), w, h);
    }

    // очистить область
    Canvas& clearRect(long x, long y, long w, long h) {
        if (!p) return *this;
        _cmd(api::clearRect);
        p->colon();
        _params(4, x, y, w, h);
        p->quotes();
        return *this;
    }
    Canvas& clearRect(const Geo& geo, long w, long h) {
        return clearRect(geo._latE6(), geo._lonE6(), w, h);
    }

    // залить
    Canvas& fill() {
        if (!p) return *this;
        _cmd(api::fill);
        p->quotes();
        return *this;
    }

    // обвести
    Canvas& stroke() {
        if (!p) return *this;
        _cmd(api::stroke);
        p->quotes();
        return *this;
    }

    // начать путь
    Canvas& beginPath() {
        if (!p) return *this;
        _cmd(api::beginPath);
        p->quotes();
        return *this;
    }

    // переместить курсор
    Canvas& moveTo(long x, long y) {
        if (!p) return *this;
        _cmd(api::moveTo);
        p->colon();
        _params(2, x, y);
        p->quotes();
        return *this;
    }

    // завершить путь (провести линию на начало)
    Canvas& closePath() {
        if (!p) return *this;
        _cmd(api::closePath);
        p->quotes();
        return *this;
    }

    // нарисовать линию от курсора
    Canvas& lineTo(long x, long y) {
        if (!p) return *this;
        _cmd(api::lineTo);
        p->colon();
        _params(2, x, y);
        p->quotes();
        return *this;
    }
    Canvas& lineTo(const Geo& geo) {
        return lineTo(geo._latE6(), geo._lonE6());
    }

    // ограничить область рисования
    // https://www.w3schools.com/tags/canvas_clip.asp
    Canvas& clip() {
        if (!p) return *this;
        _cmd(api::clip);
        p->quotes();
        return *this;
    }

    // провести кривую
    // https://www.w3schools.com/tags/canvas_quadraticcurveto.asp
    Canvas& quadraticCurveTo(long cpx, long cpy, long x, long y) {
        if (!p) return *this;
        _cmd(api::quadraticCurveTo);
        p->colon();
        _params(4, cpx, cpy, x, y);
        p->quotes();
        return *this;
    }
    Canvas& quadraticCurveTo(const Geo& cp, const Geo& xy) {
        return quadraticCurveTo(cp._latE6(), cp._lonE6(), xy._latE6(), xy._lonE6());
    }

    // провести кривую Безье
    // https://www.w3schools.com/tags/canvas_beziercurveto.asp
    Canvas& bezierCurveTo(long cp1x, long cp1y, long cp2x, long cp2y, long x, long y) {
        if (!p) return *this;
        _cmd(api::bezierCurveTo);
        p->colon();
        _params(6, cp1x, cp1y, cp2x, cp2y, x, y);
        p->quotes();
        return *this;
    }
    Canvas& bezierCurveTo(const Geo& cp1, const Geo& cp2, const Geo& xy) {
        return bezierCurveTo(cp1._latE6(), cp1._lonE6(), cp2._latE6(), cp2._lonE6(), xy._latE6(), xy._lonE6());
    }

    // провести дугу (радианы)
    // https://www.w3schools.com/tags/canvas_arc.asp
    Canvas& arc(long x, long y, long r, float sa = 0, float ea = TWO_PI, bool ccw = 0) {
        if (!p) return *this;
        _cmd(api::arc);
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
    Canvas& arc(const Geo& geo, long r, float sa = 0, float ea = TWO_PI, bool ccw = 0) {
        return arc(geo._latE6(), geo._lonE6(), r, sa, ea, ccw);
    }

    // скруглить
    // https://www.w3schools.com/tags/canvas_arcto.asp
    Canvas& arcTo(long x1, long y1, long x2, long y2, long r) {
        if (!p) return *this;
        _cmd(api::arcTo);
        p->colon();
        _params(5, x1, y1, x2, y2, r);
        p->quotes();
    }
    Canvas& arcTo(const Geo& geo1, const Geo& geo2, long r) {
        return arcTo(geo1._latE6(), geo1._lonE6(), geo2._latE6(), geo2._lonE6(), r);
    }

    // масштабировать область рисования
    // https://www.w3schools.com/tags/canvas_scale.asp
    Canvas& scale(long sw, long sh) {
        if (!p) return *this;
        _cmd(api::scale);
        p->colon();
        _params(2, sw, sh);
        p->quotes();
        return *this;
    }

    // вращать область рисования (в радианах)
    // https://www.w3schools.com/tags/canvas_rotate.asp
    Canvas& rotate(float v) {
        if (!p) return *this;
        _cmd(api::rotate);
        _addP(v);
        return *this;
    }

    // перемещать область рисования
    // https://www.w3schools.com/tags/canvas_translate.asp
    Canvas& translate(long x, long y) {
        if (!p) return *this;
        _cmd(api::translate);
        p->colon();
        _params(2, x, y);
        p->quotes();
        return *this;
    }

    // вывести закрашенный текст, опционально макс. длина
    Canvas& fillText(GHTREF text, long x, long y, long w = 0) {
        if (!p) return *this;
        _cmd(api::fillText);
        p->colon();
        p->addTextEsc(text);
        p->comma();
        _params(3, x, y, w);
        p->quotes();
        return *this;
    }
    Canvas& fillText(GHTREF text, const Geo& geo, long w = 0) {
        return fillText(text, geo._latE6(), geo._lonE6(), w);
    }

    // вывести обведённый текст, опционально макс. длина
    Canvas& strokeText(GHTREF text, long x, long y, long w = 0) {
        if (!p) return *this;
        _cmd(api::strokeText);
        p->colon();
        p->addTextEsc(text);
        p->comma();
        _params(3, x, y, w);
        p->quotes();
        return *this;
    }
    Canvas& strokeText(GHTREF text, const Geo& geo, long w = 0) {
        return strokeText(text, geo._latE6(), geo._lonE6(), w);
    }

    // вывести картинку
    // https://www.w3schools.com/tags/canvas_drawimage.asp
    Canvas& drawImage(GHTREF path, long x, long y) {
        if (!p) return *this;
        _cmd(api::drawImage);
        p->colon();
        p->addText(path);
        p->comma();
        _params(2, x, y);
        p->quotes();
        return *this;
    }
    Canvas& drawImage(GHTREF path, const Geo& geo) {
        return drawImage(path, geo._latE6(), geo._lonE6());
    }

    Canvas& drawImage(GHTREF path, long x, long y, long w) {
        if (!p) return *this;
        _cmd(api::drawImage);
        p->colon();
        p->addText(path);
        p->comma();
        _params(3, x, y, w);
        p->quotes();
        return *this;
    }
    Canvas& drawImage(GHTREF path, const Geo& geo, long w) {
        return drawImage(path, geo._latE6(), geo._lonE6(), w);
    }

    Canvas& drawImage(GHTREF path, long x, long y, long w, long h) {
        if (!p) return *this;
        _cmd(api::drawImage);
        p->colon();
        p->addText(path);
        p->comma();
        _params(4, x, y, w, h);
        p->quotes();
        return *this;
    }
    Canvas& drawImage(GHTREF path, const Geo& geo, long w, long h) {
        return drawImage(path, geo._latE6(), geo._lonE6(), w, h);
    }

    Canvas& drawImage(GHTREF path, long sx, long sy, long sw, long sh, long x, long y, long w, long h) {
        if (!p) return *this;
        _cmd(api::drawImage);
        p->colon();
        p->addText(path);
        p->comma();
        _params(8, sx, sy, sw, sh, x, y, w, h);
        p->quotes();
        return *this;
    }

    // сохранить конфигурацию полотна
    Canvas& save() {
        if (!p) return *this;
        _cmd(api::save);
        p->quotes();
        return *this;
    }

    // восстановить конфигурацию полотна
    Canvas& restore() {
        if (!p) return *this;
        _cmd(api::restore);
        p->quotes();
        return *this;
    }

   private:
    void _checkFirst() {
        if (!p) return;
        if (_first) {
            _first = 0;
            p->beginArr(ghc::Tag::data);
        } else {
            p->comma();
        }
    }
    void _cmd(api cmd) {
        if (!p) return;
        _checkFirst();
        p->quotes();
        p->s += (uint8_t)cmd;
    }
    void _params(long num, ...) {
        va_list valist;
        va_start(valist, num);
        for (int i = 0; i < num; i++) {
            su::Value v(va_arg(valist, long));
            v.addString(p->s);
            if (i < num - 1) p->comma();
        }
        va_end(valist);
    }

    void _color(uint32_t hex, uint8_t a = 255) {
        p->s += ((uint32_t)hex << 8) | a;
    }
    void _font() {
        if (!p) return;
        _cmd(api::font);
        p->colon();
        p->s += _fsize;
        p->s += F("px ");
        p->s += _fname;
        p->quotes();
    }

    template <typename T>
    void _addP(T val) {
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