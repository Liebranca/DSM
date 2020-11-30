#ifndef __ZJC_HASH_H__
#define __ZJC_HASH_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../ZJC_CommonTypes.h"
#include "ZJC_LinkList.h"

//  - --- - --- - --- - --- -

typedef struct ZJC_HASHTABLE {

    uint  size;
    uint  xfac;
    uint  max_value;

    LND** table;

} HASH;

//  - --- - --- - --- - --- -

HASH* ZJC_build_hash        (uint size, uint max_value      );
void  ZJC_del_hash          (HASH* hash                     );

void  ZJC_push_hash         (HASH* hash, uint id, uint data );
uint  ZJC_pop_hash          (HASH* hash, uint id            );
uint  ZJC_find_hash         (HASH* hash, uint id            );

//  - --- - --- - --- - --- -

#ifdef __cplusplus
}
#endif

#endif // __ZJC_HASH_H__
