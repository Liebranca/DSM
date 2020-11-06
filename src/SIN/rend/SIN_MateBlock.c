#include "SIN_MateBlock.h"
#include "lyutils/ZJC_Evil.h"

#include "GL/glew.h"

//  - --- - --- - --- - --- -

static uint           materialBuff              = 0;
static SIN_UMATERIAL* SIN_MaterialBlock         = NULL;
static char           SIN_MateBlock_UpdateQueue = 0;

//  - --- - --- - --- - --- -

void SIN_MateBlock_init()                       {

    SIN_MaterialBlock = (SIN_UMATERIAL*) evil_malloc(1, sizeof(SIN_UMATERIAL));

    glGenBuffers    (1, &materialBuff                                                 );
    glBindBuffer    (GL_UNIFORM_BUFFER, materialBuff                                  );
    glBufferData    (GL_UNIFORM_BUFFER, sizeof(SIN_MaterialBlock), NULL, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, materialBuff                               );
    glBindBuffer    (GL_UNIFORM_BUFFER, 0                                             );                                }

void SIN_MateBlock_end ()                       { glDeleteBuffers(1, &materialBuff); WARD_EVIL_MFREE(SIN_MaterialBlock);}

void SIN_MateBlock_update()                     {

    glBindBuffer   (GL_UNIFORM_BUFFER, materialBuff                                       );
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(SIN_UMATERIAL), (void*) SIN_MaterialBlock);
    glBindBuffer   (GL_UNIFORM_BUFFER, 0                                                  );

    SIN_MateBlock_UpdateQueue = 0;                                                                                      }

//  - --- - --- - --- - --- -

SIN_MATDATA* SIN_MateBlock_getSlot(uint slotnum){ return SIN_MaterialBlock->materials + slotnum;                        }

void SIN_MateBlock_requestUpdate()              { SIN_MateBlock_UpdateQueue = 1;                                        }
void SIN_MateBlock_updateCheck()                { if(SIN_MateBlock_UpdateQueue) { SIN_MateBlock_update(); }             }

//  - --- - --- - --- - --- -
