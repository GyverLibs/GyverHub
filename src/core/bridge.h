#pragma once
#include <Arduino.h>

#include "bridge_class.h"
#include "hooks.h"
#include "hub_macro.hpp"
#include "types.h"

#define GH_CONN_TOUT 3500
#define GH_BR_PARSE 0
#define GH_BR_SEND 1
#define GH_BR_ANSWER 2

namespace gh {

struct BridgeData {
    BridgeData(GHTREF text) : text(text) {}

    GHTREF text;
    String topic;
    bool broadcast = 0;
};

class Bridge {
   public:
    Bridge() {}
    Bridge(void* hub, Connection conn, ghc::ParseHook parse) {
        config(hub, conn, parse);
    }

    // настроить
    void config(void* hub, Connection conn, ghc::ParseHook parse) {
        _hub = hub;
        connection = conn;
        _parse = parse;
    }

    virtual void begin(){};
    virtual void end(){};
    virtual void tick(){};
    virtual void send(BridgeData& data){};

    // парсить url=data или url + data отдельно
    void parse(GHTXT url, GHTXT data = GHTXT()) {
        if (!canParse() || !url.valid()) return;
        if (!data.valid()) {
            int16_t eq = url.indexOf('=');
            if (eq > 0) {
                data = url.substring(eq + 1);
                url = url.substring(0, eq);
            }
        }
        _inParse = 1;
        _parse(_hub, *this, url, data);
        _inParse = 0;
    }

    // установить режим (приём, передача, ответ)
    void setMode(bool parse, bool send, bool answer) {
        bitWrite(_mode, GH_BR_PARSE, parse);
        bitWrite(_mode, GH_BR_SEND, send);
        bitWrite(_mode, GH_BR_ANSWER, answer);
    }

    // может принимать
    bool canParse() {
        return bitRead(_mode, GH_BR_PARSE);
    }

    // может отправлять
    bool canSend() {
        return isAnswer() ? bitRead(_mode, GH_BR_ANSWER) : bitRead(_mode, GH_BR_SEND);
    }

    // true если сейчас идёт парсинг (это ответ приложению)
    bool isAnswer() {
        return _inParse;
    }

    // тип подключения
    Connection connection = Connection::System;

    // получить порт
    uint16_t getPort() {
        return _port;
    }

    // установить порт
    void setPort(uint16_t port) {
        _port = port;
    }

    // установить focus
    void setFocus() {
        _focus_tmr = millis();
    }

    // снять focus
    void clearFocus() {
        _focus_tmr = millis() - GH_CONN_TOUT;
    }

    // получить focus
    bool getFocus() {
        return (millis() - _focus_tmr < GH_CONN_TOUT);
    }

    // http transfer
    bool hasTransfer() {
        return _transfer;
    }

    // http transfer
    void setTransfer(bool transfer) {
        _transfer = transfer;
    }

   protected:
    void* _hub;
    ghc::ParseHook _parse;
    uint16_t _port = 0;
    bool _transfer = 0;

   private:
    uint32_t _focus_tmr = 0;
    uint8_t _mode = 0b111;
    bool _inParse = 0;
};

}  // namespace gh