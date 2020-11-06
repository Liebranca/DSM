#include "SIN_Ambient.h"
#include "GL/glew.h"

//  - --- - --- - --- - --- -

static uint          ambientBuff             = 0;
static SIN_UAMBIENT* SIN_Ambient             = NULL;
static char          SIN_Ambient_UpdateQueue = 0;

//  - --- - --- - --- - --- -

void SIN_Ambient_init       ()                  {

    SIN_Ambient =    new SIN_UAMBIENT();

    glGenBuffers    (1, &ambientBuff                                               );
    glBindBuffer    (GL_UNIFORM_BUFFER, ambientBuff                                );
    glBufferData    (GL_UNIFORM_BUFFER, sizeof(SIN_UAMBIENT), NULL, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, ambientBuff                             );
    glBindBuffer    (GL_UNIFORM_BUFFER, 0                                          );                                   }

void SIN_Ambient_end        ()                  { glDeleteBuffers(1, &ambientBuff); delete SIN_Ambient;                 }

void SIN_Ambient_update     ()                  {

    glBindBuffer   (GL_UNIFORM_BUFFER, ambientBuff                                 );
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(SIN_UAMBIENT), (void*) SIN_Ambient);
    glBindBuffer   (GL_UNIFORM_BUFFER, 0                                           );

    SIN_Ambient_UpdateQueue = 0;                                                                                        }

//  - --- - --- - --- - --- -

void SIN_Ambient_setCam(glm::mat4* ViewProjection,
                        glm::vec3* CamFwd)      { SIN_Ambient->ViewProjection = *ViewProjection;
                                                  SIN_Ambient->CamFwd         = glm::vec4(*CamFwd, 1);
                                                  SIN_Ambient_UpdateQueue     = 1;                                      }

void SIN_Ambient_setSky(glm::vec3* color,
                        float intensity)        { SIN_Ambient->Color          = glm::vec4(*color, intensity);
                                                  SIN_Ambient_UpdateQueue     = 1;                                      }

void SIN_Ambient_updateCheck()                  { if(SIN_Ambient_UpdateQueue) { SIN_Ambient_update(); }                 }

//  - --- - --- - --- - --- -
