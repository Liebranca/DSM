#ifndef __SIN_CANVAS_H__
#define __SIN_CANVAS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "ZJC_CommonTypes.h"

//  - --- - --- - --- - --- -

void  SIN_DepthBuffer_init      (cchar** vert_source, cchar** frag_source);
uint  SIN_DepthBuffer_getProgram();
uint* SIN_DepthBuffer_getUniform();
void  SIN_DepthBuffer_bind      ();
void  SIN_DepthBuffer_end       ();
void  SIN_DepthMap_bind         ();

//  - --- - --- - --- - --- -

void SIN_Canvas_init            (cchar** vert_source, cchar** frag_source);
void SIN_Canvas_draw            ();
void SIN_Canvas_end             ();

//  - --- - --- - --- - --- -

#ifdef __cplusplus
}
#endif

#endif // __SIN_CANVAS_H__
