#ifndef __SPCTR_BASEHED_H__
#define __SPCTR_BASEHED_H__

#include "ZJC_CommonTypes.h"
#include "SPCTR_BaseKLS.h"

//  ---     ---     ---     ---     ---

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ESPECTRO_HED {

    uchar* name;                                // Name for this block
    uchar* description;                         // Describe the block to the puny humans

    KLS    kls;                                 // Alias for 'class'; how to interpret value

    ushort size;                                // Number of elements for array-like values
    ushort ptr;                                 // Current position in array

    void*  value;                               // KLS-casted pointer to a value struct

} HED;

//  ---     ---     ---     ---     ---

#ifdef __cplusplus
}
#endif

#endif // __SPCTR_BASEKLS_H__
