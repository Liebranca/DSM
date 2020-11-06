#include "SIN_Canvas.h"
#include "../SIN_Constants.h"
#include "../types/SIN_Shader.h"
#include "../types/SIN_MeshBatch.h"

#include "GL/glew.h"

//  - --- - --- - --- - --- -

static uint   depthBufferLoc  = 0;
static uint   depthTexLoc     = 0;
static uint   depthProgram    = 0;
static uint   depthShader [2] = { 0, 0 };
static uint   depthUniform[2] = { 0, 0 };

static uint   canvasVAO       = 0;
static uint   canvasVBO       = 0;
static uint   canvasProgram   = 0;
static uint   canvasShader[2] = { 0, 0 };

//  - --- - --- - --- - --- -

void SIN_DepthBuffer_init
            (const shaderParams* shader)        {

    glGenFramebuffers     (1, &depthBufferLoc                                                       );
    glGenTextures         (1, &depthTexLoc                                                          );
    glActiveTexture       (GL_TEXTURE0                                                              );

    glBindTexture         (GL_TEXTURE_2D_ARRAY, depthTexLoc                                         );
    glTexStorage3D        (GL_TEXTURE_2D_ARRAY, 1, GL_DEPTH_COMPONENT16,
                           SIN_SHADOWSIZE, SIN_SHADOWSIZE, 64                                       );

    glTexParameteri       (GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BASE_LEVEL, 0                            );
    glTexParameteri       (GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL,  0                            );
    glTexParameteri       (GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST                   );
    glTexParameteri       (GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST                   );
    glTexParameteri       (GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER               );
    glTexParameteri       (GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER               );
    glTexParameteri       (GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE    );

    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f                                                  };
    glTexParameterfv      (GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, borderColor                );

    glBindFramebuffer     (GL_FRAMEBUFFER, depthBufferLoc                                           );

    glFramebufferTexture  (GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexLoc, 0                      );
    glDrawBuffer          (GL_NONE                                                                  );
    glReadBuffer          (GL_NONE                                                                  );

    depthProgram = 
           glCreateProgram(                                                                         );

    depthShader[0] =
    SIN_createShader      (shader->source_v, shader->num_vsources, GL_VERTEX_SHADER                 );
    depthShader[1] =
    SIN_createShader      (shader->source_f, shader->num_fsources, GL_FRAGMENT_SHADER               );

    for (uint i = 0;
         i < 2; i++)      { glAttachShader(depthProgram, depthShader[i]);                           }

    for(uchar attribLoc = 0; attribLoc < shader->num_attribs; attribLoc++)
    { glBindAttribLocation(depthProgram, 0, shader->attribs[attribLoc]);                            }

    glLinkProgram         (depthProgram                                                             );
    SIN_checkShaderError  (depthProgram, GL_LINK_STATUS, 1, "Shader linking failed"                 );
    glValidateProgram     (depthProgram                                                             );
    SIN_checkShaderError  (depthProgram, GL_VALIDATE_STATUS, 1, "Shader validation failed"          );

    for(uchar uniformLoc = 0; uniformLoc < shader->num_uniforms; uniformLoc++)
    { depthUniform[uniformLoc] = glGetUniformLocation(depthProgram, shader->uniforms[uniformLoc]);  }

    uint block_index =
    glGetUniformBlockIndex(depthProgram,        shader->ubos[0]                                     );
    glUniformBlockBinding (depthProgram,        block_index, 1                                      );

    glBindFramebuffer     (GL_FRAMEBUFFER,      0                                                   );
    glBindTexture         (GL_TEXTURE_2D_ARRAY, 0                                                   );                  }

void SIN_DepthBuffer_end()                      {

    for (uint i = 0; i < 2; i++)                { glDetachShader(depthProgram, depthShader[i]);
                                                  glDeleteShader(depthShader[i]              );                         }

    glDeleteProgram                             (depthProgram                                                           );
    glDeleteTextures                            (1, &depthTexLoc                                                        );
    glDeleteFramebuffers                        (1, &depthBufferLoc                                                     );
                                                                                                                        }

//  - --- - --- - --- - --- -

uint  SIN_DepthBuffer_getProgram()              { return depthProgram;                                                  }
uint* SIN_DepthBuffer_getUniform()              { return depthUniform;                                                  }

void SIN_DepthBuffer_bind       ()              { glViewport       (0, 0, SIN_SHADOWSIZE, SIN_SHADOWSIZE  );
                                                  glBindFramebuffer(GL_FRAMEBUFFER, depthBufferLoc        );
                                                  glClear          (GL_DEPTH_BUFFER_BIT );                              }

void  SIN_DepthMap_bind         ()              { glActiveTexture(GL_TEXTURE0                     );
                                                  glBindTexture  (GL_TEXTURE_2D, depthTexLoc      );                    }

//  - --- - --- - --- - --- -

void SIN_Canvas_init
        (const shaderParams* shader)            {

    float quadVertices[] =      { -1.0f,  1.0f,  0.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f,  1.0f, -1.0f,
                                   1.0f,  0.0f, -1.0f,  1.0f,  0.0f,  1.0f,  1.0f, -1.0f,  1.0f,  0.0f,
                                   1.0f,  1.0f,  1.0f,  1.0f                                                };

    glGenVertexArrays           (1, &canvasVAO                                                              );
    glGenBuffers                (1, &canvasVBO                                                              );
    glBindVertexArray           (canvasVAO                                                                  );

    glBindBuffer                (GL_ARRAY_BUFFER, canvasVBO                                                 );
    glBufferData                (GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW       );

    glEnableVertexAttribArray   (0                                                                          );
    glVertexAttribPointer       (0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*) 0                     );
    glEnableVertexAttribArray   (1                                                                          );
    glVertexAttribPointer       (1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*) (2 * sizeof(float))   );

    canvasProgram = 
           glCreateProgram      (                                                                           );

    canvasShader[0] =
    SIN_createShader            (shader->source_v, shader->num_vsources, GL_VERTEX_SHADER                   );
    canvasShader[1] =
    SIN_createShader            (shader->source_f, shader->num_fsources, GL_FRAGMENT_SHADER                 );

    for (uint i = 0;
         i < 2; i++)            { glAttachShader(canvasProgram, canvasShader[i]);                           }

    for(uchar attribLoc = 0; attribLoc < shader->num_attribs; attribLoc++)
    { glBindAttribLocation(canvasProgram, 0, shader->attribs[attribLoc]);                                   }

    glLinkProgram               (canvasProgram                                                              );
    SIN_checkShaderError        (canvasProgram, GL_LINK_STATUS, 1, "Shader linking failed"                  );
    glValidateProgram           (canvasProgram                                                              );
    SIN_checkShaderError        (canvasProgram, GL_VALIDATE_STATUS, 1, "Shader validation failed"           );

    int surfLoc  =
        glGetUniformLocation    (canvasProgram, shader->samplers[0]                                         );

    glUniform1i                 (surfLoc,       0                                                           );          }

void SIN_Canvas_draw            (uint texloc)   {

    glUseProgram        (canvasProgram              );
    glActiveTexture     (GL_TEXTURE0                );
    glBindTexture       (GL_TEXTURE_2D_ARRAY, texloc);

    glBindVertexArray   (canvasVAO                  );
    glDrawArrays        (GL_TRIANGLES, 0, 6         );

    resbatch            (                           );
    SIN_resetShdloc     (                           );                                                                  }

void SIN_Canvas_end             ()              {

    for (uint i = 0; i < 2; i++)                { glDetachShader(canvasProgram, canvasShader[i]);
                                                  glDeleteShader(canvasShader[i]              );                        }

    glDeleteProgram                             (canvasProgram                                                          );
    glDeleteBuffers                             (1, &canvasVBO                                                          );
    glDeleteBuffers                             (1, &canvasVAO                                                          );
                                                                                                                        }

//  - --- - --- - --- - --- -

