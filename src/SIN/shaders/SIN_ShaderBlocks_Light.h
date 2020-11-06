#ifndef __SIN_SHADERBLOCKS_LIGHT_H__
#define __SIN_SHADERBLOCKS_LIGHT_H__

#include "SIN_ShaderParams.h"

#ifdef __cplusplus
extern "C" {
#endif

extern const GLchar* SIN_ShaderBlock_LightUniforms[1];
extern const GLchar* SIN_ShaderBlock_LightSamplers[2];

extern const GLchar* SIN_ShaderBlock_VertLightIn;
extern const GLchar* SIN_ShaderBlock_VertLightOut;
extern const GLchar* SIN_ShaderBlock_FragLight;

#ifdef __cplusplus
}
#endif

#endif // __SIN_SHADERBLOCKS_LIGHT_H__
