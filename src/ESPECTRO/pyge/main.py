#   ---     ---     ---     ---     ---
# pyside for the C generic container handler

from ESPECTRO       import CSIDE, wrap_cfunc;
from ESPECTRO.PYZJC import *;

from .types         import HED;

#   ---     ---     ---     ---     ---
# init and cleanup                               (num_conts, size )
onInit = wrap_cfunc(CSIDE, "ESPECTRO_INIT",None, [uint, uint     ]);
onQuit = wrap_cfunc(CSIDE, "ESPECTRO_END", None, [               ]);

shohed = wrap_cfunc(CSIDE, "shohed", None, []);

#   ---     ---     ---     ---     ---
