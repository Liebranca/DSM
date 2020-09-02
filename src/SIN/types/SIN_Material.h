#ifndef __SIN_MATERIAL_H__
#define __SIN_MATERIAL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../Zajec/ZJC_CommonTypes.h"

//  - --- - --- - --- - --- -

#define SIN_MATERIAL_MAX_TEXTURES 4

typedef struct SIN_MATERIAL {

    ushort id;

    ushort shdloc;
    ushort texloc[SIN_MATERIAL_MAX_TEXTURES];
    ushort users;

} Material;

//  - --- - --- - --- - --- -

int       SIN_matbucket_init     ();
int       SIN_matbucket_end      ();
void      unsub_material         (ushort loc);

Material* build_material         (ushort matid, ushort texid[3], ushort shdid);
Material* SIN_matbucket_find     (ushort matid);
Material* SIN_matbucket_get      (ushort loc);
ushort    SIN_matbucket_findloc  (ushort matid);

ushort    SIN_getActiveMaterials ();

#ifdef __cplusplus
}
#endif

#endif // __SIN_MATERIAL_H__
