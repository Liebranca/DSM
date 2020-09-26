#ifndef __CH_WIN_H__
#define __CH_WIN_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "chRAT.h"
#include "chKEYB.h"
#include "chJOY.h"

#define       CHASM_NUM_JOYS   4

//  - --- - --- - --- - --- -

typedef struct CHASM_WINDOW_HANDLE              { 

    Uint32        id;

    int           isClosed;
    int           isOnTop;

    ipair         size;
    ipair         hsize;

    short         keys;
    chRAT         mouse;
    chJOY         joys[CHASM_NUM_JOYS];

    SDL_Window*   window;
    SDL_GLContext context;

} chWH;

//  - --- - --- - --- - --- -

void        openJoy(chWH* whandle, int i);

chWH        build_whandle   (const char title[], int height, int width);
int         del_whandle     (chWH* whandle);
void        swapBuffers     (chWH* whandle);
void        hideCursor      ();
void        showCursor      ();
int         getIsClosed     (chWH* whandle);
void        pollEvents      (chWH* whandle);

//  - --- - --- - --- - --- -

#ifdef __cplusplus
}
#endif

#endif // __CH_WIN_H__
