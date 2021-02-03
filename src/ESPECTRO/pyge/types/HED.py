# pyside for ESPECTRO\types\SPCTR_BaseHED

from ESPECTRO       import CSIDE, wrap_cfunc;
from ESPECTRO.PYZJC import *;

from .              import KLS;

#   ---     ---     ---     ---     ---

class HED(struct):

    _fields_ = [ ("name",        charstar),
                 ("description", charstar),

                 ("kls",         KLS     ),

                 ("size",        ushort  ),
                 ("ptr",         ushort  ),

                 ("value",       voidstar) ];

#   ---     ---     ---     ---     ---