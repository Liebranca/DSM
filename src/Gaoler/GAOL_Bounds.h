#ifndef __GAOL_BOUNDS_H__
#define __GAOL_BOUNDS_H__

#include "GAOL_Box.h"

#ifdef __cplusplus
extern "C" {
#endif

//  - --- - --- - --- - --- -

typedef struct GAOL_COLLISION_BOUNDS            {

    glm::vec4 points[8];
    COLBOX box;

    GAOL_COLLISION_BOUNDS()                     { ;                                                                     }
    ~GAOL_COLLISION_BOUNDS()                    { ;                                                                     }

    GAOL_COLLISION_BOUNDS(glm::vec4 _points[8]);

    COLBOX* genBox(glm::mat4& model);

} COLBOUNDS;

//  - --- - --- - --- - --- -

#ifdef __cplusplus
}
#endif

#endif //__GAOL_BOUNDS_H__

