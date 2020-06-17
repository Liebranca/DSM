#ifndef __SIN_MESH_H__
#define __SIN_MESH_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../Zajec/lybyte/ZJC_BinTypes.h"

//  - --- - --- - --- - --- -

typedef struct SIN_MESH_3D                      {

    cchar*  id;

    ushort  vertCount;
    ushort  indexCount;

    pVP3D*  bounds;
    VP3D*   verts;
    ushort* indices;

    ushort  matloc;
    ushort  users;

} M3D;
typedef struct SIN_MESH_2D                      {

    cchar*  id;

    ushort  vertCount;
    ushort  indexCount;

    pVP2D*  bounds;
    VP2D*   verts;
    ushort* indices;

    uint    matloc;
    uint    users;

} M2D;

//  - --- - --- - --- - --- -

int     SIN_meshbucket_init3d  ();
int     SIN_meshbucket_end3d    ();

ushort  unsub_mesh_3d          (ushort loc);

M3D*    SIN_meshbucket_find3d  (cchar* id);
M3D*    SIN_meshbucket_get3d   (ushort loc);

M3D*    build_mesh_3d          (cchar* id, cchar* matid, cchar* archive, uint offset);

//  - --- - --- - --- - --- -

int     SIN_meshbucket_end2d    ();
int     SIN_meshbucket_init2d  ();

M2D*    SIN_meshbucket_find2d  (cchar* id);
M2D*    SIN_meshbucket_get2d   (ushort loc);

M2D*    build_mesh_2d          (cchar* id, cchar* matid, cchar* archive, uint offset);

#ifdef __cplusplus
}
#endif

#endif // __SIN_MESH_H__
