#include "ZJC_Hash.h"
#include "../lyutils/ZJC_Evil.h"

#include <stdio.h>

const uchar MAX_HASH_LENGTH = 16;

//  - --- - --- - --- - --- -

sHash* build_sHash(size_t size)                 {

    sHash* hash    = (sHash*) evil_malloc(1, sizeof(sHash));
    ulong locsize  = size*size;

    hash->size     = size;
    hash->maxloc   = locsize;

    hash->locs     = (ushort*) evil_malloc(locsize, sizeof(ushort));

    return hash;                                                                                                        }

void  del_sHash  (sHash* hash)                  { WARD_EVIL_MFREE(hash->locs); WARD_EVIL_MFREE(hash);                   }

//  - --- - --- - --- - --- -

ulong hashhex  (ushort key, ulong maxloc)       {

    if(key >= maxloc)                           { fprintf(stderr, "Key %u is over limit of %u locations for this table\n",
                                                  key, maxloc); return ERROR;                                           }

    return key + 3;                                                                                                     }

ulong  sh_hashit  (sHash* hash, ushort key)     {

    ulong loc = hashhex(key, hash->maxloc);
    if(loc < 3)                                 { return ERROR;                                                         }

    loc -= 3;
    loc  = loc % (hash->maxloc);
    return loc;                                                                                                         }

//  - --- - --- - --- - --- -

int    sh_insert  (sHash* hash,
                   ushort key,
                   ushort rloc)                 {

    ulong loc = sh_hashit(hash, key);
    hash->locs[loc] = rloc+1;

    return 0;                                                                                                           }

int   sh_pop     (sHash* hash, ushort key)      {

    ulong loc = sh_hashit(hash, key);
    hash->locs[loc] = 0;

    return 0;                                                                                                           }

//  - --- - --- - --- - --- -

ushort sh_hashloc (sHash* hash, ushort key)     {

    ulong loc   = sh_hashit(hash, key);
    if(hash->locs[loc] == 0)                    { return 0;                                                             }
    return hash->locs[loc];                                                                                             }

