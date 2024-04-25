#pragma once
#include "hub_macro.hpp"

#define GH_ROW(b, w, code) \
    do {                   \
        b.beginRow(w);     \
        code;              \
        b.endRow();        \
    } while (0);

#define GH_COL(b, w, code) \
    do {                   \
        b.beginCol(w);     \
        code;              \
        b.endCol();        \
    } while (0);

#define GH_BUILD_VAL(_fname, _wtype)                                         \
    ghc::Widget& _fname(GHVREF text = GHVAL()) {                             \
        return _varAndType(_wtype, GHTXT(), &text);                          \
    }                                                                        \
    ghc::Widget& _fname##_(GHTREF name, GHVREF text = GHVAL()) {             \
        return _varAndType(_wtype, name, &text);                             \
    }                                                                        \
    ghc::Widget& _fname##_(GHTREF name, void* pairs) {                       \
        return _varAndType(_wtype, name, ghc::AnyPtr(pairs, Type::PAIRS_T)); \
    }

#define GH_BUILD_VAR(_fname, _wtype)                                       \
    ghc::Widget& _fname(ghc::AnyPtr ptr = ghc::AnyPtr()) {                 \
        return _varAndType(_wtype, GHTXT(), ptr);                          \
    }                                                                      \
    ghc::Widget& _fname##_(GHTREF name, ghc::AnyPtr ptr = ghc::AnyPtr()) { \
        return _varAndType(_wtype, name, ptr);                             \
    }

#define GH_BUILD_VAR_TYPE(_fname, _type, _wtype)               \
    ghc::Widget& _fname(_type ptr = nullptr) {                 \
        return _varAndType(_wtype, GHTXT(), ptr);              \
    }                                                          \
    ghc::Widget& _fname##_(GHTREF name, _type ptr = nullptr) { \
        return _varAndType(_wtype, name, ptr);                 \
    }

#define GH_BUILD_CONTAINER(_fname, _wtype, _rowcol)                                                                                                               \
    bool begin##_fname(uint16_t width = 0, GHTREF label = GHTXT(), gh::Color color = gh::Colors::Default, uint8_t fontsize = GH_DEF_CONT_FSIZE) {                 \
        return _beginContainer(GHTXT(), _wtype, GHTXT(), _rowcol, width, label, color, fontsize);                                                                 \
    }                                                                                                                                                             \
    bool begin##_fname##_(GHTREF name, uint16_t width = 0, GHTREF label = GHTXT(), gh::Color color = gh::Colors::Default, uint8_t fontsize = GH_DEF_CONT_FSIZE) { \
        return _beginContainer(name, _wtype, GHTXT(), _rowcol, width, label, color, fontsize);                                                                    \
    }                                                                                                                                                             \
    void end##_fname() {                                                                                                                                          \
        endContainer();                                                                                                                                           \
    }                                                                                                                                                             \
    bool next##_fname##_(GHTREF name, uint16_t width = 0, GHTREF label = GHTXT(), gh::Color color = gh::Colors::Default, uint8_t fontsize = GH_DEF_CONT_FSIZE) {  \
        endContainer();                                                                                                                                           \
        return _beginContainer(name, _wtype, GHTXT(), _rowcol, width, label, color, fontsize);                                                                    \
    }                                                                                                                                                             \
    bool next##_fname(uint16_t width = 0, GHTREF label = GHTXT(), gh::Color color = gh::Colors::Default, uint8_t fontsize = GH_DEF_CONT_FSIZE) {                  \
        endContainer();                                                                                                                                           \
        return _beginContainer(GHTXT(), _wtype, GHTXT(), _rowcol, width, label, color, fontsize);                                                                 \
    }

#define GH_BUILD_CUSTOM_CONTAINER(_fname, _rowcol)                                                                                                                              \
    bool begin##_fname##_(GHTREF name, GHTREF wtype, uint16_t width = 0, GHTREF label = GHTXT(), gh::Color color = gh::Colors::Default, uint8_t fontsize = GH_DEF_CONT_FSIZE) { \
        return _beginContainer(name, ghc::Tag::none, wtype, _rowcol, width, label, color, fontsize);                                                                            \
    }                                                                                                                                                                           \
    bool begin##_fname(GHTREF wtype, uint16_t width = 0, GHTREF label = GHTXT(), gh::Color color = gh::Colors::Default, uint8_t fontsize = GH_DEF_CONT_FSIZE) {                 \
        return _beginContainer(GHTXT(), ghc::Tag::none, wtype, _rowcol, width, label, color, fontsize);                                                                         \
    }                                                                                                                                                                           \
    bool next##_fname##_(GHTREF name, GHTREF wtype, uint16_t width = 0, GHTREF label = GHTXT(), gh::Color color = gh::Colors::Default, uint8_t fontsize = GH_DEF_CONT_FSIZE) {  \
        return _nextContainer(name, ghc::Tag::none, wtype, _rowcol, width, label, color, fontsize);                                                                             \
    }                                                                                                                                                                           \
    bool next##_fname(GHTREF wtype, uint16_t width = 0, GHTREF label = GHTXT(), gh::Color color = gh::Colors::Default, uint8_t fontsize = GH_DEF_CONT_FSIZE) {                  \
        return _nextContainer(GHTXT(), ghc::Tag::none, wtype, _rowcol, width, label, color, fontsize);                                                                          \
    }

#define GH_BUILD_CONTAINER_CLASS(_fname)                                                                                                                                     \
    class _fname##_ {                                                                                                                                                        \
       public:                                                                                                                                                               \
        _fname##_(GHTREF name, Builder& b, uint16_t width = 0, GHTREF label = GHTXT(), gh::Color color = gh::Colors::Default, uint8_t fontsize = GH_DEF_CONT_FSIZE) : b(b) { \
            b.begin##_fname##_(name, width, label, color, fontsize);                                                                                                         \
        }                                                                                                                                                                    \
        ~_fname##_() {                                                                                                                                                       \
            b.end##_fname();                                                                                                                                                 \
        }                                                                                                                                                                    \
                                                                                                                                                                             \
       private:                                                                                                                                                              \
        Builder& b;                                                                                                                                                          \
    };                                                                                                                                                                       \
    class _fname {                                                                                                                                                           \
       public:                                                                                                                                                               \
        _fname(Builder& b, uint16_t width = 0, GHTREF label = GHTXT(), gh::Color color = gh::Colors::Default, uint8_t fontsize = GH_DEF_CONT_FSIZE) : b(b) {                 \
            b.begin##_fname(width, label, color, fontsize);                                                                                                                  \
        }                                                                                                                                                                    \
        ~_fname() {                                                                                                                                                          \
            b.end##_fname();                                                                                                                                                 \
        }                                                                                                                                                                    \
                                                                                                                                                                             \
       private:                                                                                                                                                              \
        Builder& b;                                                                                                                                                          \
    };

#define _GH_NEWLINE_DUMMY