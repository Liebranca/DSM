#include "SIN_Material.h"

#include "lyarr/ZJC_Hash.h"
#include "lyarr/ZJC_Stack.h"
#include "lyutils/ZJC_Evil.h"

#include <stdio.h>

#define   SIN_MAX_MATERIALS       64

Material* SIN_matbucket         = NULL;
Material  SIN_emptymat          = {0};
ushort    SIN_ACTIVE_MATERIALS  = 0;

sStack    SIN_MAT_SLOTSTACK     = {0};
sHash     SIN_MATHASH           = {0};

//  - --- - --- - --- - --- -

int       SIN_matbucket_init  ()                {

    SIN_MATHASH = build_sHash(SIN_MAX_MATERIALS);

    SIN_MAT_SLOTSTACK = build_sStack(SIN_MAX_MATERIALS);
    for(int i = SIN_MAX_MATERIALS-1; i > -1; i--)  { sStack_push(&SIN_MAT_SLOTSTACK, i);                                }

    SIN_matbucket = (Material*) evil_malloc(SIN_MAX_MATERIALS, sizeof(Material));

    return 0;                                                                                                           }

int       SIN_matbucket_end   ()                {

    del_sHash(&SIN_MATHASH);
    del_sStack(&SIN_MAT_SLOTSTACK);
    WARD_EVIL_MFREE(SIN_matbucket);
    return 0;                                                                                                           }

//  - --- - --- - --- - --- -

Material* SIN_matbucket_find  (ushort matid)    {

    ushort loc = sh_hashloc(&SIN_MATHASH, matid);
    if(loc == 0)                                { return NULL;                                                          }

    return SIN_matbucket+(loc-1);                                                                                       }

ushort SIN_matbucket_findloc  (ushort matid)    {

    ushort loc = sh_hashloc(&SIN_MATHASH, matid);
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
                               ushort texture,
                               ushort shader)   {

    Material* material = SIN_matbucket_find(matid);

    if(material == NULL)
    {
        if(SIN_ACTIVE_MATERIALS
        == SIN_MAX_MATERIALS)                   { fprintf(stderr, "Cannot create more than %u materials\n",
                                                  SIN_MAX_MATERIALS); return NULL;                                      }

        material = 0;
        material->id       = matid;
        material->texture  = texture;
        material->shader   = shader;

        uint loc = sStack_pop(&SIN_MAT_SLOTSTACK);
        WARD_EVIL_UNSIG(loc, 1);

        sh_insert(&SIN_MATHASH, matid, loc);

        SIN_matbucket[loc] = *material;
        SIN_ACTIVE_MATERIALS++;
    }

    return material;                                                                                                    }

//  - --- - --- - --- - --- -

void    del_material        (Material* material,
                             ushort loc)        {

    SIN_matbucket[loc] = SIN_emptymat;
    int memward = sStack_push(&SIN_MAT_SLOTSTACK, loc);
    WARD_EVIL_UNSIG(memward, 1);

    SIN_ACTIVE_MATERIALS--;                                                                                             }

void    unsub_material      (ushort loc)        {

    Material* material = SIN_matbucket+loc;

    if(material)
    {
        material->users--;
        if(material->users == 0)                { sh_pop(&SIN_MATHASH, loc); del_material(material, loc);               }
    }

                                                                                                                        }

//  - --- - --- - --- - --- -
