#include "SIN_Material.h"

#include "lyarr/ZJC_Hash.h"
#include "lyarr/ZJC_Stack.h"
#include "lyutils/ZJC_Evil.h"

#include "SIN_Texture.h"
#include "SIN_Shader.h"

#include <stdio.h>

#define   SIN_MAX_MATERIALS             64

static Material SIN_emptymat          = {0};
static ushort   SIN_ACTIVE_MATERIALS  = 0;

static sStack*  SIN_MAT_SLOTSTACK     = NULL;
static sHash*   SIN_MATHASH           = NULL;

static Material SIN_matbucket[SIN_MAX_MATERIALS];

//  - --- - --- - --- - --- -

int       SIN_matbucket_init  ()                {

    SIN_MATHASH       = build_sHash (SIN_MAX_MATERIALS);
    SIN_MAT_SLOTSTACK = build_sStack(SIN_MAX_MATERIALS);

    for(int i = SIN_MAX_MATERIALS-1;
        i > 0; i--)                             { sStack_push(SIN_MAT_SLOTSTACK, i);                                    }

    return 0;                                                                                                           }

int       SIN_matbucket_end   ()                {

    del_sStack(SIN_MAT_SLOTSTACK);
    del_sHash (SIN_MATHASH);

    return 0;                                                                                                           }

//  - --- - --- - --- - --- -

Material* SIN_matbucket_find  (ushort matid)    {

    ushort loc = sh_hashloc(SIN_MATHASH, matid);
    if(loc == 0)                                { return NULL;                                                          }

    return SIN_matbucket+(loc-1);                                                                                       }

ushort SIN_matbucket_findloc  (ushort matid)    {

    ushort loc = sh_hashloc(SIN_MATHASH, matid);
    if(loc == 0)                                { fprintf(stderr, "Material %u not found\n", matid);
                                                  return 0;                                                             }

    return loc-1;                                                                                                       }

Material* SIN_matbucket_get      (ushort loc)   {

    if(loc < SIN_MAX_MATERIALS)
    { 
        if(SIN_matbucket+loc)                   { return SIN_matbucket+loc;                                             }

        printf("Location %u points to an empty material slot\n", loc);
        return NULL;
    }

    printf("No material at location %u\n", loc);
    return NULL;                                                                                                        }

//  - --- - --- - --- - --- -

Material* build_material      (ushort matid,
                               ushort texid[3],
                               ushort shdid)    {

    Material* material = SIN_matbucket_find(matid);

    if(material == NULL)
    {
        if(SIN_ACTIVE_MATERIALS
        == SIN_MAX_MATERIALS)                   { fprintf(stderr, "Cannot create more than %u materials\n",
                                                  SIN_MAX_MATERIALS); return NULL;                                      }

        uint loc = sStack_pop(SIN_MAT_SLOTSTACK);
        WARD_EVIL_UNSIG(loc, 1);

        sh_insert(SIN_MATHASH, matid, loc);

        material         = SIN_matbucket+loc;

        material->id     = matid;
        for(uint i = 0;
            i < SIN_MATERIAL_MAX_TEXTURES; i++) { if(texid[i])
                                                { material->texloc[i] = SIN_texbucket_findloc(texid[i]); }

                                                  else
                                                { material->texloc[i] = 0; }                                            }

        material->shdloc = SIN_shdbucket_findloc(shdid);

        SIN_ACTIVE_MATERIALS++;

        Program* program  = SIN_shdbucket_get(material->shdloc);
        program->users++;

        for(uint i = 0;
            i < SIN_MATERIAL_MAX_TEXTURES; i++) { if(material->texloc[i])
                                                { Texture* tex      = SIN_texbucket_get(material->texloc[i]);
                                                  tex->users++; }                                                       }

    }

    return material;                                                                                                    }

//  - --- - --- - --- - --- -

void    del_material        (Material* material,
                             ushort loc)        {

    unsub_shader(material->shdloc);
    for(uint i = 0;
        i < SIN_MATERIAL_MAX_TEXTURES; i++)     { if(material->texloc[i])
                                                { unsub_tex(material->texloc[i]); }                                     }

    SIN_matbucket[loc] = SIN_emptymat;
    int memward = sStack_push(SIN_MAT_SLOTSTACK, loc);
    WARD_EVIL_UNSIG(memward, 1);

    SIN_ACTIVE_MATERIALS--;                                                                                             }

void    unsub_material      (ushort loc)        {

    Material* material = SIN_matbucket+loc;

    if(material)
    {
        material->users--;
        if(material->users == 0)                { sh_pop(SIN_MATHASH, loc); del_material(material, loc);                }
    }

                                                                                                                        }

//  - --- - --- - --- - --- -
