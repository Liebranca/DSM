#ifndef __SIN_TEXTURE_H__
#define __SIN_TEXTURE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../Zajec/lybyte/ZJC_BinTypes.h"
#include "../rend/SIN_MateBlock.h"

//  - --- - --- - --- - --- -

typedef struct SIN_TEXTURE {

    ushort id;
    ushort users;

    ushort width;
    ushort height;

    uint   location;
    uchar  imcount;

} Texture;

//  - --- - --- - --- - --- -

int      SIN_texbucket_init    ();
int      SIN_texbucket_end     ();

int      SIN_tex_extract_from  (cchar* filename);
int      SIN_tex_extract_end   (cchar* filename);

Texture* SIN_buildTexture      (ushort idbase, ushort* shdid, uchar offset, SIN_MATDATA* matdata);

Texture* SIN_texbucket_get     (ushort loc);
Texture* SIN_texbucket_find    (ushort id);
ushort   SIN_texbucket_findloc (ushort id);

void     SIN_unsubTexture      (ushort loc);
void     SIN_bindTexture       (ushort loc, uint slot);

//  - --- - --- - --- - --- -

#ifdef __cplusplus
}
#endif

#endif // __SIN_TEXTURE_H__

