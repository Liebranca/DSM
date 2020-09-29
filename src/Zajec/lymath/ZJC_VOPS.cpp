#include "ZJC_VOPS.h"
#include "../../../include/glm/gtx/transform.hpp"

//  - --- - --- - --- - --- -

glm::vec3 xzvec(glm::vec3 v)                    { return { v.x, 0, v.z };                                               }

glm::vec3 localizePos(glm::vec3& mvec,
                      glm::vec3 fvec,
                      glm::vec3 lvec)           {

        glm::vec3 displace = { 0,0,0 };
        if (mvec[2]) {
            displace += fvec * mvec[2];
        }
        if (mvec[0]) {
            displace += lvec * mvec[0];
        }

        displace[1] = mvec[1];
        return displace;                                                                                                }

//  - --- - --- - --- - --- -

glm::mat4 getOrthographicProjection(float width,
                                    float height,
                                    float scale,
                                    float near,
                                    float far)  {

    width *= 0.5f;       height *= 0.5f;
    width *= scale; height *= scale;
    return glm::ortho(-width, width, -height, height, near, far);                                                       }

glm::mat4 getPerspectiveProjection (float width,
                                    float height,
                                    float fov,
                                    float near,
                                    float far)  { return glm::perspective(glm::radians(fov), width/height, near, far ); }

//  - --- - --- - --- - --- -

