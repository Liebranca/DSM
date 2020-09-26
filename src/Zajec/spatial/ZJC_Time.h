#ifndef __ZJC_TIME_H__
#define __ZJC_TIME_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../ZJC_CommonTypes.h"

//      - --- - --- - --- - --- -

void   clock_init           ();
void   clock_frameStart     ();
void   clock_frameEnd       ();
int    clock_getSleep       ();
uint   clock_getSleepTime   ();
float  clock_fBy            (float f);
float  clock_getScale       ();
void   clock_setScale       (float newScale);
void   clock_setFramecap    (int newcap);
float  clock_getDelta       ();

//  - --- - --- - --- - --- -

extern float fBy;

/*DS_TICKER(float max) { this->max = max; this->curr = max; }

virtual ~DS_TICKER() { ; }
float curr = 0.0f, max = 0.0f;
int cycles = -1;

//      - --- - --- - --- - --- -

float getp() { return (this->curr / this->max) + this->cycles; }

void restart() { this->curr = 0.0f; this->cycles = -1; }

bool update(float elapsed);*/

#ifdef __cplusplus
}
#endif

#endif //__ZJC_TIME_H__
