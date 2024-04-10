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

enum class cv : uint8_t {
    MITER,
    ROUND,
    BEVEL,
    SQUARE,
    PROJECT,
    CORNER,
    CORNERS,
    CENTER,
    RADIUS,
    LEFT,
    RIGHT,
    TOP,
    BOTTOM,
    BASELINE,
};

namespace gh {

class Canvas {
   private:
    enum class api : uint8_t {
        clear,
        background,
        fill,
        noFill,
        stroke,
        noStroke,
        strokeWeight,
        strokeJoin,
        strokeCap,
        rectMode,
        ellipseMode,

        imageMode,
        image,

        textFont,
        textSize,
        textAlign,
        text,

        point,
        line,
        rect,
        arc,
        ellipse,
        circle,
        bezier,

        beginShape,
        endShape,
        vertex,
        bezierVertex,

        pixelScale,
        rotate,
        translate,
        push,
        pop,
    };

   public:
    Canvas() {}
    Canvas(ghc::Packet* packet) : p(packet) {}

    // =================== SYSTEM ===================
    // подключить внешний буфер
    void setBuffer(ghc::Packet* packet) {
        p = packet;
    }

    // добавить строку кода на js
    void custom(GHTREF text) {
        if (!p) return;
        _checkFirst();
        p->addStringEsc(text);
    }

    // true - координаты в пикселях, false (умолч.) - приведённые координаты для Canvas, метры для карты Map
    void pixelScale(bool pix) {
        _cmd(api::pixelScale, 1, pix);
    }

    // =================== BACK ===================

    // очистить
    void clear() {
        _cmd(api::clear);
    }

    // залить полотно указанным цветом, умолч. 0xddd
    void background(const Color& color, uint8_t a = 255) {
        _color(api::background, color, a);
    }
    void background(uint32_t hex, uint8_t a = 255) {
        _color(api::background, hex, a);
    }
    void background(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
        _color(api::background, r, g, b, a);
    }

    // =================== FILL ===================

    // выбрать цвет заливки (цвет, прозрачность), умолч. 0xfff
    void fill(const Color& color, uint8_t a = 255) {
        _color(api::fill, color, a);
    }
    void fill(uint32_t hex, uint8_t a = 255) {
        _color(api::fill, hex, a);
    }
    void fill(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
        _color(api::fill, r, g, b, a);
    }

    // отключить заливку
    void noFill() {
        _cmd(api::noFill);
    }

    // =================== STROKE ===================

    // выбрать цвет обводки (цвет, прозрачность), умолч. 0x000
    void stroke(const Color& color, uint8_t a = 255) {
        _color(api::stroke, color, a);
    }
    void stroke(uint32_t hex, uint8_t a = 255) {
        _color(api::stroke, hex, a);
    }
    void stroke(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
        _color(api::stroke, r, g, b, a);
    }

    // отключить обводку
    void noStroke() {
        _cmd(api::noStroke);
    }

    // толщина обводки
    void strokeWeight(int v) {
        _cmd(api::strokeWeight, 1, v);
    }

    // соединение линий: MITER (умолч.), ROUND, BEVEL
    // https://processing.org/reference/strokeJoin_.html
    void strokeJoin(cv mode) {
        _cmd(api::strokeJoin, mode);
    }

    // края линий: ROUND (умолч.), SQUARE, PROJECT
    // https://processing.org/reference/strokeCap_.html
    void strokeCap(cv mode) {
        _cmd(api::strokeCap, mode);
    }

    // =================== MODE ===================

    // режим прямоугольника: CORNER (умолч.), CORNERS, CENTER, RADIUS
    // https://processing.org/reference/rectMode_.html
    void rectMode(cv mode) {
        _cmd(api::rectMode, mode);
    }

    // режим окружности: CENTER (умолч.), RADIUS, CORNER, CORNERS
    // https://processing.org/reference/ellipseMode_.html
    void ellipseMode(cv mode) {
        _cmd(api::ellipseMode, mode);
    }

    // =================== IMAGE ===================

    // режим изображения: CORNER (умолч.), CORNERS, CENTER
    // https://processing.org/reference/imageMode_.html
    void imageMode(cv mode) {
        _cmd(api::imageMode, mode);
    }

    // вывести изображение по пути (из файла или из Интернета)
    // https://processing.org/reference/image_.html
    void image(GHTREF path, long x, long y) {
        _cmdText(api::image, path, 2, x, y);
    }
    void image(GHTREF path, long x, long y, long w) {
        _cmdText(api::image, path, 3, x, y, w);
    }
    void image(GHTREF path, long x, long y, long w, long h) {
        _cmdText(api::image, path, 4, x, y, w, h);
    }

    void image(GHTREF path, Geo xy) {
        image(path, xy._latE6(), xy._lonE6());
    }
    void image(GHTREF path, Geo xy, long w) {
        image(path, xy._latE6(), xy._lonE6(), w);
    }
    void image(GHTREF path, Geo xy, long w, long h) {
        image(path, xy._latE6(), xy._lonE6(), w, h);
    }

    // =================== TEXT ===================

    // шрифт (умолч. "sans-serif")
    void textFont(GHTREF font) {
        _cmdText(api::textFont, font, 0);
    }

    // размер шрифта (умолч. 10px)
    void textSize(int size) {
        _cmd(api::textSize, 1, size);
    }

    // выравнивание текста (LEFT (умолч.), CENTER, RIGHT), (BASELINE (умолч.), TOP, BOTTOM, CENTER)
    // https://processing.org/reference/textAlign_.html
    void textAlign(cv x, cv y = cv::BASELINE) {
        _cmd(api::textAlign, 2, (int)x, (int)y);
    }

    // вывести текст
    void text(GHTREF txt, long x, long y) {
        _cmdText(api::text, txt, 2, x, y);
    }
    void text(GHTREF txt, Geo xy) {
        text(txt, xy._latE6(), xy._lonE6());
    }

    // =================== PRIMITIVES ===================
    // точка
    // https://processing.org/reference/point_.html
    void point(long x, long y) {
        _cmd(api::point, 2, x, y);
    }
    void point(Geo xy) {
        point(xy._latE6(), xy._lonE6());
    }

    // линия
    // https://processing.org/reference/line_.html
    void line(long x1, long y1, long x2, long y2) {
        _cmd(api::line, 4, x1, y1, x2, y2);
    }
    void line(Geo xy1, Geo xy2) {
        line(xy1._latE6(), xy1._lonE6(), xy2._latE6(), xy2._lonE6());
    }

    // прямоугольник
    // https://processing.org/reference/rect_.html
    void rect(long x, long y, long w, long h) {
        _cmd(api::rect, 4, x, y, w, h);
    }
    void rect(long x, long y, long w, long h, long r) {
        _cmd(api::rect, 5, x, y, w, h, r);
    }
    void rect(long x, long y, long w, long h, long tl, long tr, long br, long bl) {
        _cmd(api::rect, 8, x, y, w, h, tl, tr, br, bl);
    }

    void rect(Geo xy, long w, long h) {
        rect(xy._latE6(), xy._lonE6(), w, h);
    }
    void rect(Geo xy1, Geo xy2) {
        rect(xy1._latE6(), xy1._lonE6(), xy2._latE6(), xy2._lonE6());
    }
    void rect(Geo xy, long w, long h, long r) {
        rect(xy._latE6(), xy._lonE6(), w, h, r);
    }
    void rect(Geo xy1, Geo xy2, long r) {
        rect(xy1._latE6(), xy1._lonE6(), xy2._latE6(), xy2._lonE6(), r);
    }
    void rect(Geo xy, long w, long h, long tl, long tr, long br, long bl) {
        rect(xy._latE6(), xy._lonE6(), w, h, tl, tr, br, bl);
    }
    void rect(Geo xy1, Geo xy2, long tl, long tr, long br, long bl) {
        rect(xy1._latE6(), xy1._lonE6(), xy2._latE6(), xy2._lonE6(), tl, tr, br, bl);
    }

    // квадрат
    // https://processing.org/reference/square_.html
    void square(long x, long y, long size) {
        rect(x, y, size, size);
    }
    void square(Geo xy, long size) {
        square(xy._latE6(), xy._lonE6(), size);
    }

    // дуга
    // https://processing.org/reference/arc_.html
    void arc(long x, long y, long w, long h, float start, float stop) {
        _cmd(api::arc, 6, x, y, w, h, (long)degrees(start), (long)degrees(stop));
    }
    void arc(Geo xy, long w, long h, float start, float stop) {
        arc(xy._latE6(), xy._lonE6(), w, h, start, stop);
    }

    // эллипс
    // https://processing.org/reference/ellipse_.html
    void ellipse(long x, long y, long w, long h) {
        _cmd(api::ellipse, 4, x, y, w, h);
    }
    void ellipse(Geo xy, long w, long h) {
        ellipse(xy._latE6(), xy._lonE6(), w, h);
    }

    // окружность
    // https://processing.org/reference/circle_.html
    void circle(long x, long y, long r) {
        _cmd(api::circle, 3, x, y, r);
    }
    void circle(Geo xy, long r) {
        circle(xy._latE6(), xy._lonE6(), r);
    }

    // кривая Безье
    // https://processing.org/reference/bezier_.html
    void bezier(long x1, long y1, long x2, long y2, long x3, long y3, long x4, long y4) {
        _cmd(api::bezier, 8, x1, y1, x2, y2, x3, y3, x4, y4);
    }
    void bezier(Geo xy1, Geo xy2, Geo xy3, Geo xy4) {
        bezier(xy1._latE6(), xy1._lonE6(), xy2._latE6(), xy2._lonE6(), xy3._latE6(), xy3._lonE6(), xy4._latE6(), xy4._lonE6());
    }

    // =================== SHAPE ===================
    // начать фигуру
    void beginShape() {
        _cmd(api::beginShape);
    }

    // закончить фигуру
    void endShape(bool close = 0) {
        _cmd(api::endShape, 1, close);
    }

    // следующая точка фигуры
    void vertex(long x, long y) {
        _cmd(api::vertex, 2, x, y);
    }
    void vertex(Geo xy) {
        vertex(xy._latE6(), xy._lonE6());
    }

    // следующая точка фигуры безье
    void bezierVertex(long xa1, long ya1, long xa2, long ya2, long xe, long ye) {
        _cmd(api::bezierVertex, 6, xa1, ya1, xa2, ya2, xe, ye);
    }
    void bezierVertex(Geo anch1, Geo anch2, Geo endp) {
        bezierVertex(anch1._latE6(), anch1._lonE6(), anch2._latE6(), anch2._lonE6(), endp._latE6(), endp._lonE6());
    }

    // =================== CANVAS ===================
    // вращать область рисования (в радианах)
    // https://processing.org/reference/rotate_.html
    void rotate(float rad) {
        _cmd(api::rotate, 1, (long)degrees(rad));
    }

    // перемещать область рисования
    // https://processing.org/reference/translate_.html
    void translate(long x, long y) {
        _cmd(api::translate, 2, x, y);
    }
    void translate(Geo xy) {
        translate(xy._latE6(), xy._lonE6());
    }

    // сохранить настройки рисования
    // https://processing.org/reference/push_.html
    void push() {
        _cmd(api::push);
    }

    // восстановить настройки рисования
    // https://processing.org/reference/pop_.html
    void pop() {
        _cmd(api::pop);
    }

    // =================== PRIVATE ===================
   private:
    void _api(api cmd) {
        _checkFirst();
        p->quotes();
        p->s += (uint8_t)cmd;
    }
    void _cmd(api cmd) {
        if (!p) return;
        _api(cmd);
        p->quotes();
        p->addChar(']');
    }
    void _cmd(api cmd, cv v) {
        _cmd(cmd, 1, (int)v);
    }
    void _cmd(api cmd, long n, ...) {
        if (!p) return;
        _api(cmd);
        p->colon();

        va_list args;
        va_start(args, n);
        _params(n, args);
        va_end(args);
        p->end();
        p->quotes();
        p->addChar(']');
    }
    void _cmdText(api cmd, GHTREF text, long n, ...) {
        if (!p) return;
        _api(cmd);
        p->colon();
        p->addText(text);

        va_list args;
        va_start(args, n);
        _params(n, args);
        va_end(args);
        p->end();
        p->quotes();
        p->addChar(']');
    }

    void _color(api cmd, const uint32_t& hex) {
        if (!p) return;
        _api(cmd);
        p->colon();
        p->s += '#';
        p->addIntRaw(su::Value(hex, HEX));
        p->quotes();
        p->addChar(']');
    }
    void _color(api cmd, const uint32_t& hex, const uint8_t& a) {
        _color(cmd, (hex << 8) | a);
    }
    void _color(api cmd, const uint8_t& r, const uint8_t& g, const uint8_t& b, const uint8_t& a) {
        union RGBA {
            uint8_t bytes[4];
            uint32_t hex;
        };
        _color(cmd, RGBA{a, b, g, r}.hex);
    }

    void _params(long n, va_list args) {
        for (int i = 0; i < n; i++) {
            p->addInt(su::Value(va_arg(args, long), HEX));
        }
    }
    void _checkFirst() {
        if (!p) return;
        if (_first) {
            _first = 0;
            p->beginArr(ghc::Tag::data);
        } else {
            p->replaceLast(',');
        }
    }

    ghc::Packet* p = nullptr;
    bool _first = 1;
};

}  // namespace gh