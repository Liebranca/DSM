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

void SIN_DepthBuffer_init(cchar** vert_source,
                          cchar** frag_source)  {

    glGenFramebuffers     (1, &depthBufferLoc                                                );
    glGenTextures         (1, &depthTexLoc                                                   );

    glBindTexture         (GL_TEXTURE_2D, depthTexLoc                                        );
    glTexImage2D          (GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16,
                           SIN_SHADOWSIZE_X, SIN_SHADOWSIZE_Y, 0, GL_DEPTH_COMPONENT,
                           GL_FLOAT, 0                                                       );

    glTexParameteri       (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST                  );
    glTexParameteri       (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST                  );
    glTexParameteri       (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER              );
    glTexParameteri       (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER              );

    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f                                           };
    glTexParameterfv      (GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor               );

    glBindFramebuffer     (GL_FRAMEBUFFER, depthBufferLoc                                    );

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexLoc, 0);
    glDrawBuffer          (GL_NONE                                                           );
    glReadBuffer          (GL_NONE                                                           );

    depthProgram = 
           glCreateProgram(                                                                  );

    depthShader[0] =
              createShader(vert_source, GL_VERTEX_SHADER                                     );
    depthShader[1] =
              createShader(frag_source, GL_FRAGMENT_SHADER                                   );

    for (uint i = 0;
         i < 2; i++)      { glAttachShader(depthProgram, depthShader[i]);                    }

    glBindAttribLocation  (depthProgram, 0, "Position"                                       );

    glLinkProgram         (depthProgram                                                      );
    checkShaderError      (depthProgram, GL_LINK_STATUS, 1, "Shader linking failed"          );
    glValidateProgram     (depthProgram                                                      );
    checkShaderError      (depthProgram, GL_VALIDATE_STATUS, 1, "Shader validation failed"   );

    depthUniform[0] =
      glGetUniformLocation(depthProgram, "Model"                                             );
    depthUniform[1] = 
      glGetUniformLocation(depthProgram, "ViewProjection"                                    );

    glBindFramebuffer     (GL_FRAMEBUFFER, 0                                                 );                         }

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

void SIN_DepthBuffer_bind       ()              { glViewport       (0, 0, SIN_SHADOWSIZE_X, SIN_SHADOWSIZE_Y  );
                                                  glBindFramebuffer(GL_FRAMEBUFFER, depthBufferLoc            );
                                                  glClear          (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );        }

void  SIN_DepthMap_bind         ()              { glActiveTexture(GL_TEXTURE0 + SIN_TEXID_BASE + 4);
                                                  glBindTexture  (GL_TEXTURE_2D, depthTexLoc      );                    }

//  - --- - --- - --- - --- -

void SIN_Canvas_init(cchar** vert_source,
                     cchar** frag_source)       {

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
              createShader      (vert_source, GL_VERTEX_SHADER                                              );
    canvasShader[1] =
              createShader      (frag_source, GL_FRAGMENT_SHADER                                            );

    for (uint i = 0;
         i < 2; i++)            { glAttachShader(canvasProgram, canvasShader[i]);                           }

    glBindAttribLocation        (canvasProgram, 0, "Position"                                               );
    glBindAttribLocation        (canvasProgram, 1, "UV"                                                     );

    glLinkProgram               (canvasProgram                                                              );
    checkShaderError            (canvasProgram, GL_LINK_STATUS, 1, "Shader linking failed"                  );
    glValidateProgram           (canvasProgram                                                              );
    checkShaderError            (canvasProgram, GL_VALIDATE_STATUS, 1, "Shader validation failed"           );

    int diffuseLoc  =
        glGetUniformLocation    (canvasProgram, "DiffuseMap"                                                );

    glUniform1i                 (diffuseLoc,    0                                                           );

          }

void SIN_Canvas_draw            ()              {

    glUseProgram        (canvasProgram              );
    glActiveTexture     (GL_TEXTURE0                );
    glBindTexture       (GL_TEXTURE_2D, depthTexLoc );

    glBindVertexArray   (canvasVAO                  );
    glDrawArrays        (GL_TRIANGLES, 0, 6         );

    resbatch            (                           );
    shader_reset_loc    (                           );                                                                  }

void SIN_Canvas_end             ()              {

    for (uint i = 0; i < 2; i++)                { glDetachShader(canvasProgram, canvasShader[i]);
                                                  glDeleteShader(canvasShader[i]              );                        }

    glDeleteProgram                             (canvasProgram                                                          );
    glDeleteBuffers                             (1, &canvasVBO                                                          );
    glDeleteBuffers                             (1, &canvasVAO                                                          );
                                                                                                                        }

//  - --- - --- - --- - --- -

