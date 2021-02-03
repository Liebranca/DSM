#include "SPCTR_BaseKLS.h"
#include "lymath/ZJC_GOPS.h"

//  ---     ---     ---     ---     ---

cuchar*  GamePropTypes  [] = { 9, "INT", "FLOAT", "STRING", "BOOL", "LIST", "DICT", "TIMER", "VECTOR", "REF" };

cuchar** ValueTypes     [] = { GamePropTypes };

//  ---     ---     ---     ---     ---

EXPORT ushort SPCTR_getsize_valType(ushort i)   { return ((uchar)ValueTypes[i][0]);                                     }

EXPORT cuchar* SPCTR_moveptr_KLS(KLS* kls,
                                 sint steps)    { ushort size   = SPCTR_getsize_valType(kls->arr_idex                   );
                                                  kls->itm_idex = wrapi                (kls->itm_idex, steps, 1, size   );

                                                  return SPCTR_getcurr_KLS(kls);                                        }

EXPORT cuchar* SPCTR_getcurr_KLS(KLS* kls)      { return ValueTypes[kls->arr_idex][kls->itm_idex];                      }

//  ---     ---     ---     ---     ---
