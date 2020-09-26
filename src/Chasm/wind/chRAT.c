#include "chRAT.h"

//  - --- - --- - --- - --- -

#define CH_RATS_ACTIVE  0x01
#define CH_RATS_IGNORED 0x02

//  - --- - --- - --- - --- -

void chRAT_RESET(chRAT* rat, ipair dim)         { rat->state = 3; rat->abs.x = dim.x; rat->abs.y = dim.y;
                                                                  rat->rel.x = 0.0f;  rat->rel.y = 0.0f;                }

void chRAT_STOP (chRAT* rat)                    { chRAT_UNACTI(rat); rat->rel.x = 0.0f; rat->rel.y = 0.0f;  }

void chRAT_RUN   (chRAT* rat,
                  SDL_MouseMotionEvent* motion) { rat->abs.x = motion->x; rat->abs.y = motion->y;
                                                  rat->rel.x = (float)(motion->xrel * rat->sens);
                                                  rat->rel.y = (float)(motion->yrel * rat->sens);

                                                  chRAT_MKACTI(rat);                                                    }

int  chRAT_TOWALL(chRAT* rat, ipair dim)        { return    (rat->abs.x == 0        )
                                                         | ((rat->abs.y == 0        ) << 1)
                                                         | ((rat->abs.x == dim.x - 1) << 2)
                                                         | ((rat->abs.y == dim.y - 1) << 3);                            }

fpair chRAT_GETMOTION(chRAT* rat, float mult)   { fpair motion = {rat->rel.x * mult, rat->rel.y * mult}; return motion; }

//  - --- - --- - --- - --- -

int  chRAT_ISIGGY(chRAT* rat)                   { return rat->state & CH_RATS_IGNORED;                                  }
void chRAT_UNIGGY(chRAT* rat)                   { rat->state &= ~CH_RATS_IGNORED;                                       }
void chRAT_MKIGGY(chRAT* rat)                   { rat->state |=  CH_RATS_IGNORED;                                       }

int  chRAT_ISACTI(chRAT* rat)                   { return rat->state & CH_RATS_ACTIVE;                                   }
void chRAT_UNACTI(chRAT* rat)                   { rat->state &= ~CH_RATS_ACTIVE;                                        }
void chRAT_MKACTI(chRAT* rat)                   { rat->state |=  CH_RATS_ACTIVE;                                        }

//  - --- - --- - --- - --- -

