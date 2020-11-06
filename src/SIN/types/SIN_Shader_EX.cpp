#include "SIN_Shader_EX.h"
#include "SIN_Shader.h"
#include "GL/glew.h"

#include <string>

//  - --- - --- - --- - --- -

void SHDEX_umat4 (uint loc, glm::mat4* mat)     { glUniformMatrix4fv(loc, 1, GL_FALSE, &((*mat)[0][0]));                }
void SHDEX_umat3 (uint loc, glm::mat3* mat)     { glUniformMatrix3fv(loc, 1, GL_FALSE, &((*mat)[0][0]));                }
void SHDEX_uvec4 (uint loc, glm::vec4* vec)     { glUniform4fv      (loc, 1,           &((*vec)[0]   ));                }
void SHDEX_uvec3 (uint loc, glm::vec3* vec)     { glUniform3fv      (loc, 1,           &((*vec)[0]   ));                }
void SHDEX_uvec2 (uint loc, glm::vec2* vec)     { glUniform2fv      (loc, 1,           &((*vec)[0]   ));                }

//  - --- - --- - --- - --- -

void SHDEX_baseTransformUpdate
(glm::mat4* model)                              { SHDEX_umat4(__program->uniforms[SIN_MODEL_U], model);                 }

void SHDEX_fullTransformUpdate
(glm::mat4* model, glm::mat3* nmat)             { SHDEX_umat4(__program->uniforms[SIN_MODEL_U], model);
                                                  SHDEX_umat3(__program->uniforms[SIN_NMAT_U ], nmat );                 }

//  - --- - --- - --- - --- -

