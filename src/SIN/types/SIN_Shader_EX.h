#ifndef __SIN_SHADEREX_H__
#define __SIN_SHADEREX_H__

#include "glm/glm.hpp"
#include "ZJC_CommonTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

//  - --- - --- - --- - --- -

void SHDEX_umat4 (uint loc, glm::mat4* mat);
void SHDEX_umat3 (uint loc, glm::mat3* mat);
void SHDEX_uvec4 (uint loc, glm::vec4* vec);
void SHDEX_uvec3 (uint loc, glm::vec3* vec);
void SHDEX_uvec2 (uint loc, glm::vec2* vec);

//  - --- - --- - --- - --- -

void SHDEX_baseTransformUpdate(glm::mat4* model);
void SHDEX_fullTransformUpdate(glm::mat4* model, glm::mat3* nmat);

//  - --- - --- - --- - --- -

#ifdef __cplusplus
}
#endif

#endif // __SIN_SHADEREX_H__
