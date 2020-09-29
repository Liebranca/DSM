#ifndef __SIN_SHADEREX_H__
#define __SIN_SHADEREX_H__

#include "glm/glm.hpp"
#include "ZJC_CommonTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

//  - --- - --- - --- - --- -

typedef struct SIN_SHADER_LIGHTSOURCE {

    glm::mat4 mat;
    glm::vec3 position;
    glm::vec3 dirn;
    glm::vec4 color;

} SIN_LIGHT;

//  - --- - --- - --- - --- -

void shader_update_model      (glm::mat4* model);
void shader_update_nmat       (glm::mat3* nmat);
void shader_update_camera     (glm::mat4* viewprojection, glm::vec3* camfwd, glm::vec3* campos);

void shader_update_viewproj   (glm::mat4* viewprojection);
void shader_update_lights     (SIN_LIGHT* lights, uint num_lights);
void shader_update_campos     (glm::vec3* campos);

void shader_update_ambient    (glm::vec4* ambient_color);

//  - --- - --- - --- - --- -

#ifdef __cplusplus
}
#endif

#endif // __SIN_SHADEREX_H__
