#pragma once
#include <Arduino.h>

#include "stats.h"

struct GHhub {
    GHhub() {}
    GHhub(GHconn_t nconn, const char* nid, bool nmanual) {
        conn = nconn;
        if (strlen(nid) <= 8) strcpy(id, nid);
        manual = nmanual;
    }

    // тип соединения
    GHconn_t conn = GH_SYSTEM;

    // id клиента
    char id[9] = {'\0'};
    
    // из ручного парсера
    bool manual = false;

    bool eq(GHhub& hub) {
        return (hub.conn == conn && !strcmp(hub.id, id));
    }
    bool operator==(GHhub& hub) {
        return eq(hub);
    }
    bool operator!=(GHhub& hub) {
        return !eq(hub);
    }
};