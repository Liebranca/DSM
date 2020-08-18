#include "DarkAge.h"

#include "GAOL_Bounds.h"

static unsigned char INITFLAGS = 0;

void DARKAGE_BEGIN(int flags)                   {

    GAOL_boundbucket_init();

    INITFLAGS = flags;                                                                                                  }

void DARKAGE_END  ()                            {

    GAOL_boundbucket_end();
    }
