#pragma once
#include <Arduino.h>

#include "stats.h"

struct GHclient {
    GHclient() {}
    GHclient(GHconn_t nfrom, const char* nid, GHsource_t nsource = GH_ESP) {
        from = nfrom;
        if (strlen(nid) <= 8) strcpy(id, nid);
        source = nsource;
    }

    // тип соединения
    GHconn_t from = GH_SYSTEM;

    // id клиента
    char id[9] = {'\0'};

    // id как String
    String idString() {
        return id;
    }
    
    // источник
    GHsource_t source;

    bool eq(GHclient& client) {
        return (client.from == from && !strcmp(client.id, id));
    }
    bool operator==(GHclient& client) {
        return eq(client);
    }
    bool operator!=(GHclient& client) {
        return !eq(client);
    }
};