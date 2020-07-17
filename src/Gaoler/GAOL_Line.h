#ifndef __GAOL_LINE_H__
#define __GAOL_LINE_H__

#include "GAOL_ColData.h"

#ifdef __cplusplus
extern "C" {
#endif

//  - --- - --- - --- - --- -

typedef struct GAOL_COLLISION_LINE {

    float     length;

    glm::vec3 p1;
    glm::vec3 p2;
    glm::vec3 normal;
    glm::vec3 centroid;

    GAOL_COLLISION_LINE()                       { ;                                                                     }
    GAOL_COLLISION_LINE(glm::vec3 _p1,
                        glm::vec3 _p2)
                       :p1(_p1), p2(_p2)        { normal   = p2 - p1;
                                                  centroid = (p1 + p2) * 0.5f;
                                                  length   = glm::length(normal);
                                                  normal   = glm::normalize(normal);                                    }

    ~GAOL_COLLISION_LINE()                      { ;                                                                     }

    glm::vec3 pointAlong(float s)               { return p1 + ((normal * length) * s);                                  }

} COLLINE;

//  - --- - --- - --- - --- -

#ifdef __cplusplus
}
#endif

#endif //__GAOL_LINE_H__
