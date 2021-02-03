#ifndef __SPCTR_BASEKLS_H__
#define __SPCTR_BASEKLS_H__

#include "ZJC_CommonTypes.h"
#include "../ESPECTRO.h"

//  ---     ---     ---     ---     ---

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ESPECTRO_KLS {

    ushort arr_idex;
    ushort itm_idex;

} KLS;

//  ---     ---     ---     ---     ---

// Get size of ValueType array
EXPORT ushort SPCTR_getsize_valType(ushort idex);

// Shift KLS by (-/+) steps across ValueType
EXPORT cuchar* SPCTR_moveptr_KLS(KLS* kls, sint steps);

// Get current (char) value within ValueType array
EXPORT cuchar* SPCTR_getcurr_KLS(KLS* kls);

//  ---     ---     ---     ---     ---

#ifdef __cplusplus
}
#endif

#endif // __SPCTR_BASEKLS_H__


