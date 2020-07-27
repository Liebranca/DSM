#ifndef __SIN_H__
#define __SIN_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "types/SIN_Texture.h"
#include "types/SIN_Shader.h"
#include "types/SIN_Material.h"
#include "types/SIN_Mesh.h"
#include "types/SIN_MeshBatch.h"

#include "GL/glew.h"

//  - --- - --- - --- - --- -

void SIN_INIT(unsigned char flags);
void SIN_END ();

//  - --- - --- - --- - --- -

void SIN_SWIPESCREEN(float color[4], float mult);

#ifdef __cplusplus
}
#endif

#endif // __SIN_H__
