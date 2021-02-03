pectropath  = "D:\\lieb_git\\dsm\\src\\ESPECTRO";
version     = "0.1a";

#   ---     ---     ---     ---     ---

# them four lines by Jim Anderson. Thanks, Jim.
def wrap_cfunc(lib, funcname, restype, argtypes):

    func          = lib.__getattr__(funcname)
    func.restype  = restype
    func.argtypes = argtypes

    return func

from ctypes import WinDLL; CSIDE = WinDLL(pectropath + "\\ESPECTRO.dll");
from .      import PYZJC;
from .      import pyge;

#   ---     ---     ---     ---     -----

def register(): pass;

#   ---     ---     ---     ---     -----