#include "SIN_Mesh.h"

#include "lyarr/ZJC_Container.h"
#include "lybyte/ZJC_FileHandling.h"
#include "lyutils/ZJC_Evil.h"

#include "SIN_Material.h"
#include "SIN_MeshBatch.h"

#include "GL/glew.h"

#include <stdio.h>
#include <math.h>

#define           SIN_MAX_MESHES       512

static M3D        SIN_emptymesh      = {0};

static uint       SIN_ACTIVE_MESHES  = 0;

static DAF*       MESH_ARCHIVE       = NULL;

static Container* SIN_meshbucket     = NULL;

//  - --- - --- - --- - --- -

int SIN_init_meshbucket (uint top_id)           {

    SIN_meshbucket = ZJC_build_cont             (SIN_MAX_MESHES, sizeof(M3D), top_id, "Mesh"                            );
    return 0;                                                                                                           }

int SIN_end_meshbucket ()                       {

    for(uint i = 0;
        i < SIN_MAX_MESHES; i++)                { M3D* mesh = ((M3D*) SIN_meshbucket->buff) + i;

                                                  if(mesh->id)
                                                { WARD_EVIL_MFREE(mesh->bounds); }                                      }

    ZJC_del_cont(SIN_meshbucket);

    return 0;                                                                                                           }

//  - --- - --- - --- - --- -

int SIN_fopen_mesh (cchar* filename)            { return extraction_start(filename, 0, &MESH_ARCHIVE);                  }

int SIN_fclose_mesh(cchar* filename)            { return extraction_end(filename, &MESH_ARCHIVE);                       }

//  - --- - --- - --- - --- -

M3D* SIN_findItem_meshbucket(uint id,
                             int shutit)        {        ZJC_findItem_cont(SIN_meshbucket, M3D, id, shutit);            }

M3D* SIN_getItem_meshbucket (uint loc)          {        ZJC_getItem_cont (SIN_meshbucket, M3D, loc       );            }
uint SIN_findLoc_meshbucket (uint id )          { return ZJC_findLoc_cont (SIN_meshbucket, id,  0         ) - 1;        }

//  - --- - --- - --- - --- -

M3D*    SIN_build_mesh       (uint  id,
                              uchar offset)     {

    M3D* mesh = SIN_findItem_meshbucket(id, 0);

    if(mesh == NULL)
    {
        if(SIN_ACTIVE_MESHES
        == SIN_MAX_MESHES)                      { fprintf(stderr, "Cannot create more than %u 3D meshes",
                                                  SIN_MAX_MESHES); return NULL;                                         }

        uint loc        = ZJC_push_cont  (SIN_meshbucket, id);
                          WARD_EVIL_UNSIG(loc,           1  );

        mesh            = ((M3D*) SIN_meshbucket->buff) + loc;

        mesh->id        = id;

        VP3D_8* verts   = NULL;
        ushort* indices = NULL;
        ushort  matid   = 0;

        extractcrk(MESH_ARCHIVE,
                   offset,
                   &matid,
                   &mesh->vertCount,
                   &mesh->indexCount,
                   &mesh->bounds,
                   &verts,
                   &indices);

        mesh->matloc    = SIN_matbucket_findloc(matid);

//  - --- - --- - --- - --- -

        float* vertex_data = (float*) evil_malloc(mesh->vertCount * 14, sizeof(float));

        for(uint i = 0, j = 0; i < mesh->vertCount; i++, j += 14)
        {

            vertex_data[j+0 ] = frac_tofloat((verts + i)->co[0]);
            vertex_data[j+1 ] = frac_tofloat((verts + i)->co[1]);
            vertex_data[j+2 ] = frac_tofloat((verts + i)->co[2]);

            vertex_data[j+3 ] = frac_tofloat((verts + i)->normal[0]);
            vertex_data[j+4 ] = frac_tofloat((verts + i)->normal[1]);
            vertex_data[j+5 ] = frac_tofloat((verts + i)->normal[2]);

            vertex_data[j+6 ] = frac_tofloat((verts + i)->tangent[0]);
            vertex_data[j+7 ] = frac_tofloat((verts + i)->tangent[1]);
            vertex_data[j+8 ] = frac_tofloat((verts + i)->tangent[2]);

            vertex_data[j+9 ] = frac_tofloat((verts + i)->bitangent[0]);
            vertex_data[j+10] = frac_tofloat((verts + i)->bitangent[1]);
            vertex_data[j+11] = frac_tofloat((verts + i)->bitangent[2]);

            vertex_data[j+12] = ufrac_tofloat((verts + i)->uv[0]);
            vertex_data[j+13] = ufrac_tofloat((verts + i)->uv[1]);
        }

//  - --- - --- - --- - --- -

        mesh->drawLoc    = SIN_active_meshbatch->loc;
        mesh->drawOffset = SIN_active_meshbatch->icursor;
        mesh->buffOffset = SIN_active_meshbatch->vcursor;

        SIN_meshbatch_upload(mesh->vertCount, mesh->indexCount, vertex_data, indices);

        WARD_EVIL_MFREE(vertex_data);
        WARD_EVIL_MFREE(verts);
        WARD_EVIL_MFREE(indices);

        SIN_ACTIVE_MESHES++;

        Material* material = SIN_matbucket_get(mesh->matloc);
        material->users++;

    }

    return mesh;                                                                                                        }

//  - --- - --- - --- - --- -

void SIN_draw_mesh(M3D*   mesh)                 { glDrawElements( GL_TRIANGLES, mesh->indexCount * 3, GL_UNSIGNED_SHORT,
                                                                 (void*) (mesh->drawOffset));                           }

void SIN_del_mesh(M3D* mesh,
                  uint loc)                     {

    WARD_EVIL_MFREE   (mesh->bounds);
    SIN_unsub_material(mesh->matloc);

    *mesh = SIN_emptymesh;
    SIN_ACTIVE_MESHES--;                                                                                                }

void  SIN_unsub_mesh(uint loc)                  {

    M3D* mesh = SIN_meshbucket_get(loc);

    if(mesh)
    {
        mesh->users--;
        if(mesh->users == 0)                    { if( ZJC_pop_cont(SIN_meshbucket, mesh->id)  )
                                                {     SIN_del_mesh(mesh,           loc     ); }                         }
    }

                                                                                                                        }

//  - --- - --- - --- - --- -

