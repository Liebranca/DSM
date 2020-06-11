#include "ZJC_CommonTypes.h"
#include <string>

extern "C" {
    namespace zjc {
        long hexstr_tolong(char* v)                         { return strtol(v, NULL, 0);                                    }
    }
}
