#include "SPCTR_BaseKLS.h"
#include "lymath/ZJC_GOPS.h"
#include "lyutils/ZJC_Evil.h"

//  ---     ---     ---     ---     ---

// Common game property ValueTypes
cuchar*  GamePropTypes  [] = { (cuchar*)9, "INT", "FLOAT", "STRING", "BOOL", "LIST", "DICT", "TIMER", "VECTOR", "REF"   };

// An array of ValueType arrays
cuchar** ValueTypes     [] = { GamePropTypes };

//  ---     ---     ---     ---     ---

ushort SPCTR_getsize_valType(ushort i)          { return ((uchar)ValueTypes[i][0]);                                     }

cuchar* SPCTR_moveptr_KLS(KLS* kls,
                          sint steps)           { ushort size   = SPCTR_getsize_valType(kls->arr_idex                   );
                                                  kls->itm_idex = wrapi                (kls->itm_idex, steps, 1, size   );

                                                  return SPCTR_getcurr_KLS(kls);                                        }

cuchar* SPCTR_setptr_KLS(KLS* kls,
                         sint idex)             { ushort size   = SPCTR_getsize_valType(kls->arr_idex);
                                                  kls->itm_idex = clampi               (idex, 1, size);

                                                  return SPCTR_getcurr_KLS(kls);                                        }

cuchar* SPCTR_getcurr_KLS(KLS* kls)             { return ValueTypes[kls->arr_idex][kls->itm_idex];                      }

//  ---     ---     ---     ---     ---
