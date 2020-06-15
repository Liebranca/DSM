#ifndef __ZJC_DYNARRAY_H__
#define __ZJC_DYNARRAY_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../lyutils/ZJC_Evil.h"
#include "../ZJC_CommonTypes.h"

#include <stdint.h>

//  - --- - --- - --- - --- -

typedef struct float_dynarray               {

    uint32_t    size;
    float*      buff;

} fArray;

fArray build_fArray(uint32_t size);
void   del_fArray(fArray* farr);

//  - --- - --- - --- - --- -

#ifdef __cplusplus
}
#endif

#endif // __ZJC_DYNARRAY_H__
