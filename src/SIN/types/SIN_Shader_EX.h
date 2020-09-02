#ifndef __SIN_SHADEREX_H__
#define __SIN_SHADEREX_H__

#include "glm/glm.hpp"
#include "ZJC_CommonTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

//  - --- - --- - --- - --- -

void shader_chkProgram        (ushort loc);
void shader_setProgram        (ushort loc);
void shader_useProgram        ();

void shader_update_model      (glm::mat4* model);
void shader_update_nmat       (glm::mat3* nmat);
void shader_update_camera     (glm::mat4* viewprojection, glm::vec3* camfwd, glm::vec3* campos);

void shader_update_viewproj   (glm::mat4* viewprojection);
void shader_update_campos     (glm::vec3* campos);

void shader_update_ambient    (glm::vec4* ambient_color);

//  - --- - --- - --- - --- -

#ifdef __cplusplus
}
#endif

#endif // __SIN_SHADEREX_H__
