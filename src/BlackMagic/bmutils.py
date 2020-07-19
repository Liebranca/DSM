import bpy, sys, os, struct
from .frac8_converter import meshfrac

#   ---     ---     ---     ---     ---
#   classy trick for silent exceptions

class STAHP(Exception): pass;
def quiet_hook(kind, message, traceback):
    if kind == STAHP:
        print(message);
    else:
        sys.__excepthook__(kind, message, traceback);

sys.excepthook = quiet_hook;

#   ---     ---     ---     ---     ---

import time;
debugPrint = True;

#   ---     ---     ---     ---     ---

def cmodus(a, b): return int(abs(a)%abs(b)*(1,-1)[a<0]);
def ftb(num):     return struct.pack('<f', num);
