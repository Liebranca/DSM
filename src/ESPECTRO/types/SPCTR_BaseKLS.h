#ifndef __SPCTR_BASEKLS_H__
#define __SPCTR_BASEKLS_H__

#include "ZJC_CommonTypes.h"

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
ushort      SPCTR_getsize_valType       (ushort idex                );

// Shift KLS by (-/+) steps across ValueType
cuchar*     SPCTR_moveptr_KLS           (KLS*   kls, int steps      );

//  ---     ---     ---     ---     ---

#ifdef __cplusplus
}
#endif

#endif // __SPCTR_BASEKLS_H__


