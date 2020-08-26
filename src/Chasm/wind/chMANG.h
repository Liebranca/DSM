#ifndef __CH_MANG_H__
#define __CH_MANG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "chWIN.h"
#include "GL/glew.h"
#include "ZJC_CommonTypes.h"

#define  CHASM_KEY_FWD 1
#define  CHASM_KEY_BCK 2
#define  CHASM_KEY_LFT 4
#define  CHASM_KEY_RGT 8

//  - --- - --- - --- - --- -

typedef struct CHASM_WINDOW_MANAGER             {

    chWH*         wins;
    Uint8         curwin;
    Uint8         openwins;

    SDL_GLContext context;

} chMG;

//  - --- - --- - --- - --- -

int   chmang_init           (cchar* title, uint width, uint height);
int   chmang_end            ();

int   chmang_winOpen        ();
void  chmang_frameStart     ();
void  chmang_frameEnd       ();

void  chmang_wait           ();

int   chmang_buildwin       (cchar* title, uint width, uint height);

chWH* chmang_curwin         ();
chWH* chmang_getwin         (Uint8 i);

int   getKeyPress           (int keys);

//  - --- - --- - --- - --- -

extern chMG chmang;

#ifdef __cplusplus
}
#endif

#endif // __CH_MANG_H__
