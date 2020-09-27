#ifndef __DARKAGE_RESOURCE_H__
#define __DARKAGE_RESOURCE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "ZJC_CommonTypes.h"
#include "SIN_Constants.h"

//  - --- - --- - --- - --- -

typedef struct DA_ASSETLOAD_HANDLE {

    cchar*  tex_archive;
    cushort num_textures;
    cushort tex_idbase;
    cuchar  tex_offset;

    cushort num_materials;
    cushort mat_idbase;
    cushort mat_texture_idbase;
    cushort mat_num_textures[ZJC_DAFSIZE];
    cushort mat_shader_ids[ZJC_DAFSIZE];
    cuchar  mat_opaque[ZJC_DAFSIZE];

    cchar*  mesh_archive;
    cushort num_meshes;
    cushort mesh_idbase;
    cuchar  mesh_offset;

    int    loaded;

} RESOURCE;

//  - --- - --- - --- - --- -

void DA_LOAD_RESOURCE(RESOURCE* res, cchar* cwd);

//  - --- - --- - --- - --- -

#ifdef __cplusplus
}
#endif

#endif // __DARKAGE_H__
