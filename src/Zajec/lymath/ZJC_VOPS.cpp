#include "ZJC_VOPS.h"

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

