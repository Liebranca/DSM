#ifndef __SIN_SHADER_H__
#define __SIN_SHADER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../Zajec/ZJC_CommonTypes.h"
#include "../shaders/SIN_ShaderParams.h"

#define __SIN_MAX_SHADERS_PER_PROGRAM__ 2

enum     SIN_UNIFORM_NAMES    { SIN_MODEL_U, SIN_NMAT_U, __SIN_NUM_UNIFORMS__ };

//  - --- - --- - --- - --- -

typedef struct SIN_SHADER_PROGRAM {

    ushort id;
    ushort users;

    uint   location;
    uint   shaders [__SIN_MAX_SHADERS_PER_PROGRAM__];
    uint   uniforms[__SIN_NUM_UNIFORMS__           ];

    ushort flags;

} Program;

//  - --- - --- - --- - --- -

int      SIN_shdbucket_init     ();
int      SIN_shdbucket_end      ();

ushort   SIN_shdbucket_findloc  (ushort id);
Program* SIN_shdbucket_get      (ushort loc);
Program* SIN_shdbucket_find     (ushort id);
Program* SIN_buildShader        (ushort id, const shaderParams* shader);

void     SIN_unsubShader       (ushort loc);

int      SIN_chkProgram        (ushort loc);
void     SIN_setProgram        (ushort loc);
void     SIN_useProgram        ();

void     SIN_resetShdloc       ();

//  - --- - --- - --- - --- -

uint SIN_createShader          (cchar** source, uchar num_sources, uint shaderType);
void SIN_checkShaderError      (uint shader, uint flag, int isProgram, const char* errorMessage);

extern Program* __program;
extern ushort   __curShdLoc;

#ifdef __cplusplus
}
#endif

#endif // __SIN_SHADER_H__
