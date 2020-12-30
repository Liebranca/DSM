#include "SIN_Material.h"

#include "lyarr/ZJC_Container.h"
#include "lyutils/ZJC_Evil.h"

#include "SIN_Texture.h"
#include "SIN_Shader.h"

#include <stdio.h>

static Material   SIN_emptymat          = {0};
static uint       SIN_ACTIVE_MATERIALS  = 0;

static uint       SIN_OPAQUE_MATERIALS  = 0;

static Container* SIN_matbucket;

//  - --- - --- - --- - --- -

uint SIN_getMax_material()                      { return SIN_MAX_MATERIALS;                                             }

int SIN_init_matbucket(uint top_id)             {

    SIN_matbucket = ZJC_build_cont              (SIN_MAX_MATERIALS, sizeof(Material), top_id, "Material"                );
    return 0;                                                                                                           }

int SIN_end_matbucket()                         { ZJC_del_cont(SIN_matbucket); return 0;                                }

//  - --- - --- - --- - --- -

uint SIN_getActiveCount_material()              { return SIN_ACTIVE_MATERIALS;                                          }
uint SIN_getOpaqueCount_material()              { return SIN_OPAQUE_MATERIALS;                                          }

//  - --- - --- - --- - --- -

Material* SIN_findItem_matbucket(uint id,
                                 int  shutit)   {        ZJC_findItem_cont(SIN_matbucket, Material, id, shutit );       }

Material* SIN_getItem_matbucket (uint loc)      {        ZJC_getItem_cont (SIN_matbucket, Material, loc);               }
uint      SIN_findLoc_matbucket (uint id )      { return ZJC_findLoc_cont (SIN_matbucket, id,       0  ) - 1;           }

//  - --- - --- - --- - --- -

Material* SIN_build_material(uint  matid,
                             uint  texid,
                             uchar texoffset)    {

    Material* material = SIN_findItem_matbucket(matid, 1);

    if(material == NULL)
    {
        if(SIN_ACTIVE_MATERIALS
        == SIN_MAX_MATERIALS)                   { fprintf(stderr, "Cannot create more than %u materials\n",
                                                  SIN_MAX_MATERIALS); return NULL;                                      }

        uint loc             = ZJC_push_cont  (SIN_matbucket, matid);
                               WARD_EVIL_UNSIG(loc,           1    );

        material             = ((Material*) SIN_matbucket->buff) + loc;
        material->id         = matid;

        ushort*      shdid   = NULL;
        SIN_MATDATA* matdata = SIN_MateBlock_getSlot(loc);

        //SIN_buildTexture(texid, &shdid, texoffset, matdata);

        material->texloc  = SIN_texbucket_findloc(texid);

        if(matdata->flags & 2)                  { SIN_OPAQUE_MATERIALS++;                                               }

        // material->shdloc  = SIN_shdbucket_findloc(shdid);

        SIN_ACTIVE_MATERIALS++;

        Program* program  = SIN_shdbucket_get(material->shdloc);
        program->users++;

        Texture* tex      = SIN_texbucket_get(material->texloc);
        tex->users++;

    }

    return material;                                                                                                    }

//  - --- - --- - --- - --- -

int SIN_getIsOpaque_material(uint loc)          { return (SIN_MateBlock_getSlot(loc)->flags & 2);                       }

//  - --- - --- - --- - --- -

void SIN_del_material(Material* material,
                      uint      loc)            {

    if(SIN_getMaterialIsOpaque(loc))            { SIN_OPAQUE_MATERIALS--;                                               }

    SIN_unsubShader (material->shdloc);
    SIN_unsubTexture(material->texloc);

    *material = SIN_emptymat;
    SIN_ACTIVE_MATERIALS--;                                                                                             }

void SIN_unsub_material(uint loc)               {

    Material* material = SIN_getItem_matbucket(loc);

    if(material)
    {
        material->users--;
        if(material->users == 0)                { if( ZJC_pop_cont    (SIN_matbucket, material->id)  )
                                                {     SIN_del_material(material,      loc         ); }                  }
    }

                                                                                                                        }

//  - --- - --- - --- - --- -
