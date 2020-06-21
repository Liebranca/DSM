#include "ZJC_DynArray.h"

//  - --- - --- - --- - --- -

fArray build_fArray(uint32_t size)              {

    fArray farr;
    farr.size = size;
    farr.buff = (float*) evil_malloc(size, sizeof(float));

    return farr;                                                                                                        }

void   del_fArray(fArray* farr)                 { WARD_EVIL_MFREE(farr->buff);                                          }

//  - --- - --- - --- - --- -

usArray build_usArray(uint32_t size)            {

    usArray usarr;
    usarr.size = size;
    usarr.buff = (ushort*) evil_malloc(size, sizeof(ushort));

    return usarr;                                                                                                       }

void   del_usArray(usArray* usarr)              { WARD_EVIL_MFREE(usarr->buff);                                         }

