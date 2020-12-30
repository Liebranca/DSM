#include "ZJC_DynArray.h"

#include <stdio.h>

//  - --- - --- - --- - --- -

fArray* build_fArray(uint32_t size)             {

    fArray* farr = NULL;

    WARD_EVIL_MALLOC(farr,       fArray, sizeof(fArray), 1   );
    WARD_EVIL_MALLOC(farr->buff, float,  sizeof(float ), size);

    farr->size   = size;

    return farr;                                                                                                        }

void   del_fArray(fArray* farr)                 { WARD_EVIL_MFREE(farr->buff); WARD_EVIL_MFREE(farr);                   }

//  - --- - --- - --- - --- -

usArray* build_usArray(uint32_t size)           {

    usArray* usarr = NULL;

    WARD_EVIL_MALLOC(usarr,       usArray, sizeof(usArray), 1   );
    WARD_EVIL_MALLOC(usarr->buff, ushort,  sizeof(ushort ), size);

    usarr->size = size;

    return usarr;                                                                                                       }

void   del_usArray(usArray* usarr)              { WARD_EVIL_MFREE(usarr->buff); WARD_EVIL_MFREE(usarr);                 }

