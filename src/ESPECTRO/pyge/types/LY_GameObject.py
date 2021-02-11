from bge.types      import KX_GameObject;

from ESPECTRO       import CSIDE, wrap_cfunc;
from ESPECTRO.PYZJC import *;

from mathutils import Vector;

class Vec3(struct):
    _fields_ = [ ("x", c_float),
                 ("y", c_float),
                 ("z", c_float) ];

    def __init__(self, x, y, z):

        self.x = c_float(x);
        self.y = c_float(y);
        self.z = c_float(z);

    def topy(self):
        return Vector([self.x, self.y, self.z]);

class FUCKYOU(struct):
    _fields_ = [ ("name",          charstar),
                 ("worldPosition", Vec3    ) ];

class LY_GameObject(KX_GameObject):

    def __init__(self, old_owner):
        self.__lyattr = FUCKYOU();

        self.__lyattr.name = mcstr("Cube");
        self.__lyattr.worldPosition = Vec3(0.0, 0.0, 0.0);

    @property
    def name(self):
        return self.__lyattr.name;

    @property
    def worldPosition(self):
        return self.__lyattr.worldPosition;

    @worldPosition.setter
    def worldPosition(self, vec):
        vec = Vec3(*vec);

        self.__lyattr.worldPosition.x += vec.x;
        self.__lyattr.worldPosition.y += vec.y;
        self.__lyattr.worldPosition.z += vec.z;