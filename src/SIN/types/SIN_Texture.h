#ifndef __SIN_TEXTURE_H__
#define __SIN_TEXTURE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../Zajec/lybyte/ZJC_BinTypes.h"

//  - --- - --- - --- - --- -

typedef struct SIN_TEXTURE {

    ushort id;
    ushort users;

    ushort width;
    ushort height;

    uint   location;

} Texture;

//  - --- - --- - --- - --- -

int      SIN_texbucket_init    ();
int      SIN_texbucket_end     ();

int      SIN_tex_extract_from  (cchar* filename);
int      SIN_tex_extract_end   (cchar* filename);

Texture* build_texture         (ushort id, uchar offset, int isNormalMap);

Texture* SIN_texbucket_get     (ushort loc);
Texture* SIN_texbucket_find    (ushort id);
ushort   SIN_texbucket_findloc (ushort id);

void     unsub_tex             (ushort loc);
void     bind_tex_to_slot      (ushort loc, uint slot);

//  - --- - --- - --- - --- -

#ifdef __cplusplus
}
#endif

#endif // __SIN_TEXTURE_H__

