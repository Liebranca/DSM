#ifndef ZAJEC_DYNARRAY_H
#define ZAJEC_DYNARRAY_H

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

#endif // ZAJEC_DYNARRAY_H
