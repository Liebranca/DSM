#ifndef __CH_KEYB_H__
#define __CH_KEYB_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "ZJC_CommonTypes.h"
#include "SDL/SDL.h"

//  - --- - --- - --- - --- -

static int CH_KEYB_KCODES[16] = { SDLK_r,      SDLK_e,      SDLK_SPACE, SDLK_q,
                                  SDLK_LSHIFT, SDLK_LCTRL,  -1,         -1,
                                  SDLK_TAB,    SDLK_ESCAPE, SDLK_z,     SDLK_x,
                                  SDLK_w,      SDLK_a,      SDLK_s,     SDLK_d  };

//  - --- - --- - --- - --- -

void chKEYB_SETKEY (short* keys, uchar input);
void chKEYB_CLRKEY (short* keys, uchar input);
int  chKEYB_GETKEY (short* keys, short value);

#ifdef __cplusplus
}
#endif

#endif // __CH_KEYB_H__
