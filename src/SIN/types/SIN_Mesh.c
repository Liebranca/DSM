#include "SIN_Mesh.h"

#include "lyarr/ZJC_Hash.h"
#include "lyarr/ZJC_Stack.h"
#include "lybyte/ZJC_FileHandling.h"
#include "lyutils/ZJC_Evil.h"

#include "SIN_Material.h"

#include <stdio.h>

#define SIN_MAX_MESHES      256

M3D*   SIN_meshbucket     = NULL;
M3D    SIN_emptymesh      = {0};

ushort SIN_ACTIVE_MESHES  = 0;

DAF*   CURRENT_ARCHIVE    = {0};

sStack SIN_MESH_SLOTSTACK = {0};
sHash  SIN_MEHASH         = {0};

//  - --- - --- - --- - --- -

int SIN_meshbucket_init ()                      {

    SIN_MEHASH     = build_sHash(SIN_MAX_MESHES);

    SIN_MESH_SLOTSTACK = build_sStack(SIN_MAX_MESHES);
    for(int i = SIN_MAX_MESHES-1; i > -1; i--)  { sStack_push(&SIN_MESH_SLOTSTACK, i);                                  }

    SIN_meshbucket = (M3D*) evil_malloc(SIN_MAX_MESHES, sizeof(M3D));

    return 0;                                                                                                           }

int SIN_meshbucket_end  ()                      {

    del_sHash(&SIN_MEHASH);
    del_sStack(&SIN_MESH_SLOTSTACK);

    for(uint i = 0;
        i < SIN_MAX_MESHES; i++)                { M3D* mesh = SIN_meshbucket + i;

                                                  if(mesh != NULL) 
                                                { WARD_EVIL_MFREE(mesh->bounds);
                                                  WARD_EVIL_MFREE(mesh->verts);
                                                  WARD_EVIL_MFREE(mesh->indices); }                                     }

    WARD_EVIL_MFREE(SIN_meshbucket);
    return 0;                                                                                                           }

//  - --- - --- - --- - --- -

M3D* SIN_meshbucket_find (ushort id)            {

    ushort loc = sh_hashloc(&SIN_MEHASH, id);
    if(loc == 0)                                { return NULL;                                                          }

    return SIN_meshbucket+(loc-1);                                                                                      }

M3D* SIN_meshbucket_get  (ushort loc)           {

    if(loc < SIN_MAX_MESHES)
    { 
        if(SIN_meshbucket+loc)                  { return SIN_meshbucket+loc;                                            }

        printf("Location %u points to an empty mesh slot\n", loc);
        return NULL;
    }

    printf("No mesh at location %u\n", loc);
    return NULL;                                                                                                        }

//  - --- - --- - --- - --- -

M3D*    build_mesh          (ushort id,
                             ushort matid,
                             cchar* archive,
                             uint   offset)     {

    M3D* mesh = SIN_meshbucket_find(matid);

    if(mesh == NULL)
    {
        if(SIN_ACTIVE_MESHES
        == SIN_MAX_MESHES)                      { fprintf(stderr, "Cannot create more than %u 3D meshes",
                                                  SIN_MAX_MESHES); return NULL;                                         }

        mesh = 0;
        mesh->id     = id;
        mesh->matloc = SIN_matbucket_findloc(matid);

        extractcrk(CURRENT_ARCHIVE,
                   offset,
                   &mesh->vertCount,
                   &mesh->indexCount,
                   mesh->bounds,
                   mesh->verts,
                   mesh->indices);

        uint loc = sStack_pop(&SIN_MESH_SLOTSTACK);
        WARD_EVIL_UNSIG(loc, 1);

        sh_insert(&SIN_MEHASH, matid, loc);

        SIN_meshbucket[loc] = *mesh;
        SIN_ACTIVE_MESHES++;

        Material* material = SIN_matbucket_get(mesh->matloc);
        material->users++;

    }

    return mesh;                                                                                                        }

//  - --- - --- - --- - --- -

void     del_mesh            (M3D*   mesh,
                              ushort loc)       {

    WARD_EVIL_MFREE(mesh->bounds);
    WARD_EVIL_MFREE(mesh->verts);
    WARD_EVIL_MFREE(mesh->indices);

    unsub_material(mesh->matloc);

    SIN_meshbucket[loc] = SIN_emptymesh;
    int memward = sStack_push(&SIN_MESH_SLOTSTACK, loc);
    WARD_EVIL_UNSIG(memward, 1);

    SIN_ACTIVE_MESHES--;                                                                                                }

void  unsub_mesh       (ushort loc)             {

    M3D* mesh = SIN_meshbucket_get(loc);

    if(mesh)
    {
        mesh->users--;
        if(mesh->users == 0)                    { sh_pop(&SIN_MEHASH, mesh->id); del_mesh(mesh, loc);                   }
    }

                                                                                                                        }

//  - --- - --- - --- - --- -

