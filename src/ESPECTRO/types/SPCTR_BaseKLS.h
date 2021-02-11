#ifndef __SPCTR_BASEKLS_H__
#define __SPCTR_BASEKLS_H__

#include "ZJC_CommonTypes.h"
#include "lyutils/ZJC_Str.h"

#include "SPCTR_Numerics.h"

//  ---     ---     ---     ---     ---

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ESPECTRO_KLS {

    ushort arr_idex;
    ushort itm_idex;

} KLS;

//  ---     ---     ---     ---     ---
// generic type-casting macros.

#define KLS_CAST_INT(X) (IBAR*) X

#define KLS_CAST(TYPE, X) CAT(KLS_CAST_, TYPE)##(X)

#define KLS_FINDTYPE(kls, v) {  switch kls->arr_idex:                                                                    \
                                case 0:                                                                                  \
                                    switch kls->itm_idex:                                                                \
                                    case 0: KLS_CAST(INT, v)\
                             }

//  ---     ---     ---     ---     ---

// Get size of ValueType array
ushort SPCTR_getsize_valType(ushort idex);

// Shift KLS by (-/+) steps across ValueType
cuchar* SPCTR_moveptr_KLS(KLS* kls, sint steps);

// Set KLS to idex
cuchar* SPCTR_setptr_KLS(KLS* kls, sint idex);

// Get current (char) value within ValueType array
cuchar* SPCTR_getcurr_KLS(KLS* kls);

//  ---     ---     ---     ---     ---

#ifdef __cplusplus
}
#endif

#endif // __SPCTR_BASEKLS_H__


