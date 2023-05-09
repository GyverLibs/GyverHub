#pragma once
#include <Arduino.h>

#include "stats.h"

struct GHhub {
    GHhub() {}
    GHhub(GHconn_t nconn, const char* nid) {
        conn = nconn;
        if (strlen(nid) <= 8) strcpy(id, nid);
    }

    // тип соединения
    GHconn_t conn = GH_SYSTEM;

    // id клиента
    char id[9] = {'\0'};

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