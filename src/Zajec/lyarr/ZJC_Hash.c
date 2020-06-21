#include "ZJC_Hash.h"

#include <stdio.h>

const uchar MAX_HASH_LENGTH = 16;

//  - --- - --- - --- - --- -

sHash build_sHash(size_t size)                  {

    sHash hash = {0};
    ulong locsize = size*size;

    hash.size   = size;
    hash.maxloc = locsize;
    hash.locs   = build_usArray(locsize);

    for(uint i  = 0; i < locsize; i++)          { hash.locs.buff[i]  = 0;                                               }

    return hash;                                                                                                        }

void  del_sHash  (sHash* hash)                  { del_usArray     (&hash->locs);                                        }

//  - --- - --- - --- - --- -

ulong hashhex  (ushort key, ulong maxloc)       {

    if(key >= maxloc)                           { fprintf(stderr, "Key %s is over limit of %u locations for this table\n",
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
    hash->locs.buff[loc] = rloc+1;

    return 0;                                                                                                           }

int   sh_pop     (sHash* hash, ushort key)      {

    ulong loc = sh_hashit(hash, key);
    hash->locs.buff[loc] = 0;

    return 0;                                                                                                           }

//  - --- - --- - --- - --- -

ushort sh_hashloc (sHash* hash, ushort key)      {

    ulong loc   = sh_hashit(hash, key);
    if(hash->locs.buff[loc] == 0)                { return 0;                                                            }
    return hash->locs.buff[loc];                                                                                        }

