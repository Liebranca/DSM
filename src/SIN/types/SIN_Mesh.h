#ifndef __SIN_MESH_H__
#define __SIN_MESH_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../Zajec/lybyte/ZJC_BinTypes.h"

//  - --- - --- - --- - --- -

typedef struct SIN_MESH_3D                      {

    ushort  id;

    ushort  vertCount;
    ushort  indexCount;

    pVP3D_8*  bounds;
    VP3D_8*   verts;
    ushort* indices;

    ushort  matloc;
    ushort  users;

} M3D;

//  - --- - --- - --- - --- -

int     SIN_meshbucket_init ();
int     SIN_meshbucket_end  ();

void    unsub_mesh          (ushort loc);

M3D*    SIN_meshbucket_find (ushort id);
M3D*    SIN_meshbucket_get  (ushort loc);
M3D*    build_mesh          (ushort id, ushort matid, cchar* archive, uint offset);

//  - --- - --- - --- - --- -

#ifdef __cplusplus
}
#endif

#endif // __SIN_MESH_H__
