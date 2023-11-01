#include "datatypes.h"

void GHtypeFromStr(const char* str, void* var, GHdata_t type) {
    if (!var) return;
    switch (type) {
        case GH_STR:
            *(String*)var = str;
            break;
        case GH_CSTR:
            strcpy((char*)var, str);
            break;

        case GH_BOOL:
            *(bool*)var = (str[0] == '1');
            break;

        case GH_INT8:
            *(int8_t*)var = atoi(str);
            break;

        case GH_UINT8:
            *(uint8_t*)var = atoi(str);
            break;

        case GH_INT16:
            *(int16_t*)var = atoi(str);
            break;
        case GH_UINT16:
            *(uint16_t*)var = atoi(str);
            break;

        case GH_INT32:
            *(int32_t*)var = atol(str);
            break;
        case GH_UINT32:
            *(uint32_t*)var = atol(str);
            break;

        case GH_FLOAT:
            *(float*)var = atof(str);
            break;
        case GH_DOUBLE:
            *(double*)var = atof(str);
            break;

        case GH_COLOR:
            ((GHcolor*)var)->setHEX(atol(str));
            break;
        case GH_FLAGS:
            ((GHflags*)var)->flags = atoi(str);
            break;
        case GH_POS: {
            uint32_t xy = atol(str);
            ((GHpos*)var)->_changed = true;
            ((GHpos*)var)->x = xy >> 16;
            ((GHpos*)var)->y = xy & 0xffff;
        } break;

        case GH_NULL:
            break;
    }
}

void GHtypeToStr(String* s, void* var, GHdata_t type) {
    if (!var) {
        *s += '0';
        return;
    }
    switch (type) {
        case GH_STR:
            //*s += *(String*)var;
            GH_addEsc(s, ((String*)var)->c_str());
            break;
        case GH_CSTR:
            //*s += (char*)var;
            GH_addEsc(s, var);
            break;

        case GH_BOOL:
            *s += *(bool*)var;
            break;

        case GH_INT8:
            *s += *(int8_t*)var;
            break;

        case GH_UINT8:
            *s += *(uint8_t*)var;
            break;

        case GH_INT16:
            *s += *(int16_t*)var;
            break;
        case GH_UINT16:
            *s += *(uint16_t*)var;
            break;

        case GH_INT32:
            *s += *(int32_t*)var;
            break;
        case GH_UINT32:
            *s += *(uint32_t*)var;
            break;

        case GH_FLOAT:
            *s += *(float*)var;
            break;
        case GH_DOUBLE:
            *s += *(double*)var;
            break;

        case GH_COLOR:
            *s += ((GHcolor*)var)->getHEX();
            break;
        case GH_FLAGS:
            *s += ((GHflags*)var)->flags;
            break;
        case GH_POS:
            break;

        case GH_NULL:
            *s += '0';
            break;
    }
}