#include "SPCTR_Numerics.h"
#include "../Zajec/lyutils/ZJC_Evil.h"

//  ---     ---     ---     ---     ---

IBAR* SPCTR_build_IBAR(sshort max,
                       sshort min,
                       sshort cur,
                       sshort mod)              { IBAR* bar = NULL; WARD_EVIL_MALLOC(bar, IBAR, sizeof(IBAR), 1);
                                                  bar->max = max; bar->min = min; bar->cur = cur; bar->mod = mod;
                                                  return bar;                                                           }

void  SPCTR_del_IBAR(IBAR* bar)                 { WARD_EVIL_MFREE(bar);                                                 }

