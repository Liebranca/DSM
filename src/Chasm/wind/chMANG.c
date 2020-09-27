#include "chMANG.h"

#include "lyutils/ZJC_Evil.h"
#include "spatial/ZJC_Time.h"

#include <stdio.h>

//  - --- - --- - --- - --- -

Uint8        CH_MAXWINS   = 8;
chMG         chmang       = {0};

static chWH* curwin       = NULL;

static uint w_width       = 640;
static uint w_height      = 480;

static int  look_useMouse =  0;
static int  main_joy      = -1;

//  - --- - --- - --- - --- -

int   chmang_winOpen    ()                      { return !chmang_curwin()->isClosed;                                    }
void  chmang_frameStart ()                      { clock_frameStart(); pollEvents(curwin);                               }
void  chmang_fBy        ()                      { fBy = clock_fBy(1.0f);                                                }
void  chmang_frameEnd   ()                      { clock_frameEnd(); chmang_fBy(); swapBuffers(curwin);       }

void  chmang_wait       ()                      { if(clock_getSleep()) { SDL_Delay(clock_getSleepTime()); }             }

uint  getWinWidth       ()                      { return w_width;                                                       }
uint  getWinHeight      ()                      { return w_height;                                                      }

//  - --- - --- - --- - --- -

int chmang_init         (cchar* title,
                         int fullscreen)        {

    SDL_Init           (SDL_INIT_EVERYTHING         );

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE,           3);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,         3);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,          2);

    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE,       16);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,        16);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,       1);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

    chmang.curwin   = 0;
    chmang.openwins = 1;

    SDL_DisplayMode DM;
    SDL_GetCurrentDisplayMode(0, &DM);

    if(fullscreen) { w_width  = DM.w; w_height = DM.h; }

    chmang.wins     = (chWH*) evil_malloc(8, sizeof(chWH));

    *chmang.wins    = build_whandle(title, w_width, w_height);
    chmang.context  = SDL_GL_CreateContext(chmang_curwin()->window);

    curwin          = chmang_curwin();

    if(fullscreen) { SDL_SetWindowFullscreen(curwin->window, SDL_WINDOW_FULLSCREEN_DESKTOP); }

    glewExperimental = GL_TRUE;
    GLenum status = glewInit();
    if (status != GLEW_OK)                      { fprintf(stderr, "GLEW failed it's own init; something's wrong...\n");
                                                  chmang_end(); return FATAL;                                           }

    if (!glewIsSupported("GL_VERSION_3_2"))     { printf("This application requires OpenGL v3.2\n");                    }

    SDL_GL_SetSwapInterval(1);

    int numjoys = SDL_NumJoysticks();

    if(numjoys > 0)
    {
        main_joy = 0;
        for(int i = 0; i < numjoys; i++)
        {
            SDL_JoystickEventState(SDL_ENABLE);
            openJoy(curwin, i); curwin->joys[i].sens = 5.0f;
        }
    }

    glEnable   (GL_DEPTH_TEST                       );
    glEnable   (GL_CULL_FACE                        );
    glCullFace (GL_BACK                             );
    glEnable   (GL_BLEND                            );
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    clock_init(                                     );

    return 0;                                                                                                           }

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

void  getKeyPress       (short* keys)           { *keys =   (curwin->keys & *keys)
                                                          | (curwin->joys[main_joy].buttons & *keys);                   }

void  getMotionKeys     (fpair* moveaxis)       {

    moveaxis->x = 0.0f;
    moveaxis->y = 0.0f;

    if(main_joy >= 0)
    {
        if(chJOY_RSTICK_ISACTI(
          &curwin->joys[main_joy]))             { moveaxis->x -=  curwin->joys[main_joy].__CH_JOYMOVEAX__.x;
                                                  moveaxis->y -=  curwin->joys[main_joy].__CH_JOYMOVEAX__.y; return;    }
    }

    if(curwin->keys & (0x2000 + 0x8000))
    {
        moveaxis->x                             = 1.0f;

        if(curwin->keys & 0x8000)               { moveaxis->x  = -1.0f;                                                 }
    }

    if(curwin->keys & (0x1000 + 0x4000))
    {
        moveaxis->y                             = 1;

        if(curwin->keys & 0x4000)               { moveaxis->y  = -1.0f;                                                 }
    }
}

int   mouseActive       ()                      {

    look_useMouse = chRAT_ISACTI(&curwin->mouse);

    if      (look_useMouse)                     { return 1;                                                             }
    else if (main_joy >= 0)                     { return chJOY_LSTICK_ISACTI(&curwin->joys[main_joy]);                  }
    return 0;                                                                                                           }

void  onMouseMotion     ()                      { chRAT_STOP(&curwin->mouse);                                           }
fpair getMouseMotion    ()                      { if(look_useMouse) { return chRAT_GETMOTION(&curwin->mouse, fBy);      }
                                                  fpair lookax = curwin->joys[main_joy].__CH_JOYLOOKAX__;

                                                  lookax.x *= fBy; lookax.y *= fBy; return lookax;                      }

