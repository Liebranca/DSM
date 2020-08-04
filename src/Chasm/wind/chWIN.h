#ifndef __CH_WIN_H__
#define __CH_WIN_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "SDL/SDL.h"

//  - --- - --- - --- - --- -

typedef struct CHASM_WINDOW_HANDLE              { 

    Uint32        id;

    int           isClosed;
    int           isOnTop;
    int           width;
    int           height;

    int           h_width;
    int           h_height;
    int           q_width;
    int           q_height;

    int           pkeys;
    int           helkeys;

    int           mouseIgnore;
    int           mouseActive;
    int           mousePos_x;
    int           mousePos_y;

    float         mouseRel_x;
    float         mouseRel_y;
    float         mouseSens;

    SDL_Window*   window;
    SDL_GLContext context;

} chWH;

//  - --- - --- - --- - --- -

chWH        build_whandle   (const char title[], int height, int width);
int         del_whandle     (chWH* whandle);
void        swapBuffers     (chWH* whandle);
void        hideCursor      ();
void        showCursor      ();
int         getIsClosed     (chWH* whandle);
void        pollEvents      (chWH* whandle);
void        onMouseMotion   (chWH* whandle);

//  - --- - --- - --- - --- -

#ifdef __cplusplus
}
#endif

#endif // __CH_WIN_H__
