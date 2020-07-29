#include "ZJC_DynArray.h"

#include <stdio.h>

//  - --- - --- - --- - --- -

fArray* build_fArray(uint32_t size)             {

    fArray* farr = (fArray*) evil_malloc(1, sizeof(fArray));

    farr->size   = size;
    farr->buff   = (float*) evil_malloc(size, sizeof(float));

    return farr;                                                                                                        }

void   del_fArray(fArray* farr)                 { WARD_EVIL_MFREE(farr->buff); WARD_EVIL_MFREE(farr);                   }

//  - --- - --- - --- - --- -

usArray* build_usArray(uint32_t size)           {

    usArray* usarr = (usArray*) evil_malloc(1, sizeof(usArray));

    usarr->size = size;
    usarr->buff = (ushort*) evil_malloc(size, sizeof(ushort));

    return usarr;                                                                                                       }

void   del_usArray(usArray* usarr)              { WARD_EVIL_MFREE(usarr->buff); WARD_EVIL_MFREE(usarr);                 }

