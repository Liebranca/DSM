#ifndef __SIN_AMBIENT_H__
#define __SIN_AMBIENT_H__

#include "glm/glm.hpp"
#include "ZJC_CommonTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

//  - --- - --- - --- - --- -

struct SIN_UAMBIENT {

    glm::mat4 ViewProjection;
    glm::vec4 CamFwd;
    glm::vec4 Color;
};

//  - --- - --- - --- - --- -

void SIN_Ambient_init       ();
void SIN_Ambient_end        ();

void SIN_Ambient_setCam     (glm::mat4* ViewProjection, glm::vec3* CamFwd);
void SIN_Ambient_setSky     (glm::vec3* color, float intensity);
void SIN_Ambient_updateCheck();

//  - --- - --- - --- - --- -

#ifdef __cplusplus
}
#endif

#endif // __SIN_AMBIENT_H__
