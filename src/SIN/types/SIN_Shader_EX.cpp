#include "SIN_Shader_EX.h"
#include "SIN_Shader.h"
#include "GL/glew.h"

Program* __program   = NULL;
ushort   __curShdLoc = 0;

//  - --- - --- - --- - --- -

void shader_setProgram (ushort loc)             { __program = SIN_shdbucket_get(loc); __curShdLoc = loc+1;              }
void shader_useProgram ()                       { glUseProgram(__program->location);                                    }

void shader_chkProgram (ushort loc)             {
    if (loc+1 != __curShdLoc)                   { shader_setProgram(loc); shader_useProgram(); }                        }

void shader_update_model  (glm::mat4& model)    { glUniformMatrix4fv(__program->uniforms[SIN_TRANSFORM_U],
                                                                     1, GL_FALSE, &model[0][0]);                        }

void shader_update_nmat   (glm::mat3& nmat)     { glUniformMatrix3fv(__program->uniforms[SIN_NORMAL_U],
                                                                     1, GL_FALSE, &nmat[0][0]);                         }

void shader_update_camera (glm::mat4& viewprojection,
                           glm::vec3& camfwd)   {

    glUniformMatrix4fv(__program->uniforms[SIN_PROJECTION_U], 1, GL_FALSE, &viewprojection[0][0]);
    glUniform3fv(__program->uniforms[SIN_CAMFWD_U], 1, &camfwd[0]);                                                     }

void shader_update_ambient(glm::vec4& ambient_color)
                                                { glUniform4fv(__program->uniforms[SIN_AMBIENT_U],
                                                               1, &ambient_color[0]);                                   }

//  - --- - --- - --- - --- -

