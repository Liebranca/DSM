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

int     SIN_fopen_mesh          (cchar* filename);
int     SIN_fclose_mesh         (cchar* filename);

int     SIN_init_meshbucket     (uint top_id);
int     SIN_end_meshbucket      ();

void    SIN_unsub_mesh          (uint loc);

M3D*    SIN_findItem_meshbucket (uint id);
uint    SIN_findLoc_meshbucket  (uint id);
M3D*    SIN_getItem_meshbucket  (uint loc);
M3D*    SIN_build_mesh          (uint id, uchar offset);

void    SIN_draw_mesh           (M3D*   mesh);

//  - --- - --- - --- - --- -

#ifdef __cplusplus
}
#endif

#endif // __SIN_MESH_H__
