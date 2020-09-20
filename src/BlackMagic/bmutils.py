import bpy, sys, os, struct
from mathutils import Matrix, Vector, Quaternion
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
def ftbarr(arr):  return struct.pack('%sf'%len(arr), *arr);

#   ---     ---     ---     ---     ---

DA_SceneObject_byteSize = 44;

def make_DA_Object(resid, offset, pos, rot, scale):
    buff = bytearray(DA_SceneObject_byteSize);

    buff[ 0: 2] = resid.to_bytes(2, "little");
    buff[ 2: 4] = offset.to_bytes(2, "little");

    buff[ 4: 8] = ftb( pos[0]);
    buff[ 8:12] = ftb( pos[2]);
    buff[12:16] = ftb(-pos[1]);

    buff[16:20] = ftb(-rot[0]);
    buff[20:24] = ftb( rot[2]);
    buff[24:28] = ftb( rot[1]);
    buff[28:32] = ftb( rot[3]);

    buff[32:36] = ftb( scale[0]);
    buff[36:40] = ftb( scale[2]);
    buff[40:44] = ftb( scale[1]);

    return buff;

#   ---     ---     ---     ---     ---

DA_Scene_hedsize = 10;

class DA_Scene():

    def __init__(self, count):
        self.buff        = bytearray(DA_Scene_hedsize + (DA_SceneObject_byteSize * count));
        self.buff[ 0: 8] = b"LYEB$SSX";
        self.buff[ 8:10] = count.to_bytes(2, "little");

        self.ptr         = DA_Scene_hedsize;

    def add_entry(self, b):
        self.buff[self.ptr:self.ptr+DA_SceneObject_byteSize]  = b[0:DA_SceneObject_byteSize];
        self.ptr                                             += DA_SceneObject_byteSize;
