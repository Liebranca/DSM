#include "SIN_Shader_EX.h"
#include "SIN_Shader.h"
#include "GL/glew.h"

#include <string>

//  - --- - --- - --- - --- -

void shader_update_model  (glm::mat4* model)    { glUniformMatrix4fv(__program->uniforms[SIN_TRANSFORM_U],
                                                                     1, GL_FALSE, &((*model)[0][0]));                   }

void shader_update_nmat   (glm::mat3* nmat)     { glUniformMatrix3fv(__program->uniforms[SIN_NORMAL_U],
                                                                     1, GL_FALSE, &((*nmat)[0][0]));                    }

void shader_update_camera (glm::mat4* viewprojection,
                           glm::vec3* camfwd,
                           glm::vec3* campos)   {

    glUniformMatrix4fv(__program->uniforms[SIN_PROJECTION_U], 1, GL_FALSE, &((*viewprojection)[0][0]));
    glUniform3fv(__program->uniforms[SIN_CAMFWD_U], 1, &((*camfwd)[0]));
    glUniform3fv(__program->uniforms[SIN_CAMPOS_U], 1, &((*campos)[0]));                                                }

void shader_update_viewproj
                (glm::mat4* viewprojection)     { glUniformMatrix4fv(__program->uniforms[SIN_PROJECTION_U], 1,
                                                  GL_FALSE, &((*viewprojection)[0][0]));                                }

void shader_update_lights
                (SIN_LIGHT* lights,
                 uint       num_lights)         {

    glUniform1ui(__program->uniforms[SIN_NUMLIGHTS_U], num_lights);

    for(uint i = 0; i < num_lights; i++)
    {
        SIN_LIGHT* light = lights + i;
        std::string li   =  std::to_string(i);

        uint loc         = glGetUniformLocation(__program->location, ("Lights["+li+"].mat").c_str()     );
                           glUniformMatrix4fv  (loc, 1, GL_FALSE, &(light->mat[0][0])                   );

        loc              = glGetUniformLocation(__program->location, ("Lights["+li+"].position").c_str());
                           glUniform3fv        (loc, 1, &(light->position[0])                           );

        loc              = glGetUniformLocation(__program->location, ("Lights["+li+"].dirn").c_str()    );
                           glUniform3fv        (loc, 1, &(light->dirn[0])                               );

        loc              = glGetUniformLocation(__program->location, ("Lights["+li+"].color").c_str()   );
                           glUniform4fv        (loc, 1, &(light->color[0])                              );
    }
                                                                                                                        }

void shader_update_ambient(glm::vec4* ambient_color)
                                                { glUniform4fv(__program->uniforms[SIN_AMBIENT_U],
                                                               1, &((*ambient_color)[0]) );                             }

void shader_update_campos(glm::vec3* campos)
                                                { glUniform3fv(__program->uniforms[SIN_CAMPOS_U], 1, &((*campos)[0]));  }

//  - --- - --- - --- - --- -

