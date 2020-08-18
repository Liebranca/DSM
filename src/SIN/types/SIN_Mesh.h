#ifndef __SIN_MESH_H__
#define __SIN_MESH_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../Zajec/lybyte/ZJC_BinTypes.h"

//  - --- - --- - --- - --- -

typedef struct SIN_MESH_3D                      {

    ushort   id;

    ushort   vertCount;
    ushort   indexCount;

    uint     drawLoc;
    uint     drawOffset;
    uint     buffOffset;

    pVP3D_8* bounds;

    ushort   matloc;
    ushort   users;

} M3D;

//  - --- - --- - --- - --- -

int     SIN_mesh_extract_from (cchar* filename);
int     SIN_mesh_extract_end  (cchar* filename);

int     SIN_meshbucket_init   ();
int     SIN_meshbucket_end    ();

void    unsub_mesh            (ushort loc);

M3D*    SIN_meshbucket_find   (ushort id);
ushort SIN_meshbucket_findloc (ushort id);
M3D*    SIN_meshbucket_get    (ushort loc);
M3D*    build_mesh            (ushort id, ushort matid, uchar offset);

void    draw_mesh             (M3D*   mesh);

//  - --- - --- - --- - --- -

#ifdef __cplusplus
}
#endif

#endif // __SIN_MESH_H__
