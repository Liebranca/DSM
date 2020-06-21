#ifndef __SIN_MATERIAL_H__
#define __SIN_MATERIAL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../Zajec/ZJC_CommonTypes.h"

//  - --- - --- - --- - --- -

typedef struct SIN_MATERIAL {

    ushort id;

    ushort shader;
    ushort texture;
    ushort users;

} Material;

//  - --- - --- - --- - --- -

int       SIN_matbucket_init     ();
int       SIN_matbucket_end      ();
void      unsub_material         (ushort matid);

Material* build_material         (ushort matid, ushort texture, ushort shader);
Material* SIN_matbucket_find     (ushort matid);
Material* SIN_matbucket_get      (ushort loc);
ushort    SIN_matbucket_findloc  (ushort matid);

#ifdef __cplusplus
}
#endif

#endif // __SIN_MATERIAL_H__
