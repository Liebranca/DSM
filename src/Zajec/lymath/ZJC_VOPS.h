#ifndef __ZJC_VOPS_H__
#define __ZJC_VOPS_H__

//  - --- - --- - --- - --- -

#include "../../../include/glm/glm.hpp"

//  - --- - --- - --- - --- -

#define ZVEC3 glm::vec3      (0,0,0)

static const  glm::vec3 yAxis(0,1,0);

struct Booflo { 
    bool a = false;
    float x = 0.0f;
};

glm::vec3 xzvec                    (glm::vec3 v); 

glm::vec3 localizePos              (glm::vec3& mvec, glm::vec3 fvec, glm::vec3 lvec);

//  - --- - --- - --- - --- -

glm::mat4 getOrthographicProjection(float width, float height, float scale, float near, float far);
glm::mat4 getPerspectiveProjection (float width, float height, float fov, float near, float far);

//  - --- - --- - --- - --- -

#endif //__ZJC_VOPS_H__
