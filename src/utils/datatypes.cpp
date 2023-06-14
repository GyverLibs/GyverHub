#include "datatypes.h"

void GHtypeFromStr(const char* str, void* val, GHdata_t type) {
    if (!val) return;
    switch (type) {
        case GH_STR:
            *(String*)val = str;
            break;
        case GH_CSTR:
            strcpy((char*)val, str);
            break;

        case GH_BOOL:
            *(bool*)val = (str[0] == '1');
            break;

        case GH_INT8:
            *(int8_t*)val = atoi(str);
            break;

        case GH_UINT8:
            *(uint8_t*)val = atoi(str);
            break;

        case GH_INT16:
            *(int16_t*)val = atoi(str);
            break;
        case GH_UINT16:
            *(uint16_t*)val = atoi(str);
            break;

        case GH_INT32:
            *(int32_t*)val = atol(str);
            break;
        case GH_UINT32:
            *(uint32_t*)val = atol(str);
            break;

        case GH_FLOAT:
            *(float*)val = atof(str);
            break;
        case GH_DOUBLE:
            *(double*)val = atof(str);
            break;

        case GH_COLOR:
            ((GHcolor*)val)->setHEX(atol(str));
            break;
        case GH_FLAGS:
            ((GHflags*)val)->flags = atoi(str);
            break;
        case GH_POS: {
            uint32_t xy = atol(str);
            ((GHpos*)val)->_changed = true;
            ((GHpos*)val)->x = ((uint16_t*)&xy)[1];
            ((GHpos*)val)->y = ((uint16_t*)&xy)[0];
        } break;

        case GH_NULL:
            break;
    }
}

void GHtypeToStr(String* s, void* val, GHdata_t type) {
    if (!val) {
        *s += '0';
        return;
    }
    switch (type) {
        case GH_STR:
            *s += *(String*)val;
            break;
        case GH_CSTR:
            *s += (char*)val;
            break;

        case GH_BOOL:
            *s += *(bool*)val;
            break;

        case GH_INT8:
            *s += *(int8_t*)val;
            break;

        case GH_UINT8:
            *s += *(uint8_t*)val;
            break;

        case GH_INT16:
            *s += *(int16_t*)val;
            break;
        case GH_UINT16:
            *s += *(uint16_t*)val;
            break;

        case GH_INT32:
            *s += *(int32_t*)val;
            break;
        case GH_UINT32:
            *s += *(uint32_t*)val;
            break;

        case GH_FLOAT:
            *s += *(float*)val;
            break;
        case GH_DOUBLE:
            *s += *(double*)val;
            break;

        case GH_COLOR:
            *s += ((GHcolor*)val)->getHEX();
            break;
        case GH_FLAGS:
            *s += ((GHflags*)val)->flags;
            break;
        case GH_POS:
            break;

        case GH_NULL:
            *s += '0';
            break;
    }
}