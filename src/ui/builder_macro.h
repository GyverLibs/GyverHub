#pragma once

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

#define GH_BUILD_VAL(_fname, _tag)                                         \
    ghc::Widget& _fname(GHVREF text = GHVAL()) {                           \
        return _varAndType(_tag, GHTXT(), &text);                          \
    }                                                                      \
    ghc::Widget& _fname##_(GHTREF name, GHVREF text = GHVAL()) {           \
        return _varAndType(_tag, name, &text);                             \
    }                                                                      \
    ghc::Widget& _fname##_(GHTREF name, void* pairs) {                     \
        return _varAndType(_tag, name, ghc::AnyPtr(pairs, Type::PAIRS_T)); \
    }

#define GH_BUILD_VAL_NONAME(_fname, _tag)         \
    ghc::Widget& _fname(GHVREF text = GHVAL()) {  \
        return _varAndType(_tag, GHTXT(), &text); \
    }

#define GH_BUILD_VAR(_fname, _tag)                                         \
    ghc::Widget& _fname(ghc::AnyPtr ptr = ghc::AnyPtr()) {                 \
        return _varAndType(_tag, GHTXT(), ptr);                            \
    }                                                                      \
    ghc::Widget& _fname##_(GHTREF name, ghc::AnyPtr ptr = ghc::AnyPtr()) { \
        return _varAndType(_tag, name, ptr);                               \
    }

#define GH_BUILD_VAR_TYPE(_fname, _type, _tag)                 \
    ghc::Widget& _fname(_type ptr = nullptr) {                 \
        return _varAndType(_tag, GHTXT(), ptr);                \
    }                                                          \
    ghc::Widget& _fname##_(GHTREF name, _type ptr = nullptr) { \
        return _varAndType(_tag, name, ptr);                   \
    }

#define GH_BUILD_CUSTOM(_fname, _tag)                                         \
    ghc::Widget& _fname(GHTREF func, ghc::AnyPtr ptr) {                       \
        return _varAndType(_tag, GHTXT(), ptr, func);                         \
    }                                                                         \
    ghc::Widget& _fname(GHTREF func, GHVREF text = GHVAL()) {                 \
        return _varAndType(_tag, GHTXT(), &text, func);                       \
    }                                                                         \
    ghc::Widget& _fname##_(GHTREF name, GHTREF func, ghc::AnyPtr ptr) {       \
        return _varAndType(_tag, name, ptr, func);                            \
    }                                                                         \
    ghc::Widget& _fname##_(GHTREF name, GHTREF func, GHVREF text = GHVAL()) { \
        return _varAndType(_tag, name, &text, func);                          \
    }

#define GH_BUILD_CONTAINER(_fname, _tag)     \
    bool begin##_fname(uint16_t width = 1) { \
        return _beginContainer(width, _tag); \
    }                                        \
    void end##_fname() {                     \
        _endContainer();                     \
    }                                        \
    bool next##_fname(uint16_t width) {      \
        _endContainer();                     \
        return _beginContainer(width, _tag); \
    }

#define _GH_NEWLINE_DUMMY