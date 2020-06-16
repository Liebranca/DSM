#include "chWIN.h"

//  - --- - --- - --- - --- -

static int kFWD   = SDLK_w;
static int kBCK   = SDLK_s;
static int kLFT   = SDLK_a;
static int kRGT   = SDLK_d;
static int kSPC   = SDLK_SPACE;
static int kLCTRL = SDLK_LCTRL;
static int kUSE   = SDLK_e;

static int kESC   = SDLK_ESCAPE;

//  - --- - --- - --- - --- -

chWH        build_whandle(const char title[],
                          int width,
                          int height)           {

    chWH whandle;

    whandle.window = SDL_CreateWindow(title,
                                      SDL_WINDOWPOS_CENTERED,
                                      SDL_WINDOWPOS_CENTERED,
                                      width,
                                      height,
                                      SDL_WINDOW_OPENGL      );

    whandle.width       = width;
    whandle.height      = height;

    whandle.isClosed    = 0;
    whandle.pkeys       = 0;
    whandle.helkeys     = 0;

    whandle.mouseIgnore = 0;
    whandle.mousePos_x  = 0;
    whandle.mousePos_y  = 0;

    whandle.mouseRel_x  = 0.0f;
    whandle.mouseRel_y  = 0.0f;
    whandle.mouseSens   = 2.0f;

    return whandle;                                                                                                     };

int         del_whandle  (chWH* whandle)        { SDL_DestroyWindow(whandle->window); return 0;                         };

//  - --- - --- - --- - --- -

void mouseWrap           (chWH* whandle)        {

    SDL_WarpMouseInWindow(whandle->window,
                          whandle->height * 0.5,
                          whandle->width  * 0.5);

    whandle->mouseIgnore = 1;
    whandle->mouseRel_x  = 0.0f;
    whandle->mouseRel_y  = 0.0f;                                                                                        }

void        swapBuffers  (chWH* whandle)        { SDL_GL_SwapWindow(whandle->window);                                   }
void        hideCursor   ()                     { SDL_ShowCursor(SDL_DISABLE);                                          }
void        showCursor   ()                     { SDL_ShowCursor(SDL_ENABLE);                                           }
int         getIsClosed  (chWH* whandle)        { return whandle->isClosed;                                             }

//  - --- - --- - --- - --- -

void        pollEvents   (chWH* whandle)        {

    int k;
    SDL_Event event;

    if (SDL_PollEvent(&event)) {
        switch (event.type) {

        case SDL_QUIT:    whandle->isClosed = 0;
            break;

        case SDL_KEYDOWN: k = event.key.keysym.sym;

            if      (k == kESC)                 { whandle->isClosed = 1;                                                }
            if      (k == kFWD &&
                   !(whandle->pkeys &  1))      { whandle->pkeys   += 1;                                                }
            else if (k == kBCK &&
                   !(whandle->pkeys &  2))      { whandle->pkeys   += 2;                                                }
            if      (k == kLFT &&
                   !(whandle->pkeys &  4))      { whandle->pkeys   += 4;                                                }
            else if (k == kRGT &&
                   !(whandle->pkeys &  8))      { whandle->pkeys   += 8;                                                }
            if      (k == kSPC &&
                   !(whandle->pkeys & 16))      { whandle->pkeys   += 16;                                               }
            else if (k == kLCTRL &&
                   !(whandle->pkeys & 32))      { whandle->pkeys   += 32;                                               }
            if      (k == kUSE &&
                   !(whandle->pkeys & 64))      { whandle->pkeys   += 64;                                               }

            break;

        case SDL_KEYUP:   k = event.key.keysym.sym;

            if      (k == kFWD)                 { whandle->pkeys   -= 1;                                                }
            else if (k == kBCK)                 { whandle->pkeys   -= 2;                                                }

            if      (k == kLFT)                 { whandle->pkeys   -= 4;                                                }
            else if (k == kRGT)                 { whandle->pkeys   -= 8;                                                }

            if      (k == kSPC)                 { whandle->pkeys   -= 16;                                               }
            else if (k == kLCTRL)               { whandle->pkeys   -= 32;                                               }

            if      (k == kUSE)                 { whandle->pkeys   -= 64;                                               }

            break;

        case SDL_MOUSEMOTION:

            whandle->mousePos_x = event.motion.x;
            whandle->mousePos_y = event.motion.y;

            if (whandle->mouseIgnore)           { whandle->mouseRel_x = (float)(event.motion.xrel * whandle->mouseSens);
                                                  whandle->mouseRel_y = (float)(event.motion.yrel * whandle->mouseSens);}

            break;
        }
    }
                                                                                                                        }