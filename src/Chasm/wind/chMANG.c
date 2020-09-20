#include "chMANG.h"

#include "lyutils/ZJC_Evil.h"
#include "spatial/ZJC_Time.h"

#include <stdio.h>

//  - --- - --- - --- - --- -

Uint8        CH_MAXWINS = 8;
chMG         chmang     = {0};

static chWH* curwin     = NULL;

int  chmang_winOpen     ()                      { return !chmang_curwin()->isClosed;                                    }
void chmang_frameStart  ()                      { clock_frameStart(); fBy = clock_fBy(1.0f);
                                                  pollEvents(chmang_curwin());                                          }

void  chmang_frameEnd   ()                      { swapBuffers(chmang_curwin());
                                                  clock_frameEnd();                                                     }

void  chmang_wait       ()                      {

    if(clock_getSleep())                        { SDL_Delay( (Uint32)clock_getDelta() );                                }
                                                                                                                        }

//  - --- - --- - --- - --- -

int chmang_init         (cchar* title,
                         uint width,
                         uint height)           {

    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE,     8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,   8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,    8);

    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,  32);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    chmang.curwin   = 0;
    chmang.openwins = 1;

    chmang.wins     = (chWH*) evil_malloc(8, sizeof(chWH));

    *chmang.wins    = build_whandle(title, width, height);
    chmang.context  = SDL_GL_CreateContext(chmang_curwin()->window);

    glewExperimental = GL_TRUE;
    GLenum status = glewInit();
    if (status != GLEW_OK)                      { fprintf(stderr, "GLEW failed it's own init; something's wrong...\n");
                                                  chmang_end(); return FATAL;                                           }

    if (!glewIsSupported("GL_VERSION_3_2"))     { printf("This application requires OpenGL v3.2\n");                    }

    SDL_GL_SetSwapInterval(0);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    clock_init();

    curwin = chmang_curwin();

    return 0;

}

//  - --- - --- - --- - --- -

int  chmang_end         ()                      {

    SDL_GL_DeleteContext(chmang.context);

    for(Uint8 i = 0;
        i < chmang.openwins; i ++)              { del_whandle(chmang.wins + i);                                         }

    WARD_EVIL_MFREE(chmang.wins);
    SDL_Quit();

    return 0;                                                                                                           }

//  - --- - --- - --- - --- -

int  chmang_buildwin    (cchar* title,
                         uint  width,
                         uint  height)          {

    if(chmang.openwins < CH_MAXWINS)            { *(chmang.wins + chmang.openwins) = build_whandle(title,
                                                                                                   width,
                                                                                                   height);
                                                  chmang.curwin = chmang.openwins;
                                                  chmang.openwins++;                                                    }

    else                                        { fprintf(stderr,
                                                  "CHMANG can't open more than %u windows!\n", CH_MAXWINS);

                                                  return FATAL;                                                         }
                                                                                                                        }

//  - --- - --- - --- - --- -

chWH* chmang_curwin     ()                      { return chmang.wins + chmang.curwin;                                   }
chWH* chmang_getwin     (Uint8 i)               {

    if(i > CH_MAXWINS && i < chmang.openwins)   { return chmang.wins + i;                                               }
    return chmang_curwin();                                                                                             }

//  - --- - --- - --- - --- -

int   getKeyPress       (int keys)              { return curwin->pkeys & keys;                                          }

