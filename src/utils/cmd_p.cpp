#include "cmd_p.h"

// ======================== CMD ========================
GH_PGM(_GH_CMD0, "focus");
GH_PGM(_GH_CMD1, "ping");
GH_PGM(_GH_CMD2, "unfocus");
#ifdef GH_ESP_BUILD
GH_PGM(_GH_CMD3, "info");
GH_PGM(_GH_CMD4, "fsbr");
GH_PGM(_GH_CMD5, "reboot");
GH_PGM(_GH_CMD6, "fetch_chunk");
#endif

#ifdef GH_ESP_BUILD
#define GH_CMD_LEN 7
GH_PGM_LIST(_GH_cmd_list, _GH_CMD0, _GH_CMD1, _GH_CMD2, _GH_CMD3, _GH_CMD4, _GH_CMD5, _GH_CMD6);
#else
#define GH_CMD_LEN 3
GH_PGM_LIST(_GH_cmd_list, _GH_CMD0, _GH_CMD1, _GH_CMD2);
#endif

int GH_getCmd(char* str) {
    for (int i = 0; i < GH_CMD_LEN; i++) {
        if (!strcmp_P(str, _GH_cmd_list[i])) return i;
    }
    return -1;
}

// ===================== CMD NAME ====================
GH_PGM(_GH_CMDN0, "set");
GH_PGM(_GH_CMDN1, "click");
GH_PGM(_GH_CMDN2, "cli");
#ifdef GH_ESP_BUILD
GH_PGM(_GH_CMDN3, "delete");
GH_PGM(_GH_CMDN4, "rename");
GH_PGM(_GH_CMDN5, "fetch");
GH_PGM(_GH_CMDN6, "upload");
GH_PGM(_GH_CMDN7, "upload_chunk");
GH_PGM(_GH_CMDN8, "ota");
GH_PGM(_GH_CMDN9, "ota_chunk");
GH_PGM(_GH_CMDN10, "ota_url");
#endif

#ifdef GH_ESP_BUILD
#define GH_CMDN_LEN 11
GH_PGM_LIST(_GH_cmdN_list, _GH_CMDN0, _GH_CMDN1, _GH_CMDN2, _GH_CMDN3, _GH_CMDN4, _GH_CMDN5, _GH_CMDN6, _GH_CMDN7, _GH_CMDN8, _GH_CMDN9, _GH_CMDN10);
#else
#define GH_CMDN_LEN 3
GH_PGM_LIST(_GH_cmdN_list, _GH_CMDN0, _GH_CMDN1, _GH_CMDN2);
#endif

int GH_getCmdN(char* str) {
    for (int i = 0; i < GH_CMDN_LEN; i++) {
        if (!strcmp_P(str, _GH_cmdN_list[i])) return i;
    }
    return -1;
}