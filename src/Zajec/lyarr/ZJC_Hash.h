#ifndef __ZJC_HASH_H__
#define __ZJC_HASH_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "ZJC_DynArray.h"

//  - --- - --- - --- - --- -

typedef struct SHORT_HASH_TABLE {

    size_t    size;
    ulong     maxloc;
    usArray*  locs;

} sHash;

sHash* build_sHash(size_t size);
void   del_sHash  (sHash* hash);

ushort sh_hashloc (sHash* hash, ushort key);
ulong  sh_hashit  (sHash* hash, ushort key);
int    sh_insert  (sHash* hash, ushort key, ushort rloc);
int    sh_pop     (sHash* hash, ushort key);

//  - --- - --- - --- - --- -

#ifdef __cplusplus
}
#endif

#endif // __ZJC_HASH_H__
