#include "SIN_Lighting.h"

#include "GL/glew.h"

//  - --- - --- - --- - --- -

static uint         lightingBuff             = 0;
static SIN_ULIGHTS* SIN_Lights               = NULL;
static char         SIN_Lighting_UpdateQueue = 0;

//  - --- - --- - --- - --- -

void SIN_Lighting_init()                        {

    SIN_Lights =    new SIN_ULIGHTS();

    glGenBuffers    (1, &lightingBuff                                             );
    glBindBuffer    (GL_UNIFORM_BUFFER, lightingBuff                              );
    glBufferData    (GL_UNIFORM_BUFFER, sizeof(SIN_ULIGHTS), NULL, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, lightingBuff                           );
    glBindBuffer    (GL_UNIFORM_BUFFER, 0                                         );                                    }

void SIN_Lighting_end ()                        { glDeleteBuffers(1, &lightingBuff); delete SIN_Lights;                 }

void SIN_Lighting_update()                      {

    glBindBuffer   (GL_UNIFORM_BUFFER, lightingBuff                              );
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(SIN_ULIGHTS), (void*) SIN_Lights);
    glBindBuffer   (GL_UNIFORM_BUFFER, 0                                         );

    SIN_Lighting_UpdateQueue = 0;                                                                                       }

//  - --- - --- - --- - --- -

void SIN_Lighting_setDir(SIN_LAMP* lamp,
                         uchar lamp_index)      { SIN_Lights->lamps[lamp_index] = *lamp; SIN_Lighting_UpdateQueue = 1;  }

void SIN_Lighting_setNum(uint num_lamps)        { SIN_Lights->num_lamps = num_lamps; SIN_Lighting_UpdateQueue = 1;      }
void SIN_Lighting_updateCheck()                 { if(SIN_Lighting_UpdateQueue) { SIN_Lighting_update(); }               }

//  - --- - --- - --- - --- -

