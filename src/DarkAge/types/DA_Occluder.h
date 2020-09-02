#ifndef __DARKAGE_OCCLUDER_H__
#define __DARKAGE_OCCLUDER_H__

#include "glm/glm.hpp"

#include "GAOL_Box.h"
#include "GAOL_Sphere.h"

#ifdef __cplusplus
extern "C" {
#endif

//  - --- - --- - --- - --- -

typedef struct DA_OCCLUDER {

    glm::vec3  bounds[8];
    COLFACE    planes[6];
    COLBOX*    box;

    DA_OCCLUDER    ()                           { ;                                                                     }
    ~DA_OCCLUDER   ()                           { ;                                                                     }

    void setBox    (COLBOX* _box);
    void getFrustum(glm::vec3& eye, glm::vec3& pos, glm::vec3& up, float zFar, float hFar, float wFar);
    int  sphTest   (COLSPHERE* sph);
    bool getVisible(glm::vec3* bounds);

} OCCLU;

//  - --- - --- - --- - --- -

#ifdef __cplusplus
}
#endif

#endif // __DARKAGE_OCCLUDER_H__
