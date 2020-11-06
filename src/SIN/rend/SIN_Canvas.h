#ifndef __SIN_CANVAS_H__
#define __SIN_CANVAS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "ZJC_CommonTypes.h"
#include "../shaders/SIN_ShaderParams.h"

//  - --- - --- - --- - --- -

void  SIN_DepthBuffer_init      (const shaderParams* shader);
uint  SIN_DepthBuffer_getProgram();
uint* SIN_DepthBuffer_getUniform();
void  SIN_DepthBuffer_bind      ();
void  SIN_DepthBuffer_end       ();
void  SIN_DepthMap_bind         ();

//  - --- - --- - --- - --- -

void SIN_Canvas_init            (const shaderParams* shader);
void SIN_Canvas_draw            (uint texloc);
void SIN_Canvas_end             ();

//  - --- - --- - --- - --- -

#ifdef __cplusplus
}
#endif

#endif // __SIN_CANVAS_H__
