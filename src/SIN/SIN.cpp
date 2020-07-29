#include "SIN.h"
#include <stdio.h>

static unsigned char INITFLAGS = 0;

//  - --- - --- - --- - --- -

void SIN_INIT(unsigned char flags)              { 

    SIN_GVAO_init();

    SIN_texbucket_init();
    SIN_shdbucket_init();
    SIN_matbucket_init();
    SIN_meshbucket_init();

    INITFLAGS = flags;

                                                                                                                        }

void SIN_END()                                  { 

    SIN_meshbucket_end();
    SIN_matbucket_end();
    SIN_shdbucket_end();
    SIN_texbucket_end();

    SIN_GVAO_end();

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

