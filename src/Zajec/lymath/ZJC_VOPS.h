#ifndef __ZJC_VOPS_H__
#define __ZJC_VOPS_H__

//  - --- - --- - --- - --- -

#include "../../../include/glm/glm.hpp"

//  - --- - --- - --- - --- -

#define ZVEC3 glm::vec3(0,0,0)

struct Booflo { 
    bool a = false;
    float x = 0.0f;
};

glm::vec3 xzvec(glm::vec3 v); 

glm::vec3 localizePos(glm::vec3& mvec,
                      glm::vec3 fvec,
                      glm::vec3 lvec);

//  - --- - --- - --- - --- -

#endif //__ZJC_VOPS_H__
