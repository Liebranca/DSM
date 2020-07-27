#ifndef __ZJC_DYNARRAY_H__
#define __ZJC_DYNARRAY_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../lyutils/ZJC_Evil.h"
#include "../ZJC_CommonTypes.h"

#include <stdint.h>

//  - --- - --- - --- - --- -

typedef struct FLOAT_DYNARRAY               {

    uint32_t    size;
    float*      buff;

} fArray;

fArray* build_fArray(uint32_t size);
void    del_fArray(fArray* farr);

//  - --- - --- - --- - --- -

typedef struct USHORT_DYNARRAY              {

    uint32_t    size;
    ushort*     buff;

} usArray;

//  - --- - --- - --- - --- -

usArray* build_usArray(uint32_t size);
void     del_usArray(usArray* usarr);

#ifdef __cplusplus
}
#endif

#endif // __ZJC_DYNARRAY_H__
