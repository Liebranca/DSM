#include "SPCTR_BaseKLS.h"
#include "lymath/ZJC_GOPS.h"

//  ---     ---     ---     ---     ---

cuchar*  GamePropTypes  [] = { 9, "INT", "FLOAT", "STRING", "BOOL", "LIST", "DICT", "TIMER", "VECTOR", "REF" };

cuchar** ValueTypes     [] = { GamePropTypes };

//  ---     ---     ---     ---     ---

ushort SPCTR_getsize_valType(ushort i)          { return ((uchar)ValueTypes[i][0]);                                     }

cuchar* SPCTR_moveptr_KLS(KLS* kls, int steps)  { ushort size   = SPCTR_getsize_valType(kls->arr_idex                   );
                                                  kls->itm_idex = clampwrapi           (kls->itm_idex, steps, 1, size   );

                                                  return ValueTypes[kls->arr_idex][kls->itm_idex];                      }

//  ---     ---     ---     ---     ---

