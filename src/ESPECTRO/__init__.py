pectropath  = "D:\\lieb_git\\dsm\\src\\ESPECTRO";
version     = "0.1a";

#   ---     ---     ---     ---     ---

from . import pyge;

def register():

    # TEST
    import ctypes; lib = ctypes.WinDLL("D:\\lieb_git\\dsm\\src\\ESPECTRO\\ESPECTRO.dll");
    lib.dooku.restype = ctypes.c_char_p; x = lib.dooku(); print(x); del lib;
    # TEST

#   ---     ---     ---     ---     -----
