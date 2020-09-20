#ifndef __SIN_MATERIAL_H__
#define __SIN_MATERIAL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../Zajec/ZJC_CommonTypes.h"
#include "../SIN_Constants.h"

//  - --- - --- - --- - --- -

typedef struct SIN_MATERIAL {

    ushort id;
    uchar  num_textures;

    ushort shdloc;
    ushort texloc[SIN_MATERIAL_MAX_TEXTURES];
    ushort users;

} Material;

//  - --- - --- - --- - --- -

int       SIN_matbucket_init     ();
int       SIN_matbucket_end      ();
void      unsub_material         (ushort loc);

Material* build_material         (ushort matid, cushort texid[SIN_MATERIAL_MAX_TEXTURES], ushort shdid);
Material* SIN_matbucket_find     (ushort matid);
Material* SIN_matbucket_get      (ushort loc);
ushort    SIN_matbucket_findloc  (ushort matid);

ushort    SIN_getActiveMaterials ();

#ifdef __cplusplus
}
#endif

#endif // __SIN_MATERIAL_H__
