#include "SIN_Material.h"
#include "lyutils/ZJC_Evil.h"

#include <stdio.h>

Material* SIN_matbucket        = NULL;
Material  SIN_emptymat         = {0};
cushort   SIN_MAX_MATERIALS    = 64;
ushort    SIN_ACTIVE_MATERIALS = 0;

//  - --- - --- - --- - --- -

int       SIN_matbucket_init  ()                {

    SIN_matbucket = (Material*) evil_malloc(SIN_MAX_MATERIALS, sizeof(Material));
    return 0;                                                                                                           }

int       SIN_matbucket_end   ()                {

    WARD_EVIL_MFREE(SIN_matbucket);
    return 0;                                                                                                           }

//  - --- - --- - --- - --- -

Material* SIN_matbucket_find  (cchar* matid)    {

    for(uint i = 0;
        i < SIN_ACTIVE_MATERIALS; i++)          { if((SIN_matbucket+i)->id == matid) { return SIN_matbucket+i; }        }

    fprintf(stderr, "Material %s not found\n", matid);
    return NULL;                                                                                                        }

Material* SIN_matbucket_get   (ushort loc)      {

    if(loc < SIN_ACTIVE_MATERIALS)              { return SIN_matbucket+loc;                                             }

    fprintf(stderr, "No material at location %u\n", loc);
    return NULL;                                                                                                        }

ushort    SIN_matbucket_getloc(cchar* matid)    {

    for(uint i = 0;
        i < SIN_ACTIVE_MATERIALS; i++)          { if((SIN_matbucket+i)->id == matid) { return i; }                      }

    fprintf(stderr, "Material %s not found. Returning default location.\n", matid);
    return 0;                                                                                                           }

//  - --- - --- - --- - --- -

Material* build_material      (cchar* matid,
                               ushort   texture,
                               ushort   shader) {

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

        *(SIN_matbucket+SIN_ACTIVE_MATERIALS) = *material;
        SIN_ACTIVE_MATERIALS++;
    }

    return material;                                                                                                    }

//  - --- - --- - --- - --- -

int       del_material        (Material* material,
                               ushort loc)      {

    if(SIN_ACTIVE_MATERIALS == 0)               { fprintf(stderr,
                                                  "Material bucket is empty! There is no %s to delete.\n", material->id);
                                                  return ERROR;                                                         }

    *(SIN_matbucket+loc) = SIN_emptymat;

    for(uint i = loc;
    i < SIN_ACTIVE_MATERIALS; i++)              { *(SIN_matbucket+i) = *(SIN_matbucket+i+1);                            }

    SIN_ACTIVE_MATERIALS--;

    return 0;                                                                                                           }

ushort    unsub_material      (ushort loc)      {

    Material* material = SIN_matbucket_get(loc);
    if(material)
    {
        material->users--;
        if(material->users == 0)                { del_material(material, loc);                                          }

        return loc;
    }

    fprintf(stderr, "No material at location %u\n", loc);
    return ERROR;                                                                                                       }

//  - --- - --- - --- - --- -
