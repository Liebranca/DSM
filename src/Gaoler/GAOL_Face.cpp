#include "GAOL_Face.h"
#include "lymath/ZJC_GOPS.h"

//  - --- - --- - --- - --- -

COLDATA GAOL_COLLISION_FACE::planeIsect
                    (GAOL_COLLISION_FACE* p)    {

    glm::vec3 inormal = glm::cross(normal, p->normal);
    float det = pow(glm::length(inormal), 2);

    if (det > 0.001f)                           { glm::vec3 ipoint = ((glm::cross(inormal, p->normal) * this->d) +
                                                                      (glm::cross(this->normal, inormal) * p->d)) / det;

                                                  return COLDATA(p->normal, ipoint);                                    }

    return COLDATA(p->normal, p->centroid);

}

//  - --- - --- - --- - --- -

bool GAOL_COLLISION_FACE::edgeIsect
                    (GAOL_COLLISION_FACE* p)    {

    float dx = subminf(centroid.x, p->centroid.x);
    float dy = subminf(centroid.y, p->centroid.y);
    float dz = subminf(centroid.z, p->centroid.z);

    float l1 = (this->edges[0].length + p->edges[0].length);
    float l2 = (this->edges[1].length + p->edges[1].length);
            
    bool b1 = dx+dz < l1;
    bool b2 = dy < l2;

    return b1 && b2;                                                                                                    }

//  - --- - --- - --- - --- -

RAYDATA GAOL_COLLISION_FACE::rayIsectPoint
                            (COLLINE* ray)      {

    float denom = dot(normal, ray->normal);
    if (abs(denom) <= 1e-4f)                    { return RAYDATA(false, {0,0,0});                                       }

    float t = -(dot(normal, ray->p1) + d) / denom;
    if (t <= 1e-4)                              { return RAYDATA(false, {0,0,0});                                       }

    return RAYDATA(true, ray->p1 + t * ray->normal);                                                                    }

