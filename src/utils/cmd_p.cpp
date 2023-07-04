#include "cmd_p.h"

GH_PGM(_GH_CMD0, "focus");
GH_PGM(_GH_CMD1, "ping");
GH_PGM(_GH_CMD2, "unfocus");
GH_PGM(_GH_CMD3, "info");
GH_PGM(_GH_CMD4, "fsbr");
GH_PGM(_GH_CMD5, "format");
GH_PGM(_GH_CMD6, "reboot");
GH_PGM(_GH_CMD7, "data");
GH_PGM(_GH_CMD8, "set");
GH_PGM(_GH_CMD9, "cli");
GH_PGM(_GH_CMD10, "delete");
GH_PGM(_GH_CMD11, "rename");
GH_PGM(_GH_CMD12, "fetch");
GH_PGM(_GH_CMD13, "fetch_chunk");
GH_PGM(_GH_CMD14, "fetch_stop");
GH_PGM(_GH_CMD15, "upload");
GH_PGM(_GH_CMD16, "upload_chunk");
GH_PGM(_GH_CMD17, "ota");
GH_PGM(_GH_CMD18, "ota_chunk");
GH_PGM(_GH_CMD19, "ota_url");

#define GH_CMD_LEN 20
GH_PGM_LIST(_GH_cmd_list, _GH_CMD0, _GH_CMD1, _GH_CMD2, _GH_CMD3, _GH_CMD4, _GH_CMD5, _GH_CMD6, _GH_CMD7, _GH_CMD8, _GH_CMD9, _GH_CMD10, _GH_CMD11, _GH_CMD12, _GH_CMD13, _GH_CMD14, _GH_CMD15, _GH_CMD16, _GH_CMD17, _GH_CMD18, _GH_CMD19);

int GH_getCmd(const char* str) {
    for (int i = 0; i < GH_CMD_LEN; i++) {
#ifdef GH_ESP_BUILD
        if (!strcmp_P(str, _GH_cmd_list[i])) return i;
#else
        if (!strcmp_P(str, (PGM_P)pgm_read_word(_GH_cmd_list + i))) return i;
#endif
    }
    return -1;
}

/*
// ======================== CMD ========================
GH_PGM(_GH_CMD0, "focus");
GH_PGM(_GH_CMD1, "ping");
GH_PGM(_GH_CMD2, "unfocus");
GH_PGM(_GH_CMD3, "info");
GH_PGM(_GH_CMD4, "fsbr");
GH_PGM(_GH_CMD5, "format");
GH_PGM(_GH_CMD6, "reboot");
GH_PGM(_GH_CMD7, "fetch_chunk");
GH_PGM(_GH_CMD8, "fetch_stop");

#define GH_CMD_LEN 9
GH_PGM_LIST(_GH_cmd_list, _GH_CMD0, _GH_CMD1, _GH_CMD2, _GH_CMD3, _GH_CMD4, _GH_CMD5, _GH_CMD6, _GH_CMD7, _GH_CMD8);

int GH_getCmd(char* str) {
    for (int i = 0; i < GH_CMD_LEN; i++) {
        if (!strcmp_P(str, _GH_cmd_list[i])) return i;
    }
    return -1;
}


// ===================== CMD NAME ====================
GH_PGM(_GH_CMDN0, "set");
GH_PGM(_GH_CMDN1, "cli");
GH_PGM(_GH_CMDN2, "delete");
GH_PGM(_GH_CMDN3, "rename");
GH_PGM(_GH_CMDN4, "fetch");
GH_PGM(_GH_CMDN5, "upload");
GH_PGM(_GH_CMDN6, "upload_chunk");
GH_PGM(_GH_CMDN7, "ota");
GH_PGM(_GH_CMDN8, "ota_chunk");
GH_PGM(_GH_CMDN9, "ota_url");

#define GH_CMDN_LEN 10
GH_PGM_LIST(_GH_cmdN_list, _GH_CMDN0, _GH_CMDN1, _GH_CMDN2, _GH_CMDN3, _GH_CMDN4, _GH_CMDN5, _GH_CMDN6, _GH_CMDN7, _GH_CMDN8, _GH_CMDN9);

int GH_getCmdN(char* str) {
    for (int i = 0; i < GH_CMDN_LEN; i++) {
        if (!strcmp_P(str, _GH_cmdN_list[i])) return i;
    }
    return -1;
}
*/