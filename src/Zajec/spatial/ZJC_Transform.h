#ifndef __ZJC_TRANSFORM_H__
#define __ZJC_TRANSFORM_H__

//  - --- - --- - --- - --- -

#include "../../../include/glm/glm.hpp"
#include "../../../include/glm/gtx/quaternion.hpp"
#include "../lymath/ZJC_VOPS.h"

#ifdef __cplusplus
extern "C" {
#endif

#define IDENTITY glm::mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1)
#define Y_AXIS   glm::vec3(0, 1, 0)

typedef struct TRANSFORM3D {

    glm::vec3 position;
    glm::quat orientation;
    glm::vec3 scaling;
    glm::vec2 dirn;

    float fwd  = 0.0f;
    float tick = 0.0f;

    TRANSFORM3D* parent;

    TRANSFORM3D(glm::vec3 pos   = { 0,0,0   },
                glm::quat rot   = { 1,0,0,0 },
                glm::vec3 scale = { 1,1,1   })
               :position   (pos),
                orientation(glm::quat(1,0,0,0)),
                scaling    (scale)              { dirn = {1, 1}; rotate(rot);                                           }

    ~TRANSFORM3D()                              { ;                                                                     }

    glm::mat4 getModel     (bool ignoreParent = false);
    glm::mat3 getNormal    (glm::mat4& model);

    void      setParent    (TRANSFORM3D* par)   { this->parent = par;                                                   }

    bool      faceTo       (glm::vec3 v,
                            float elapsed,
                            float rfac = 10.0f,
                            bool fullRot = false);

    void      rotate       (glm::quat delta);
    void      move         (glm::vec3 pos)      { position += pos;                                                      }

} T3D;

//      - --- - --- - --- - --- -

#ifdef __cplusplus
}
#endif

#endif // __ZJC_TRANSFORM_H__