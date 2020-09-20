#ifndef __SIN_H__
#define __SIN_H__

#include "types/SIN_Texture.h"
#include "types/SIN_Shader.h"
#include "types/SIN_Shader_EX.h"
#include "types/SIN_Material.h"
#include "types/SIN_Mesh.h"
#include "types/SIN_MeshBatch.h"

#include "GL/glew.h"

#ifdef __cplusplus
extern "C" {
#endif

//  - --- - --- - --- - --- -

void SIN_INIT(unsigned char flags);
void SIN_END ();

void SIN_SWIPESCREEN   ();
void SIN_UPDATE_AMBIENT();

//  - --- - --- - --- - --- -

extern glm::vec4 ambientColor;
extern float     ambientMult;
extern float     ambientIntensity;

#ifdef __cplusplus
}
#endif

#endif // __SIN_H__
