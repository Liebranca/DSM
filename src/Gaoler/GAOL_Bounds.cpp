#include "GAOL_Bounds.h"


GAOL_COLLISION_BOUNDS::GAOL_COLLISION_BOUNDS
                    (glm::vec4 _points[8])      {

    points[0] = _points[0]; points[1] = _points[1];
    points[2] = _points[2]; points[3] = _points[3];
    points[4] = _points[4]; points[5] = _points[5];
    points[6] = _points[6]; points[7] = _points[7];                                                                     }

COLBOX* GAOL_COLLISION_BOUNDS::genBox
                            (glm::mat4& model)  {

    glm::vec3 co[8]  =  { glm::vec3(model * points[0]), glm::vec3(model * points[1]),
                          glm::vec3(model * points[2]), glm::vec3(model * points[3]),
                          glm::vec3(model * points[4]), glm::vec3(model * points[5]),
                          glm::vec3(model * points[6]), glm::vec3(model * points[7]) };

    box = co;
    return &box;                                                                                                        }

