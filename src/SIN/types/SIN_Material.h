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

int       SIN_init_matbucket            (uint top_id);
int       SIN_end_matbucket             ();
void      SIN_unsub_material            (uint loc);

Material* SIN_build_material            (uint matid, uint texid, uchar texoffset);
Material* SIN_findItem_matbucket        (uint id, int shutit);
Material* SIN_getItem_matbucket         (uint loc);
uint      SIN_findLoc_matbucket         (uint id);

uint      SIN_getActiveCount_material   ();
uint      SIN_getOpaqueCount_material   ();

uint      SIN_getMax_material           ();
int       SIN_getIsOpaque_material      (uint loc);

#ifdef __cplusplus
}
#endif

#endif // __SIN_MATERIAL_H__
