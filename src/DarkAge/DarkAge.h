#ifndef __DARKAGE_H__
#define __DARKAGE_H__

#include "types/DA_WorldManager.h"
#include "core/DA_Engine.h"
#include "types/DA_Camera.h"
#include "types/DA_Occluder.h"

#ifdef __cplusplus
extern "C" {
#endif

//  - --- - --- - --- - --- -

void DARKAGE_BEGIN(int flags);
void DARKAGE_END  ();

//  - --- - --- - --- - --- -

#ifdef __cplusplus
}
#endif

#endif // __DARKAGE_H__
