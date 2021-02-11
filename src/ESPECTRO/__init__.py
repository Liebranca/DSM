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
# heres the path to Blender I keep around to paste on the console, just ignore it
# "C:\Program Files\Blender Foundation\Blender"

import atexit;

def cleanup ():
    pyge.onQuit();

def register():
    pyge.onInit    (1, 1024);
    atexit.register(cleanup);

    hed      = pyge.types.HED.new("TestProp", "A test property", value = pyge.types.IBAR() );

    pyge.shohed();

    #v          = PYZJC.cast(hed.value, PYZJC.star(vTest)).contents;

    pyge.shohed();
 
    """
    leaving this here in case I forget how to make struct pointer arrays
    arr = (struct * arr_len)()
    arr = cast(arr, star(struct))
    """

def gametest(cont):

    from ESPECTRO.pyge.types.LY_GameObject import LY_GameObject

    own = cont.owner;
    own = LY_GameObject(own);

    print(own.name);
    own.worldPosition = [10,0,0];
    print(own.worldPosition);

#   ---     ---     ---     ---     -----