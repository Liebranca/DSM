#ifndef __SIN_SHADER_H__
#define __SIN_SHADER_H__

#include "../Zajec/ZJC_CommonTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

#define __SIN_MAX_SHADERS_PER_PROGRAM__ 2

enum     SIN_UNIFORM_NAMES    { SIN_TRANSFORM_U, SIN_NORMAL_U, SIN_PROJECTION_U, SIN_CAMFWD_U, SIN_CAMPOS_U, SIN_SUNFWD_U,
                                SIN_AMBIENT_U, SIN_NUMLIGHTS_U, SIN_LIGHTS_U, __SIN_NUM_UNIFORMS__ };

//  - --- - --- - --- - --- -

typedef struct SIN_SHADER_PROGRAM {

    ushort id;
    ushort users;

    uint   location;
    uint   shaders [__SIN_MAX_SHADERS_PER_PROGRAM__];
    uint   uniforms[__SIN_NUM_UNIFORMS__];

} Program;

//  - --- - --- - --- - --- -

int      SIN_shdbucket_init ();
int      SIN_shdbucket_end  ();

ushort   SIN_shdbucket_findloc  (ushort id);
Program* SIN_shdbucket_get      (ushort loc);
Program* SIN_shdbucket_find     (ushort id);
Program* build_shader           (ushort id, cchar** vert_source, cchar** frag_source);

void     unsub_shader           (ushort loc);

//  - --- - --- - --- - --- -

#ifdef __cplusplus
}
#endif

#endif // __SIN_SHADER_H__
