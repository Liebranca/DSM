#ifndef LYMATH_VOPS_H
#define LYMATH_VOPS_H

//  - --- - --- - --- - --- -

#include "../../../include/glm/glm.hpp"
extern glm::vec3 ZVEC3; //Zero-sum vec3

//  - --- - --- - --- - --- -

namespace vops {
    // A buffalo holds a single bool and a single float
    struct booflo { bool a = false; float x = 0.0f; };

    //Returns a copy of vec3 with the Y component zeroed
    glm::vec3 xzvec(glm::vec3 v); 
}

//  - --- - --- - --- - --- -

namespace lymath {
    using namespace vops;
}

#endif //LYMATH_VOPS_H
