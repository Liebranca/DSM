#include "ZJC_Time.h"
#include "../lymath/ZJC_GOPS.h"

#include <stdio.h>
#include <time.h>



//  - --- - --- - --- - --- -

const  float   CPS         = 1.0f/CLOCKS_PER_SEC;

static clock_t begin       = 0;
static clock_t end         = 0;
static float   worktime    = 0.0f;
static float   sleeptime   = 0.0f;
static float   frametime   = 0.0f;

static float   total       = 0;
static float   curr        = 0.0f;
static float   scale       = 1.0f;
static int     framecap    = 60;

void   clock_calcSleepTime ()                   { sleeptime = (float) (end - begin) * CPS;                              }
void   clock_calcWorkTime  ()                   { worktime  = (float) (begin - end) * CPS;                              }
float  clock_getCurr       ()                   { return curr;                                                          }
float  clock_getScale      ()                   { return scale;                                                         }
void   clock_setScale      (float newScale)     { scale = newScale;                                                     }
void   clock_calcFrametime ()                   { frametime = (float) CLOCKS_PER_SEC / framecap;                        }
void   clock_setFramecap   (int newcap)         { framecap = newcap; clock_calcFrametime();                             }
float  clock_calcCurr      ()                   { curr = (float) ((sleeptime + worktime) * scale);
                                                  return curr;                                                          }
float  clock_calcTotal     ()                   { total += (float) clock_calcCurr(); return total;                      }
void   clock_frameStart    ()                   { begin = clock(); clock_calcWorkTime();                                }
void   clock_frameEnd      ()                   { end   = clock(); clock_calcSleepTime(); clock_calcTotal();            }
int    clock_getSleep      ()                   { return frametime > sleeptime;                                         }
float  clock_fBy           (float f)            { return f * curr;                                                      }
void   clock_init          ()                   { clock_calcFrametime();                                                }
float  clock_getDelta      ()                   { return frametime - sleeptime;                                         }
float  clock_secondLength  ()                   { return frametime * framecap;                                          }

//  - --- - --- - --- - --- -

/*bool update(float elapsed) {
    this->curr += elapsed;
    if (this->curr >= max) { this->curr = 0.0f; this->cycles++; return true; }
    return false;
}*/
