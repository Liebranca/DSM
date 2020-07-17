#ifndef __GAOL_FACE_H__
#define __GAOL_FACE_H__

#include "GAOL_Line.h"

#ifdef __cplusplus
extern "C" {
#endif

//  - --- - --- - --- - --- -

typedef struct GAOL_COLLISION_FACE {

    glm::vec3 centroid;
    glm::vec3 normal;
    glm::vec3 p4;

    COLLINE   edges[2];
    float     d;

    GAOL_COLLISION_FACE()                       { ;                                                                     }
    GAOL_COLLISION_FACE(glm::vec3 p1,
                        glm::vec3 p2,
                        glm::vec3 p3)
                       :edges()                 { centroid = (p1 + p3) * 0.5f;
                                                  normal   = glm::normalize(glm::cross(p2 - p1, p3 - p1));
                                                  d        = glm::dot(-centroid, normal);

                                                  edges[0] = COLLINE(p1, p2);
                                                  edges[1] = COLLINE(p2, p3);

                                                  p4 = p3 + glm::vec3(p1.x, 0, p1.z);                                   }

    ~GAOL_COLLISION_FACE()                      { ;                                                                     }

    COLDATA planeIsect(GAOL_COLLISION_FACE* p);
    bool    edgeIsect (GAOL_COLLISION_FACE* p);

    RAYDATA rayIsectPoint(COLLINE* ray);

} COLFACE;

//  - --- - --- - --- - --- -

#ifdef __cplusplus
}
#endif

#endif //__GAOL_FACE_H__

