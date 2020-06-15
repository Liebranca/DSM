#include "ZJC_DynArray.h"

fArray build_fArray(uint32_t size) {

    fArray farr;

    farr.size = size;
    farr.buff = (float*) evil_malloc(size, sizeof(float));
    return farr;
}

void   del_fArray(fArray* farr) { evil_free(&farr->buff); }

