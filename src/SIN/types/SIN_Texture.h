#ifndef __SIN_TEXTURE_H__
#define __SIN_TEXTURE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../Zajec/lybyte/ZJC_BinTypes.h"
#include "../rend/SIN_MateBlock.h"

//  - --- - --- - --- - --- -

typedef struct SIN_TEXTURE {

    uint   id;
    uint   users;

    ushort width;
    ushort height;

    uint   location;
    uint   imcount;

} Texture;

//  - --- - --- - --- - --- -

int      SIN_init_texbucket     (uint top_id);
int      SIN_end_texbucket      ();

int      SIN_fopen_texture      (cchar* filename);
int      SIN_fclose_texture     (cchar* filename);

Texture* SIN_build_texture      (uint texid, uint* shdid, uchar offset, SIN_MATDATA* matdata);

Texture* SIN_getItem_texbucket  (uint loc);
Texture* SIN_findItem_texbucket (uint id, int shutit);
uint     SIN_findLoc_texbucket  (uint id);

void     SIN_unsub_texture      (uint loc);
void     SIN_bind_texture       (uint loc, uint slot);

//  - --- - --- - --- - --- -

#ifdef __cplusplus
}
#endif

#endif // __SIN_TEXTURE_H__

