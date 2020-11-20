#ifndef __SIN_MATERIAL_H__
#define __SIN_MATERIAL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../Zajec/ZJC_CommonTypes.h"
#include "../SIN_Constants.h"

//  - --- - --- - --- - --- -

typedef struct SIN_MATERIAL {

    uint id;

    uint shdloc;
    uint texloc;
    uint users;

} Material;

//  - --- - --- - --- - --- -

int       SIN_matbucket_init     ();
int       SIN_matbucket_end      ();
void      SIN_unsubMaterial      (uint loc);

Material* SIN_buildMaterial      (uint matid, uint texid, uchar texoffset);
Material* SIN_matbucket_find     (uint matid);
Material* SIN_matbucket_get      (uint loc);
uint      SIN_matbucket_findloc  (uint matid);

uint      SIN_getActiveMatCount  ();
uint      SIN_getOpaqueMatCount  ();

uint      SIN_getMaxMaterials    ();
int       SIN_getMaterialIsOpaque(uint loc);

#ifdef __cplusplus
}
#endif

#endif // __SIN_MATERIAL_H__
