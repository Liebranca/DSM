from ESPECTRO       import CSIDE, wrap_cfunc;
from ESPECTRO.PYZJC import *;

class IBAR(struct):

    _fields_ = [ ("max", sshort),
                 ("min", sshort),
                 ("cur", sshort),
                 ("mod", sshort) ];

    def __init__(self, maxv = 100, minv = 0, curv = 100, modv = 0):
        self.max = sshort(maxv); self.min = sshort(minv);
        self.cur = sshort(curv); self.mod = sshort(modv);
