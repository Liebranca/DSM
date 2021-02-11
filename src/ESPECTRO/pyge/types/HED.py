# pyside for ESPECTRO\types\SPCTR_BaseHED

from ESPECTRO       import CSIDE, wrap_cfunc;
from ESPECTRO.PYZJC import *;

from .KLS           import KLS;

#   ---     ---     ---     ---     ---
# fwd decl

SPCTR_fill_gBlock = None;
SPCTR_find_gBlock = None;
SPCTR_del_gBlock  = None;

#   ---     ---     ---     ---     ---

class HED(struct):

    _fields_ = [ ("name",        charstar ),
                 ("description", charstar ),

                 ("kls",         KLS      ),

                 ("size",        ushort   ),
                 ("ptr",         ushort   ),

                 ("value",       voidstar ) ];

#   ---     ---     ---     ---     ---
# use this rather than __init__
    @staticmethod
    def new(name, desc, klt = 0, kli = 0, size = 1, ptr = 0, value = 0):

        self             = HED                   (                                 );
        size             = max                   (1, min(size, (2**16) - 1)        );

        self.name        = mcstr                 (name                             );
        self.description = mcstr                 (desc                             );

        self.kls         = KLS                   (klt, kli + 1                     );

        self.size        = ushort                (size                             );
        self.ptr         = ushort                (ptr                              );

        value            = pointer               (value                            );
        self.value       = cast                  (value, voidstar                  );

        # you need this ID to find the block in C memory so don't fucking touch it
        self.ID          = uint                  (SPCTR_fill_gBlock(self, 0));
        self.address     = pointer               (self                             );

        return self;

#   ---     ---     ---     ---     ---
# blanks out this block for later reuse
    def free(self): SPCTR_del_gBlock(self.ID);

# gets values from C memory and copies them to the py handle
    def read(self):
        values = SPCTR_find_gBlock(self.ID).contents;
        for attr, type in HED._fields_:
            setattr(self, attr, getattr(values, attr));

# when modifying block handle from py, call this to set C memory
    def update(self, pp): SPCTR_fill_gBlock(pp, self.ID);

#   ---     ---     ---     ---     ---

    def __str__(self):
        return "%s: %s"%(self.name.decode("utf-8"), self.description.decode("utf-8"));

#   ---     ---     ---     ---     ---
# C memory setter                                                     (values, block id)
SPCTR_fill_gBlock = wrap_cfunc(CSIDE, "SPCTR_fill_gBlock", uint,      [star(HED), uint]);

# C memory getter                                                     (block id        )
SPCTR_find_gBlock = wrap_cfunc(CSIDE, "SPCTR_find_gBlock", star(HED), [uint           ]);

# Blanks out a block for later use                                    (block id        )
SPCTR_del_gBlock  = wrap_cfunc(CSIDE, "SPCTR_del_gBlock",  None,      [uint           ]);
