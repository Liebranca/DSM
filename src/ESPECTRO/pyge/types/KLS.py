# pyside for ESPECTRO\types\SPCTR_BaseKLS

from ESPECTRO       import CSIDE, wrap_cfunc;
from ESPECTRO.PYZJC import *;

#   ---     ---     ---     ---     ---
# forward declarations so the class won't biszht

SPCTR_getsize_valType = None;
SPCTR_moveptr_KLS     = None;
SPCTR_getcurr_KLS     = None;

#   ---     ---     ---     ---     ---

class KLS(struct):

    _fields_ = [ ("arr_idex", ushort),
                 ("itm_idex", ushort) ];

    def __init__(self, arr_idex, itm_idex):
        self.arr_idex = arr_idex; self.itm_idex = itm_idex;

#   ---     ---     ---     ---     ---
# 1-1 map to C side funcs

    def getsize(self       ): return SPCTR_getsize_valType(self.arr_idex     );
    def getcurr(self       ): return SPCTR_getcurr_KLS    (byref(self)       ).decode("utf-8");
    def moveptr(self, steps): return SPCTR_moveptr_KLS    (byref(self), steps).decode("utf-8");

#   ---     ---     ---     ---     ---
# utils and shorthands

    def advptr (self       ): return self.moveptr( 1);
    def bakptr (self       ): return self.moveptr(-1);

#   ---     ---     ---     ---     ---

SPCTR_getsize_valType = wrap_cfunc(CSIDE, "SPCTR_getsize_valType", ushort,   [ushort         ]);
SPCTR_moveptr_KLS     = wrap_cfunc(CSIDE, "SPCTR_moveptr_KLS",     charstar, [star(KLS), sint]);
SPCTR_getcurr_KLS     = wrap_cfunc(CSIDE, "SPCTR_getcurr_KLS",     charstar, [star(KLS)      ]);