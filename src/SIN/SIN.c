#include "SIN.H"

#include "types/SIN_Mesh.h"
#include "types/SIN_Material.h"

unsigned char INITFLAGS = 0;

//  - --- - --- - --- - --- -

void SIN_INIT(unsigned char flags)              { 

    SIN_matbucket_init();
    SIN_meshbucket_init();

    INITFLAGS = flags;

                                                                                                                        }

void SIN_END()                                  { 

    SIN_matbucket_end();
    SIN_meshbucket_end();
                                                                                                                        }

//  - --- - --- - --- - --- -

void SIN_SWIPESCREEN(float color[4],
                     float mult)                {

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glClearColor(color[0] * mult,
                 color[1] * mult,
                 color[2] * mult,
                 color[3] * mult);

    glClear(GL_COLOR_BUFFER_BIT); // GL_DEPTH_BUFFER_BIT
                                                                                                                        }

