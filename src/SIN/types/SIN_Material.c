#include "SIN_Material.h"

#include "lyarr/ZJC_Container.h"
#include "lyutils/ZJC_Evil.h"

#include "SIN_Texture.h"
#include "SIN_Shader.h"

#include <stdio.h>

static Material   SIN_emptymat          = {0};
static ushort     SIN_ACTIVE_MATERIALS  = 0;

static ushort     SIN_OPAQUE_MATERIALS  = 0;

static Container* SIN_matbucket;

//  - --- - --- - --- - --- -

uint SIN_getMaxMaterials()                      { return SIN_MAX_MATERIALS;                                             }

int SIN_matbucket_init()                        {

    SIN_matbucket = ZJC_buildcont(SIN_MAX_MATERIALS, 64, sizeof(Material), "Material");
    return 0;                                                                                                           }

int SIN_matbucket_end()                         { ZJC_delcont(SIN_matbucket); return 0;                                 }

//  - --- - --- - --- - --- -

uint SIN_getActiveMatCount()                    { return SIN_ACTIVE_MATERIALS;                                          }
uint SIN_getOpaqueMatCount()                    { return SIN_OPAQUE_MATERIALS;                                          }

//  - --- - --- - --- - --- -

Material* SIN_matbucket_find(uint matid)        { ZJC_cont_find(SIN_matbucket, Material, matid);                        }
Material* SIN_matbucket_get(uint loc)           { ZJC_cont_get(SIN_matbucket, Material, loc);                           }

uint SIN_matbucket_findloc(uint matid)          { return ZJC_cont_findLoc(SIN_matbucket, matid);                        }

//  - --- - --- - --- - --- -

Material* SIN_buildMaterial(uint  matid,
                            uint  texid,
                            uchar texoffset)    {

    Material* material = SIN_matbucket_find(matid);

    if(material == NULL)
    {
        if(SIN_ACTIVE_MATERIALS
        == SIN_MAX_MATERIALS)                   { fprintf(stderr, "Cannot create more than %u materials\n",
                                                  SIN_MAX_MATERIALS); return NULL;                                      }

        uint loc = ZJC_pushcont(SIN_matbucket, matid);

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

int SIN_getMaterialIsOpaque(uint loc)           { return (SIN_MateBlock_getSlot(loc)->flags & 2);                       }

//  - --- - --- - --- - --- -

void del_material(Material* material,
                  uint      loc)                {

    if(SIN_getMaterialIsOpaque(loc))            { SIN_OPAQUE_MATERIALS--;                                               }

    SIN_unsubShader (material->shdloc);
    SIN_unsubTexture(material->texloc);

    /*SIN_matbucket[loc] = SIN_emptymat;
    int memward = sStack_push(SIN_MAT_SLOTSTACK, loc);
    WARD_EVIL_UNSIG(memward, 1);*/

    SIN_ACTIVE_MATERIALS--;                                                                                             }

void SIN_unsubMaterial(uint loc)                {

    Material* material = SIN_matbucket_get(loc);

    if(material)
    {
        material->users--;
        if(material->users == 0)                { /*sh_pop(SIN_MATHASH, loc);*/ del_material(material, loc);                }
    }

                                                                                                                        }

//  - --- - --- - --- - --- -
