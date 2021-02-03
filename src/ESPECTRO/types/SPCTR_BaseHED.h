#ifndef __SPCTR_BASEHED_H__
#define __SPCTR_BASEHED_H__

#include "ZJC_CommonTypes.h"
#include "SPCTR_BaseKLS.h"

//  ---     ---     ---     ---     ---

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ESPECTRO_HED {

    uchar* name;                                // Unique name for this block
    uchar* description;                         // Describe the block to the puny humans

    KLS    kls;                                 // Alias for 'class'; how to interpret value

    ushort size;                                // Number of elements for array-like values
    ushort ptr;                                 // Current position in array

    void*  value;                               // KLS-casted pointer to a value struct

} HED;

//  ---     ---     ---     ---     ---

/* LARGE TODO 

    i __think__ the best bet here is just use Zajec Containers (Zajec/lyarr/ZJC_Container)

    i'd just manage those entirely in C and pass HED handles to pyside as they're requested...
    ... lesser men would fear the memory managing there, but it's just a del hook. effynezy

    what bothers me is i'd have to potentially realloc often in-editor. which isn't so bad...
    ... long as i make darn sure to gobble enough RAM to hold a reasonable number of blocks

    what i know for sure is i dont want to expose those conts to python
    1) because they're automatons that should not be fucked with by the user
    2) because they're thick-assed structs made of thick-assed structs...

*/

//  ---     ---     ---     ---     ---

#ifdef __cplusplus
}
#endif

#endif // __SPCTR_BASEKLS_H__
