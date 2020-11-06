#ifndef __DA_ENGINE_H__
#define __DA_ENGINE_H__

#include "../types/DA_GameObject.h"

#ifdef __cplusplus
extern "C" {
#endif
//  - --- - --- - --- - --- -

void SIN_Render_Update   ();
void SIN_Render_DepthPass();
void SIN_Render_ColorPass();

//  - --- - --- - --- - --- -

#ifdef __cplusplus
}
#endif

#endif // __DA_ENGINE_H__
