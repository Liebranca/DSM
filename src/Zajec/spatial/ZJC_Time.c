#include "ZJC_Time.h"

#include <stdio.h>
#include <math.h>
#include <time.h>

//  - --- - --- - --- - --- -
float          fBy         = 0.0f;
const  float   CPS         = 1.0f / CLOCKS_PER_SEC;

static clock_t framebegin  = 0;
static clock_t frameend    = 0;

static float   framedelta  = 0;
static float   frametime   = 0.0f;
static float   sleeptime   = 0;

static float   timescale   = 1.0f;
static int     framecap    = 60;
static int     framecount  = 0;

void   clock_calcDelta     ()                   { framedelta += (frameend - framebegin) * CPS;                          }
float  clock_getScale      ()                   { return timescale;                                                     }
void   clock_setScale      (float newScale)     { timescale = newScale;                                                 }
void   clock_calcFrametime ()                   { frametime =  (float) (CLOCKS_PER_SEC / framecap);                     }
void   clock_setFramecap   (int newcap)         { framecap = newcap; clock_calcFrametime();                             }
void   clock_frameStart    ()                   { framebegin = clock(); clock_calcDelta();                              }
void   clock_frameEnd      ()                   { frameend = clock(); clock_calcDelta();                                }
int    clock_getSleep      ()                   { return framedelta < frametime;                                        }
void   clock_calcSleepTime ()                   { sleeptime  = frametime - framedelta; framedelta = 0;                  }
uint   clock_getSleepTime  ()                   { uint sleep = (uint) round(sleeptime); return sleep;                   }

float  clock_fBy           (float f)            { clock_calcSleepTime(); return f * (frametime - sleeptime);            }
void   clock_init          ()                   { clock_calcFrametime();                                                }
float  clock_getDelta      ()                   { return framedelta;                                                    }

//  - --- - --- - --- - --- -

/*bool update(float elapsed) {
    this->curr += elapsed;
    if (this->curr >= max) { this->curr = 0.0f; this->cycles++; return true; }
    return false;
}*/
