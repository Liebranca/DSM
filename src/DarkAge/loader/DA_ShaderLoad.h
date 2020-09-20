#ifndef __DARKAGE_SHADER_LOAD_H__
#define __DARKAGE_SHADER_LOAD_H__

#include "ZJC_CommonTypes.h"
#include "SIN_Constants.h"

#ifdef __cplusplus
extern "C" {
#endif

//  - --- - --- - --- - --- -

typedef struct DA_SHADERLOAD_HANDLE {

    ushort  num_shaders;
    ushort  idbase;

    cchar** v_Sources[SIN_MAX_SHADERS];
    cchar** p_Sources[SIN_MAX_SHADERS];

} SHADERLOAD;

//  - --- - --- - --- - --- -

void DA_LOAD_SHADERS (const SHADERLOAD* shaders);
void DA_SHDENV_UPDATE(cushort* ids, cushort num_shaders);

//  - --- - --- - --- - --- -

#ifdef __cplusplus
}
#endif

#endif // __DARKAGE_SHADER_LOAD_H__
