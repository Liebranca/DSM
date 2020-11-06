#include "GAOL_Frustum.h"

#include "gtx/transform.hpp"

#include "lymath/ZJC_GOPS.h"
#include "lymath/ZJC_VOPS.h"

#include "GAOL_Constants.h"

//  - --- - --- - --- - --- -

GAOL_VIEWING_FRUSTUM::GAOL_VIEWING_FRUSTUM
        (glm::vec2 _dim, float _FOV,
         int _isOrtho, float _orthoScale,
         float _zNear, float _zFar)             {

    FOV          = _FOV;
    isOrtho      = _isOrtho;
    orthoScale   = _orthoScale;
    zNear        = _zNear;
    zFar         = _zFar;
    dim          = _dim;

    float aspect = dim.x / dim.y;

    hNear        = 2 * tan(FOV / 2) * zNear;
    wNear        = this->hNear * aspect;
    hFar         = 2 * tan(FOV / 2) * zFar;
    wFar         = hFar * aspect;

    if(isOrtho) { projection = getOrthographicProjection(dim.x, dim.y, orthoScale, zNear, zFar); }
    else        { projection = getPerspectiveProjection (dim.x, dim.y, FOV, zNear, zFar       ); }
                                                                                                                        }

void GAOL_VIEWING_FRUSTUM::buildPlanes
(glm::vec3& pos, glm::vec3& fwd, glm::vec3& up) {

    glm::vec3 hAxis = glm::normalize(glm::cross(fwd, up));

    glm::vec3 fc    = pos + fwd * zFar;
    glm::vec3 ftl   = fc + (up * (hFar / 2)) + (hAxis * (wFar / 2));
    glm::vec3 ftr   = fc + (up * (hFar / 2)) - (hAxis * (wFar / 2));
    glm::vec3 fbl   = fc - (up * (hFar / 2)) + (hAxis * (wFar / 2));
    glm::vec3 fbr   = fc - (up * (hFar / 2)) - (hAxis * (wFar / 2));

    glm::vec3 nc    = pos + fwd * zNear;
    glm::vec3 ntl   = nc + (up * (hNear / 2)) + (hAxis * (wNear / 2));
    glm::vec3 ntr   = nc + (up * (hNear / 2)) - (hAxis * (wNear / 2));
    glm::vec3 nbl   = nc - (up * (hNear / 2)) + (hAxis * (wNear / 2));
    glm::vec3 nbr   = nc - (up * (hNear / 2)) - (hAxis * (wNear / 2));

    planes[0]       = COLFACE(nbl,nbr,fbr); // bottom
    planes[1]       = COLFACE(ftl,ntl,ntr); // top
    planes[2]       = COLFACE(nbl,nbr,ntr); // front
    planes[3]       = COLFACE(ftl,ftr,fbr); // back
    planes[4]       = COLFACE(ntr,ftr,fbr); // right
    planes[5]       = COLFACE(ntl,ftl,fbl); // left

    planes[0].normal = -planes[0].normal;
    planes[0].d      = -planes[0].d;

    planes[4].normal = -planes[4].normal;
    planes[4].d      = -planes[4].d;                                                                                    }

//  - --- - --- - --- - --- -


int GAOL_VIEWING_FRUSTUM::rayToBox(RAYDATA ray) {

    if(ray.hitObject)                           { if(pointInside(ray.point)) { return true; }                           }
    return false;                                                                                                       }

int GAOL_VIEWING_FRUSTUM::sphereIsect
                        (COLSPHERE* sph)        { return sph->boxIsect(planes);                                         }

int GAOL_VIEWING_FRUSTUM::pointsInside
                        (glm::vec3* points,
                         uint num_points    )   {

    for (uint i = 0; i < num_points; i++)       { if (pointInside(points[i]))
                                                { return 1; }                                                           }

    return 0;                                                                                                           }

int GAOL_VIEWING_FRUSTUM::pointInside
                        (glm::vec3  point)      {

    for (uint i = 0; i < 6; i++)                { if ( planes[i].pointTest(point) < GAOL_COLNORMAL)
                                                { return 0; }                                                           }

    return 1;                                                                                                           }

int GAOL_VIEWING_FRUSTUM::boxIsect
                        (COLBOX*    box)        {

    for(uchar j = 0; j < 6; j++)                { if(box->cageIsect_opt(planes+j)) { return 1; }                        }

    return 0;                                                                                                           }

int  GAOL_VIEWING_FRUSTUM::frustumIsect
                (GAOL_VIEWING_FRUSTUM* other)   {

    for(uchar j = 0; j < 6; j++)                { if(cageIsect_opt(other->planes+j)) { return 1; }                      }

    return 0;                                                                                                           }

int GAOL_VIEWING_FRUSTUM::cageIsect_opt
                (COLFACE* other)                {

    if     (rayToBox(other->rayIsectPoint(&planes[2].edges[1])))                           { return true; }
    else if(rayToBox(other->rayIsectPoint(&planes[3].edges[1])))                           { return true; }
    else if(rayToBox(other->rayIsectPoint(&planes[4].edges[1])))                           { return true; }
    else if(rayToBox(other->rayIsectPoint(&planes[5].edges[1])))                           { return true; }

    else if(rayToBox(other->rayIsectPoint(&planes[0].edges[0])))                           { return true; }
    else if(rayToBox(other->rayIsectPoint(&planes[0].edges[1])))                           { return true; }

    else if(rayToBox(other->rayIsectPoint(&planes[1].edges[0])))                           { return true; }
    else if(rayToBox(other->rayIsectPoint(&planes[1].edges[1])))                           { return true; }

    else if(rayToBox(other->rayIsectPoint(&COLLINE(planes[0].edges[1].p2, planes[0].p4)))) { return true; }
    else if(rayToBox(other->rayIsectPoint(&COLLINE(planes[0].p4, planes[0].edges[0].p1)))) { return true; }
    else if(rayToBox(other->rayIsectPoint(&COLLINE(planes[1].edges[1].p2, planes[1].p4)))) { return true; }
    else if(rayToBox(other->rayIsectPoint(&COLLINE(planes[1].p4, planes[1].edges[0].p1)))) { return true; }

    return false;                                                                                                       }

//  - --- - --- - --- - --- -

int GAOL_VIEWING_FRUSTUM::cellInside
            (COLSPHERE* sph, COLBOX* box)       {

    int cellInFrustum = sph->boxIsect(planes);

    if(cellInFrustum < 0)                       { cellInFrustum = boxIsect(box);                                        }

    return cellInFrustum;                                                                                               }

