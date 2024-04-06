#pragma once
#include <Arduino.h>
#include <GSON.h>

#include "client.h"
#include "hooks.h"
#include "hub_macro.hpp"
#include "tags.h"

namespace ghc {

class Packet : public gson::string {
   public:
    Packet(uint16_t res = 0,
           gh::Client* client = nullptr,
           uint16_t bufsize = 0) : _client(client),
                                   _bufsize(bufsize) {
        if (bufsize && bufsize < res) reserve(bufsize);
        else if (res) reserve(res);
    }

    void beginPacket(uint32_t id = 0, gh::Client* client = nullptr) {
        s = F("#{");
        addID(id);
        addClient(client);
    }

    void endPacket() {
        end();
        s += F("}#");
    }

    void addID(uint32_t id) {
        if (id) addString(Tag::id, su::Value(id, HEX));
    }
    void addClient(gh::Client* client) {
        if (client && client->id) addString(Tag::client, su::Value(client->id, HEX));
    }

    bool endsWith(char sym) {
        return s.length() ? (s.charAt(s.length() - 1) == sym) : 0;
    }

    // + tag
    using gson::string::addBool;
    using gson::string::addFloat;
    using gson::string::addInt;
    using gson::string::addKey;
    using gson::string::addString;
    using gson::string::addStringEsc;
    using gson::string::beginArr;
    using gson::string::beginObj;

    void replaceLast(char c) {
        s.setCharAt(s.length() - 1, c);
    }
    void addChar(char c) {
        s += c;
    }
    void addKey(const Tag& tag) {
        addTag(tag);
        colon();
    }
    void beginArr(const Tag& tag) {
        addKey(tag);
        beginArr();
    }
    void beginObj(const Tag& tag) {
        addKey(tag);
        beginObj();
    }
    void addString(const Tag& key, const Tag& val) {
        addKey(key);
        addTag(val);
        comma();
    }
    void addString(const Tag& key, GHTREF text) {
        if (!text.valid()) return;
        addKey(key);
        addString(text);
    }
    void addStringEsc(const Tag& key, GHTREF text) {
        if (!text.valid()) return;
        addKey(key);
        addStringEsc(text);
    }

    void addBool(const Tag& key, const bool& value) {
        addKey(key);
        addBool(value);
    }

    void addInt(const Tag& key, const su::Value& value) {
        addKey(key);
        value.addString(s);
        comma();
    }

    void addFloat(const Tag& key, const double& value, uint8_t dec = 2) {
        addKey(key);
        addFloat(value, dec);
    }

    // send
    void send() {
        if (_client) {
            _client->send(s);
            clear();
        }
    }

    void sendText(GHTREF text) {
        if (_client) {
            send();
            _client->send(text);
        }
    }

    //    private:
    gh::Client* _client;
    uint16_t _bufsize;

    void afterValue() {
        if (_bufsize && length() >= _bufsize) send();
    }
    char add16(uint8_t i) {
        i &= 0xf;
        return (char)(i >= 10 ? (i + 87) : (i + 48));
    }
    void addTag(Tag tag) {
        uint8_t t = (uint8_t)tag;
        s += '#';
        if (t >= 16) s += add16(t >> 4);
        s += add16(t);
    }
    void escape(const su::Text& text) {
        if (text.indexOf('\"') >= 0) {
            uint16_t len = text.length();
            char p = 0;
            for (uint16_t i = 0; i < len; i++) {
                char c = text.charAt(i);
                if (c == '\"' && p != '\\') s += '\\';
                s += c;
                p = c;
            }
        } else {
            text.addString(s);
        }
    }
};

}  // namespace ghc