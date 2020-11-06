#include "chWIN.h"

#include "lymath/ZJC_GOPS.h"

#include <stdio.h>

//  - --- - --- - --- - --- -

static int    wrapMargin     = 32;

SDL_Joystick* joyinstances[CHASM_NUM_JOYS];

//  - --- - --- - --- - --- -

void        openJoy(chWH* whandle, int i)       {

    joyinstances [i]            = SDL_JoystickOpen      (i              );
    whandle->joys[i].instanceID = SDL_JoystickInstanceID(joyinstances[i]);
    whandle->joys[i].plugged    = 1;                                                                                    }

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

    whandle.size.x      = width;
    whandle.size.y      = height;

    whandle.hsize.x     = width  / 2;
    whandle.hsize.y     = height / 2;

    whandle.isClosed    = 0;
    whandle.isOnTop     = 1;
    whandle.keys        = 0;

    whandle.mouse.state = 0;
    whandle.mouse.abs.x = whandle.hsize.x;
    whandle.mouse.abs.y = whandle.hsize.y;

    whandle.mouse.rel.x = 0.0f;
    whandle.mouse.rel.y = 0.0f;
    whandle.mouse.sens  = 0.65f;

    whandle.id = SDL_GetWindowID(whandle.window);

    return whandle;                                                                                                     };

int         del_whandle  (chWH* whandle)        {

    for(uint i = 0; i < CHASM_NUM_JOYS; i++)
    {
        if(joyinstances[i])
        {
            if(SDL_JoystickGetAttached(joyinstances[i]))
            {
                SDL_JoystickClose(joyinstances[i]);
            }
        }
    }

    SDL_DestroyWindow(whandle->window);
    return 0;                                                                                                           };

//  - --- - --- - --- - --- -

void onMouseReset        (chWH* whandle)        { chRAT_RESET(&whandle->mouse, whandle->hsize);                         }
void mouseWrap           (chWH* whandle)        { onMouseReset(whandle); SDL_WarpMouseInWindow(whandle->window,
                                                                                               whandle->hsize.x,
                                                                                               whandle->hsize.y);       }

//  - --- - --- - --- - --- -

void        swapBuffers  (chWH* whandle)        { SDL_GL_SwapWindow(whandle->window);                                   }
void        hideCursor   ()                     { SDL_ShowCursor(SDL_DISABLE);                                          }
void        showCursor   ()                     { SDL_ShowCursor(SDL_ENABLE);                                           }
int         getIsClosed  (chWH* whandle)        { return whandle->isClosed;                                             }

int         cursorToWall (chWH* whandle)        { return chRAT_TOWALL(&whandle->mouse, whandle->size);                  }

//  - --- - --- - --- - --- -

void        pollEvents   (chWH* whandle)        {

    int k;
    SDL_Event event;

    SDL_Window* focused = SDL_GetMouseFocus();
    Uint32      wFlags  = SDL_GetWindowFlags(whandle->window);

    int wrap_cond = focused != whandle->window;
    if(wFlags & SDL_WINDOW_FULLSCREEN_DESKTOP) { wrap_cond = cursorToWall(whandle); }

    if(  (wrap_cond                    )
    &&  !(wFlags & SDL_WINDOW_MINIMIZED)
    &&   (whandle->isOnTop             ))       { mouseWrap(whandle);                                                   }

    while (SDL_PollEvent(&event))
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
                
                default: break;
                }

            }
        }

        case SDL_JOYBUTTONDOWN:
        {
            for(int i = 0; i < CHASM_NUM_JOYS; i++)
            {
                if(whandle->joys[i].plugged)
                {
                    if(event.jbutton.which == whandle->joys[i].instanceID)
                    { chJOY_SETBUTTON(&whandle->joys[i], event.jbutton.button); }
                }
            }

            break;
        }

        case SDL_JOYAXISMOTION:
        {
            for(int i = 0; i < CHASM_NUM_JOYS; i++)
            {
                if(whandle->joys[i].plugged)
                {
                    if(event.jaxis.which == whandle->joys[i].instanceID)
                    { chJOY_SETAXES(&whandle->joys[i], event.jaxis); }
                }
            }

            break;
        }

        case SDL_KEYDOWN:
        {
            k = event.key.keysym.sym;

            if(k == SDLK_ESCAPE)                { whandle->isClosed = 1; break;                                         }

            for(uchar i = 0; i < 16; i++)
            {
                if(k == CH_KEYB_KCODES[i])      { chKEYB_SETKEY(&whandle->keys, i);                                     }
            }

            break;
        }

        case SDL_KEYUP:
        {
            k = event.key.keysym.sym;

            for(uchar i = 0; i < 16; i++)
            {
                if(k == CH_KEYB_KCODES[i])      { chKEYB_CLRKEY(&whandle->keys, i);                                     }
            }

            break;
        }

        case SDL_MOUSEMOTION:
        {

            if(whandle->isOnTop)
            {
                if(!chRAT_ISIGGY(
                   &whandle->mouse))            { chRAT_RUN(&whandle->mouse, &event.motion);                            }

                else                            { chRAT_UNIGGY(&whandle->mouse);                                        }
            }

            break;

        }
        default: break;
        }

    }
                                                                                                                        }
