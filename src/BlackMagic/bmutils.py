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

DA_SceneObject_byteSize = 46;

def make_DA_Object(resid, offset, flags, pos, rot, scale):
    buff = bytearray(DA_SceneObject_byteSize);

    buff[ 0: 2] = resid.to_bytes(2, "little");
    buff[ 2: 4] = offset.to_bytes(2, "little");
    buff[ 4: 6] = flags.to_bytes(2, "little");

    buff[ 6:10] = ftb( pos[0]);
    buff[10:14] = ftb( pos[2]);
    buff[14:18] = ftb(-pos[1]);

    buff[18:22] = ftb(-rot[0]);
    buff[22:26] = ftb( rot[2]);
    buff[26:30] = ftb( rot[1]);
    buff[30:34] = ftb( rot[3]);

    buff[34:38] = ftb( scale[0]);
    buff[38:42] = ftb( scale[2]);
    buff[42:46] = ftb( scale[1]);

    return buff;

#   ---     ---     ---     ---     ---

DA_SceneLamp_byteSize = DA_SceneObject_byteSize + 48;

def make_DA_Lamp(resid, offset, flags, pos, rot, scale, lpos, color, ldirn, rad, atten):
    buff = bytearray(DA_SceneObject_byteSize);

    buff[ 0: 2] = resid.to_bytes(2, "little");
    buff[ 2: 4] = offset.to_bytes(2, "little");
    buff[ 4: 6] = flags.to_bytes(2, "little");

    buff[ 6:10] = ftb( pos[0]);
    buff[10:14] = ftb( pos[2]);
    buff[14:18] = ftb(-pos[1]);

    buff[18:22] = ftb(-rot[0]);
    buff[22:26] = ftb( rot[2]);
    buff[26:30] = ftb( rot[1]);
    buff[30:34] = ftb( rot[3]);

    buff[34:38] = ftb( scale[0]);
    buff[38:42] = ftb( scale[2]);
    buff[42:46] = ftb( scale[1]);

    buff[46:50] = ftb( lpos[0]);
    buff[50:54] = ftb( lpos[2]);
    buff[54:58] = ftb(-lpos[1]);

    buff[58:62] = ftb( color[0]);
    buff[62:66] = ftb( color[1]);
    buff[66:70] = ftb( color[2]);
    buff[70:74] = ftb( color[3]);

    buff[74:78] = ftb( ldirn[0]);
    buff[78:82] = ftb( ldirn[2]);
    buff[82:86] = ftb(-ldirn[1]);

    buff[86:90] = ftb( rad);
    buff[90:94] = ftb( atten);

    return buff;

#   ---     ---     ---     ---     ---

DA_buffsizes     = [DA_SceneObject_byteSize, DA_SceneLamp_byteSize]
DA_Scene_hedsize = 10;

class DA_Scene():

    def __init__(self):
        self.buff        = bytearray(DA_Scene_hedsize);
        self.buff[ 0: 8] = b"LYEB$SSX";
        self.ptr         = DA_Scene_hedsize;
        self.count       = 0;

    def add_entry(self, b, size):
        size = DA_buffsizes[size];
        self.buff.extend(b);
        self.ptr   += size;
        self.count += 1;

    def write(self, filepath, ssxname):
        self.buff[ 8:10] = (self.count).to_bytes(2, "little");
        with open(filepath + "\\" + ssxname + ".ssx", "wb+") as file: file.write(self.buff);
