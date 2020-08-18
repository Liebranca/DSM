#include "GAOL_Bounds.h"
#include "lyutils/ZJC_Evil.h"

#define GAOL_MAX_BOUNDS 512

static COLBOUNDS* GAOL_boundbucket;

//  - --- - --- - --- - --- -

void       GAOL_boundbucket_init()              { GAOL_boundbucket = (COLBOUNDS*) evil_malloc(GAOL_MAX_BOUNDS,
                                                                                              sizeof(COLBOUNDS));       }

void       GAOL_boundbucket_end()               { WARD_EVIL_MFREE(GAOL_boundbucket);                                    }

COLBOUNDS* GAOL_boundbucket_get(ushort loc)     { return GAOL_boundbucket + loc;                                        }

void       GAOL_boundbucket_add(glm::vec4 points[8],
                                ushort loc)     { COLBOUNDS* bounds = GAOL_boundbucket + loc;
                                                  for(int i = 0; i < 8; i++) { bounds->points[i] = points[i]; }         }

//  - --- - --- - --- - --- -

GAOL_COLLISION_BOUNDS::GAOL_COLLISION_BOUNDS
                    (glm::vec4 _points[8])      {

    points[0] = _points[0]; points[1] = _points[1];
    points[2] = _points[2]; points[3] = _points[3];
    points[4] = _points[4]; points[5] = _points[5];
    points[6] = _points[6]; points[7] = _points[7];                                                                     }

void GAOL_COLLISION_BOUNDS::genBox
                            (glm::mat4& model)  {

    delete box;

    glm::vec3 co[8]  =  { glm::vec3(model * points[0]), glm::vec3(model * points[1]),
                          glm::vec3(model * points[2]), glm::vec3(model * points[3]),
                          glm::vec3(model * points[4]), glm::vec3(model * points[5]),
                          glm::vec3(model * points[6]), glm::vec3(model * points[7]) };

    box = new COLBOX(co);                                                                                               }

