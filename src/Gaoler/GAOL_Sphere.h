#ifndef __GAOL_SPHERE_H__
#define __GAOL_SPHERE_H__

#include "lymath/ZJC_VOPS.h"
#include "GAOL_Face.h"

#ifdef __cplusplus
extern "C" {
#endif

//  - --- - --- - --- - --- -

typedef struct GAOL_COLLISION_SPHERE {

    glm::vec3 origin;
    float     radius;

    GAOL_COLLISION_SPHERE ()                    { ;                                                                     }
    ~GAOL_COLLISION_SPHERE()                    { ;                                                                     }

    GAOL_COLLISION_SPHERE (glm::vec3 _origin,
                           float     _radius)
                           :origin(_origin),
                            radius(_radius)     { ;                                                                     }

    int boxIsect          (COLFACE* planes);

} COLSPHERE;

//  - --- - --- - --- - --- -

#ifdef __cplusplus
}
#endif

#endif //__GAOL_SPHERE_H__
