#ifndef __GAOL_COLDATA_H__
#define __GAOL_COLDATA_H__

#include "glm.hpp"

#ifdef __cplusplus
extern "C" {
#endif

//  - --- - --- - --- - --- -

typedef struct GAOL_COLLISION_DATA              {

    glm::vec3 colnormal;
    glm::vec3 colpoint;

    GAOL_COLLISION_DATA(glm::vec3 _colnormal,
                        glm::vec3& _colpoint)
                       :colnormal(_colnormal),
                        colpoint(_colpoint)     { ;                                                                     }

    ~GAOL_COLLISION_DATA()                      { ;                                                                     }

} COLDATA;

//  - --- - --- - --- - --- -

typedef struct GAOL_COLLISION_RAYDATA           {

    bool      hitObject;
    glm::vec3 point;

    GAOL_COLLISION_RAYDATA()                    { hitObject = false; point = {0,0,0};                                   }
    ~GAOL_COLLISION_RAYDATA()                   { ;                                                                     }

    GAOL_COLLISION_RAYDATA(bool hit,
                           glm::vec3 p)
                          :hitObject(hit),
                           point(p)             { ;                                                                     }

} RAYDATA;

//  - --- - --- - --- - --- -

#ifdef __cplusplus
}
#endif

#endif //__GAOL_COLDATA_H__
