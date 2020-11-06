#ifndef __GAOL_FRUSTUM_H__
#define __GAOL_FRUSTUM_H__


#include "ZJC_CommonTypes.h"

#include "GAOL_Sphere.h"
#include "GAOL_Box.h"

#ifdef __cplusplus
extern "C" {
#endif

//  - --- - --- - --- - --- -

typedef struct GAOL_VIEWING_FRUSTUM {

    glm::mat4 projection;
    glm::vec2 dim;

    COLFACE   planes[6];

    int       isOrtho;

    float     orthoScale;
    float     FOV;

    float     zNear;
    float     hNear;
    float     wNear;

    float     zFar;
    float     hFar;
    float     wFar;

    GAOL_VIEWING_FRUSTUM() { ; }
    GAOL_VIEWING_FRUSTUM(glm::vec2 _dim, float _FOV, int _isOrtho, float _orthoScale, float _zNear, float _zFar);

    void buildPlanes    (glm::vec3& pos, glm::vec3& fwd, glm::vec3& up);

    int  sphereIsect    (COLSPHERE* sph);
    int  boxIsect       (COLBOX*    box);

    int  pointsInside   (glm::vec3* points, uint num_points);
    int  pointInside    (glm::vec3  point);

    int  frustumIsect   (GAOL_VIEWING_FRUSTUM* other);
    int  cageIsect_opt  (COLFACE* other);
    int  rayToBox       (RAYDATA ray);

    int cellInside      (COLSPHERE* sph, COLBOX* box);

} viewFrustum;

//  - --- - --- - --- - --- -

#ifdef __cplusplus
}
#endif

#endif //__GAOL_FRUSTUM_H__
