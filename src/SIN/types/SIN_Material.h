#ifndef __SIN_MATERIAL_H__
#define __SIN_MATERIAL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../Zajec/ZJC_CommonTypes.h"

//  - --- - --- - --- - --- -

typedef struct SIN_MATERIAL {

    cchar* id;

    ushort shader;
    ushort texture;
    ushort users;

} Material;

//  - --- - --- - --- - --- -

int       SIN_matbucket_init  ();
int       SIN_matbucket_end   ();
ushort    unsub_material      (ushort loc);

Material* build_material      (cchar* matid, ushort texture, ushort shader);
Material* SIN_matbucket_find  (cchar* matid);
Material* SIN_matbucket_get   (ushort loc);

ushort    SIN_matbucket_getloc(cchar* matid);

#ifdef __cplusplus
}
#endif

#endif // __SIN_MATERIAL_H__
