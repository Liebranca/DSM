#include "SIN_Mesh.h"

#include "lyarr/ZJC_Hash.h"
#include "lyarr/ZJC_Stack.h"
#include "lybyte/ZJC_FileHandling.h"
#include "lyutils/ZJC_Evil.h"

#include "SIN_Material.h"
#include "SIN_MeshBatch.h"

#include "GL/glew.h"

#include <stdio.h>
#include <math.h>

#define SIN_MAX_MESHES              512

static M3D     SIN_emptymesh      = {0};

static ushort  SIN_ACTIVE_MESHES  = 0;

static DAF*    MESH_ARCHIVE       = NULL;

static sStack* SIN_MESH_SLOTSTACK = NULL;
static sHash*  SIN_MEHASH         = NULL;

static M3D*    SIN_meshbucket     = NULL;

typedef struct TRI_NORMAL_GROUP {

    float face_normals[3];
    uint  len;

} TNG;

//  - --- - --- - --- - --- -

int SIN_meshbucket_init ()                      {

    MESH_ARCHIVE       = dafalloc();
    SIN_meshbucket     = (M3D*) evil_malloc(SIN_MAX_MESHES, sizeof(M3D));

    SIN_MEHASH         = build_sHash (SIN_MAX_MESHES);
    SIN_MESH_SLOTSTACK = build_sStack(SIN_MAX_MESHES);

    for(int i = SIN_MAX_MESHES-1; i > 0; i--)   { sStack_push(SIN_MESH_SLOTSTACK, i);                                   }

    return 0;                                                                                                           }

int SIN_meshbucket_end  ()                      {

    del_sStack(SIN_MESH_SLOTSTACK);
    del_sHash (SIN_MEHASH);

    for(uint i = 0;
        i < SIN_MAX_MESHES; i++)                { M3D* mesh = SIN_meshbucket + i;

                                                  if(mesh->id)
                                                { WARD_EVIL_MFREE(mesh->bounds); }                                      }

    WARD_EVIL_MFREE(SIN_meshbucket);
    WARD_EVIL_MFREE(MESH_ARCHIVE);

    return 0;                                                                                                           }

//  - --- - --- - --- - --- -

int     SIN_mesh_extract_from(cchar* filename)  { return extraction_start(filename, 0, &MESH_ARCHIVE);                  }

int     SIN_mesh_extract_end (cchar* filename)  { return extraction_end(filename, &MESH_ARCHIVE);                       }

//  - --- - --- - --- - --- -

M3D* SIN_meshbucket_find (ushort id)            {

    ushort loc = sh_hashloc(SIN_MEHASH, id);
    if(loc == 0)                                { return NULL;                                                          }

    return SIN_meshbucket+(loc-1);                                                                                      }

ushort SIN_meshbucket_findloc  (ushort id)      {

    ushort loc = sh_hashloc(SIN_MEHASH, id);
    if(loc == 0)                                { fprintf(stderr, "Mesh %u not found\n", id);
                                                  return 0;                                                             }

    return loc-1;                                                                                                       }

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
                             uchar  offset)     {

    M3D* mesh = SIN_meshbucket_find(id);

    if(mesh == NULL)
    {
        if(SIN_ACTIVE_MESHES
        == SIN_MAX_MESHES)                      { fprintf(stderr, "Cannot create more than %u 3D meshes",
                                                  SIN_MAX_MESHES); return NULL;                                         }

        uint loc = sStack_pop(SIN_MESH_SLOTSTACK);
        WARD_EVIL_UNSIG(loc, 1);

        sh_insert(SIN_MEHASH, id, loc);

        mesh            = SIN_meshbucket+loc;

        mesh->id        = id;
        mesh->matloc    = SIN_matbucket_findloc(matid);

        VP3D_8* verts   = NULL;
        ushort* indices = NULL;

        extractcrk(MESH_ARCHIVE,
                   offset,
                   &mesh->vertCount,
                   &mesh->indexCount,
                   &mesh->bounds,
                   &verts,
                   &indices);

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

void    draw_mesh             (M3D*   mesh)     { glDrawElements(GL_TRIANGLES, mesh->indexCount * 3, GL_UNSIGNED_SHORT,
                                                                 (void*) (mesh->drawOffset * sizeof(uint)) );           }

void     del_mesh            (M3D*   mesh,
                              ushort loc)       {

    WARD_EVIL_MFREE(mesh->bounds);
    unsub_material(mesh->matloc);

    SIN_meshbucket[loc] = SIN_emptymesh;
    int memward = sStack_push(SIN_MESH_SLOTSTACK, loc);
    WARD_EVIL_UNSIG(memward, 1);

    SIN_ACTIVE_MESHES--;                                                                                                }

void  unsub_mesh       (ushort loc)             {

    M3D* mesh = SIN_meshbucket_get(loc);

    if(mesh)
    {
        mesh->users--;
        if(mesh->users == 0)                    { sh_pop(SIN_MEHASH, mesh->id); del_mesh(mesh, loc);                    }
    }

                                                                                                                        }

//  - --- - --- - --- - --- -

