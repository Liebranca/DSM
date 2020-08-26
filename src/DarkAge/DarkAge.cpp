#include "DarkAge.h"

#include "GAOL_Bounds.h"

static unsigned char INITFLAGS = 0;

void DARKAGE_BEGIN(int flags)                   {

    GAOL_boundbucket_init();
    DA_objects_init      ();

    INITFLAGS = flags;                                                                                                  }

void DARKAGE_END  ()                            {

    DA_objects_end      ();
    DA_grid_end();
    GAOL_boundbucket_end();
    }
