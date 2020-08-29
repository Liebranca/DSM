#ifndef __GAOL_BOX_H__
#define __GAOL_BOX_H__

#include "GAOL_Face.h"
#include "lymath/ZJC_VOPS.h"

#ifdef __cplusplus
extern "C" {
#endif

//  - --- - --- - --- - --- -

typedef struct GAOL_COLLISION_BOX {

    COLFACE faces[6];

    glm::vec3 points[8];
    float area[2];

    GAOL_COLLISION_BOX (glm::vec3 co[8]);

    GAOL_COLLISION_BOX ()                       { ;                                                                     }
    ~GAOL_COLLISION_BOX()                       { ;                                                                     }

    GAOL_COLLISION_BOX selfProjection(glm::vec3 dirn);
    RAYDATA*           genCage       (COLFACE* other);
    RAYDATA            raytest       (GAOL_COLLISION_BOX* other, COLLINE* ray);

    bool               bottomFaceCol (COLFACE* other, float fac);
    bool               inDomainOf    (GAOL_COLLISION_BOX* b);
    bool               pointInside   (glm::vec3 point);
    bool               cageIsect     (RAYDATA cage[12]);
    bool               lineInside    (COLLINE* ray);
    bool               pointInDomain (glm::vec3 v);
    bool               cageIsect_opt (COLFACE* other);
    bool               rayToBox      (RAYDATA  ray);

    glm::vec3          boxToBox      (GAOL_COLLISION_BOX* b, glm::vec3 dirn);

    Booflo             groundCheck   (GAOL_COLLISION_BOX* other, float fac);

} COLBOX;

//  - --- - --- - --- - --- -

#ifdef __cplusplus
}
#endif

#endif //__GAOL_BOX_H__
