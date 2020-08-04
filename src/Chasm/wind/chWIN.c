#include "chWIN.h"

#include "lymath/ZJC_GOPS.h"

#include <stdio.h>

//  - --- - --- - --- - --- -

static int kFWD       = SDLK_w;
static int kBCK       = SDLK_s;
static int kLFT       = SDLK_a;
static int kRGT       = SDLK_d;
static int kSPC       = SDLK_SPACE;
static int kLCTRL     = SDLK_LCTRL;
static int kUSE       = SDLK_e;

static int kESC       = SDLK_ESCAPE;

static int wrapMargin = 32;

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
                                      SDL_WINDOW_OPENGL | SDL_WINDOW_BORDERLESS );

    whandle.width           = width;
    whandle.height          = height;

    whandle.h_width         = width  / 2;
    whandle.h_height        = height / 2;

    whandle.q_width         = width  / 4;
    whandle.q_height        = height / 4;

    whandle.isClosed        = 0;
    whandle.isOnTop         = 1;
    whandle.pkeys           = 0;
    whandle.helkeys         = 0;

    whandle.mouseIgnore     = 0;
    whandle.mouseActive     = 0;
    whandle.mousePos_x      = whandle.h_width;
    whandle.mousePos_y      = whandle.h_height;

    whandle.mouseRel_x      = 0.0f;
    whandle.mouseRel_y      = 0.0f;
    whandle.mouseSens       = 2.0f;

    whandle.id = SDL_GetWindowID(whandle.window);

    return whandle;                                                                                                     };

int         del_whandle  (chWH* whandle)        { SDL_DestroyWindow(whandle->window); return 0;                         };

//  - --- - --- - --- - --- -

void onMouseReset        (chWH* whandle)        { whandle->mouseIgnore     = 0;
                                                  whandle->mouseRel_x      = 0.0f;
                                                  whandle->mouseRel_y      = 0.0f;                                      }

void onMouseMotion       (chWH* whandle)        { whandle->mouseActive = 0;
                                                  whandle->mouseRel_x  = 0.0f;
                                                  whandle->mouseRel_y  = 0.0f;                                          }

void mouseWrap           (chWH* whandle)        {

    onMouseReset(whandle);

    int new_x = whandle->mousePos_x;
    int new_y = whandle->mousePos_y;

    if      ( whandle->mousePos_x
            < whandle->h_width
            - whandle->q_width       )          { new_x = whandle->width - wrapMargin;                                  }

    else if ( whandle->mousePos_x
            > whandle->h_width
            + whandle->q_width       )          { new_x = wrapMargin;                                                   }

    if      ( whandle->mousePos_y
            < whandle->h_height 
            - whandle->q_height      )          { new_y = whandle->height - wrapMargin;                                 }

    else if ( whandle->mousePos_y
            > whandle->h_height
            + whandle->q_height      )          { new_y = wrapMargin;                                                   }

    SDL_WarpMouseInWindow(whandle->window,
                          new_x,
                          new_y);                                                                                       }

//  - --- - --- - --- - --- -

void        swapBuffers  (chWH* whandle)        { SDL_GL_SwapWindow(whandle->window);                                   }
void        hideCursor   ()                     { SDL_ShowCursor(SDL_DISABLE);                                          }
void        showCursor   ()                     { SDL_ShowCursor(SDL_ENABLE);                                           }
int         getIsClosed  (chWH* whandle)        { return whandle->isClosed;                                             }

//  - --- - --- - --- - --- -

void        pollEvents   (chWH* whandle)        {

    int k;
    SDL_Event event;

    SDL_Window* focused = SDL_GetMouseFocus();
    Uint32      wFlags  = SDL_GetWindowFlags(whandle->window);

    if (   (focused != whandle->window)
       && !(wFlags & (SDL_WINDOW_MINIMIZED))
       &&  (whandle->isOnTop)              )    { mouseWrap(whandle);                                                   }

    if (SDL_PollEvent(&event))
    {
        switch (event.type)
        {

        case SDL_QUIT:
        {
            whandle->isClosed = 0;
            break;
        }

        case SDL_WINDOWEVENT:
        {
            if (event.window.windowID
                == whandle->id)
            {

                switch (event.window.event)
                {

                case SDL_WINDOWEVENT_FOCUS_GAINED:
                {
                    hideCursor();
                    whandle->isOnTop = 1;
                    break;
                }
                case SDL_WINDOWEVENT_FOCUS_LOST:
                {
                    showCursor();
                    whandle->isOnTop = 0;
                    break;
                }

                }

            }
        }

        case SDL_KEYDOWN:
        {
            k = event.key.keysym.sym;

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
        }

        case SDL_KEYUP:
        {
            k = event.key.keysym.sym;

            if      (k == kFWD)                 { whandle->pkeys   -= 1;                                                }
            else if (k == kBCK)                 { whandle->pkeys   -= 2;                                                }

            if      (k == kLFT)                 { whandle->pkeys   -= 4;                                                }
            else if (k == kRGT)                 { whandle->pkeys   -= 8;                                                }

            if      (k == kSPC)                 { whandle->pkeys   -= 16;                                               }
            else if (k == kLCTRL)               { whandle->pkeys   -= 32;                                               }

            if      (k == kUSE)                 { whandle->pkeys   -= 64;                                               }

            break;
        }

        case SDL_MOUSEMOTION:
        {

            if(whandle->isOnTop)
            {
                if (!whandle->mouseIgnore)      { whandle->mousePos_x = event.motion.x;
                                                  whandle->mousePos_y = event.motion.y;
                                                  whandle->mouseRel_x = (float)(event.motion.xrel * whandle->mouseSens);
                                                  whandle->mouseRel_y = (float)(event.motion.yrel * whandle->mouseSens);

                                                  whandle->mouseActive = 1;                                             }

                else                            { whandle->mouseIgnore = 0;                                             }
            }

            break;
        }

        default: break;

        }

    }
                                                                                                                        }
