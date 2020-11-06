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

    ushort shdloc;
    ushort texloc;
    ushort users;

} Material;

//  - --- - --- - --- - --- -

int       SIN_matbucket_init     ();
int       SIN_matbucket_end      ();
void      SIN_unsubMaterial      (ushort loc);

Material* SIN_buildMaterial      (ushort matid, ushort texid, uchar texoffset);
Material* SIN_matbucket_find     (ushort matid);
Material* SIN_matbucket_get      (ushort loc);
ushort    SIN_matbucket_findloc  (ushort matid);

ushort    SIN_getActiveMatCount  ();
ushort    SIN_getOpaqueMatCount  ();

ushort    SIN_getMaxMaterials    ();
int       SIN_getMaterialIsOpaque(ushort loc);

#ifdef __cplusplus
}
#endif

#endif // __SIN_MATERIAL_H__
