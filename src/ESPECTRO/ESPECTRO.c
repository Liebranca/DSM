#include "ESPECTRO.h"
#include "types/SPCTR_BaseKLS.h"

#include <stdio.h>

static KLS test_kls = { 0, 1 };

//  ---     ---     ---     ---     ---

EXPORT cuchar* dooku()                          { return SPCTR_moveptr_KLS(&test_kls, 17); }
