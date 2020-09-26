#ifndef __CH_RAT_H__
#define __CH_RAT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "ZJC_CommonTypes.h"
#include "SDL/SDL.h"

//  - --- - --- - --- - --- -

typedef struct CH_INPUT_MOUSE {

    ipair abs;
    fpair rel;
    float sens;
    char  state;

} chRAT;

//  - --- - --- - --- - --- -

void  chRAT_RESET    (chRAT* rat, ipair dim);
void  chRAT_STOP     (chRAT* rat);
void  chRAT_RUN      (chRAT* rat, SDL_MouseMotionEvent* motion);
int   chRAT_TOWALL   (chRAT* rat, ipair dim);
fpair chRAT_GETMOTION(chRAT* rat, float mult);

int   chRAT_ISIGGY   (chRAT* rat);
void  chRAT_UNIGGY   (chRAT* rat);
void  chRAT_MKIGGY   (chRAT* rat);

int   chRAT_ISACTI   (chRAT* rat);
void  chRAT_UNACTI   (chRAT* rat);
void  chRAT_MKACTI   (chRAT* rat);

//  - --- - --- - --- - --- -

#ifdef __cplusplus
}
#endif

#endif // __CH_RAT_H__
