#include "SIN_Mesh.h"

#include "lybyte/ZJC_FileHandling.h"
#include "lyutils/ZJC_Evil.h"

#include <stdio.h>

#include "SIN_Material.h"

M3D*    SIN_meshbucket_3d = NULL;
M3D     SIN_emptymesh_3d  = {0};

M2D*    SIN_meshbucket_2d = NULL;
M2D     SIN_emptymesh_2d  = {0};

cushort SIN_MAX_MESHES3D    = 256;
cushort SIN_MAX_MESHES2D    = 256;
ushort  SIN_ACTIVE_MESHES3D = 0;
ushort  SIN_ACTIVE_MESHES2D = 0;

DAF*    CURRENT_ARCHIVE     = {0};

//  - --- - --- - --- - --- -

int SIN_meshbucket_init3d ()                    {

    SIN_meshbucket_3d = (M3D*) evil_malloc(SIN_MAX_MESHES3D, sizeof(M3D));
    return 0;                                                                                                           }

int SIN_meshbucket_end3d  ()                    {

    for(uint i = 0;
        i < SIN_ACTIVE_MESHES3D; i++)           { M3D* mesh = SIN_meshbucket_3d + i;
                                                  WARD_EVIL_MFREE(mesh->bounds);
                                                  WARD_EVIL_MFREE(mesh->verts);
                                                  WARD_EVIL_MFREE(mesh->indices);                                       }

    WARD_EVIL_MFREE(SIN_meshbucket_3d);
    return 0;                                                                                                           }

//  - --- - --- - --- - --- -

M3D* SIN_meshbucket_find3d (cchar* id)          {

    for(uint i = 0;
        i < SIN_ACTIVE_MESHES3D; i++)           { if((SIN_meshbucket_3d+i)->id == id) { return SIN_meshbucket_3d+i; }   }

    fprintf(stderr, "3D mesh %s not found\n", id);
    return NULL;                                                                                                        }

M3D* SIN_meshbucket_get3d (ushort loc)          {

    if(loc < SIN_ACTIVE_MESHES3D)               { return SIN_meshbucket_3d+loc;                                         }

    fprintf(stderr, "No 3D mesh at location %u\n", loc);
    return NULL;                                                                                                        }

//  - --- - --- - --- - --- -

M3D*    build_mesh_3d       (cchar* id,
                             cchar* matid,
                             cchar* archive,
                             uint   offset)     {

    M3D* mesh = SIN_meshbucket_find3d(matid);

    if(mesh == NULL)
    {
        if(SIN_ACTIVE_MESHES3D
        == SIN_MAX_MESHES3D)                    { fprintf(stderr, "Cannot create more than %u 3D meshes",
                                                  SIN_MAX_MESHES3D); return NULL;                               }

        mesh = 0;
        mesh->id     = id;
        mesh->matloc = SIN_matbucket_getloc(matid);

        extractcrk(CURRENT_ARCHIVE,
                   offset,
                   &mesh->vertCount,
                   &mesh->indexCount,
                   mesh->bounds,
                   mesh->verts,
                   mesh->indices);

        *(SIN_meshbucket_3d+SIN_ACTIVE_MESHES3D) = *mesh;
        SIN_ACTIVE_MESHES3D++;
    }

    Material* material = SIN_matbucket_get(mesh->matloc);
    material->users++;

    return mesh;                                                                                                        }

//  - --- - --- - --- - --- -

int     del_mesh_3d         (M3D*   mesh,
                             ushort loc)        {

    if(SIN_ACTIVE_MESHES3D == 0)                { fprintf(stderr,
                                                  "3D mesh bucket is empty! There is no %s to delete\n", mesh->id);
                                                  return ERROR;                                                         }

    WARD_EVIL_MFREE(mesh->bounds);
    WARD_EVIL_MFREE(mesh->verts);
    WARD_EVIL_MFREE(mesh->indices);

    *(SIN_meshbucket_3d+loc) = SIN_emptymesh_3d;

    for(uint i = loc;
    i < SIN_ACTIVE_MESHES3D; i++)               { *(SIN_meshbucket_3d+i) = *(SIN_meshbucket_3d+i+1);                     }

    SIN_ACTIVE_MESHES3D--;

    return 0;                                                                                                            }

ushort  unsub_mesh_3d       (ushort loc)        {

    M3D* mesh = SIN_meshbucket_get3d(loc);
    if(mesh)
    {
        mesh->users--;
        if(mesh->users == 0)                    { del_mesh_3d(mesh, loc);                                               }

        return loc;
    }

    fprintf(stderr, "No 3D mesh at location %u\n", loc);
    return ERROR;                                                                                                       }

//  - --- - --- - --- - --- -

int SIN_meshbucket_init2d ()                    {

    SIN_meshbucket_2d = (M2D*) evil_malloc(SIN_MAX_MESHES2D, sizeof(M2D));
    return 0;                                                                                                           }

int SIN_meshbucket_end2d  ()                    {

    WARD_EVIL_MFREE(SIN_meshbucket_2d);
    return 0;                                                                                                           }

//  - --- - --- - --- - --- -

M2D* SIN_meshbucket_find2d (cchar* id)          {

    for(uint i = 0;
        i < SIN_ACTIVE_MESHES2D; i++)           { if((SIN_meshbucket_2d+i)->id == id) { return SIN_meshbucket_2d+i; }   }

    fprintf(stderr, "2D mesh %s not found\n", id);
    return NULL;                                                                                                        }

M2D* SIN_meshbucket_get2d (ushort loc)          {

    if(loc < SIN_ACTIVE_MESHES2D)               { return SIN_meshbucket_2d+loc;                                         }

    fprintf(stderr, "No 2D mesh at location %u\n", loc);
    return NULL;                                                                                                        }

