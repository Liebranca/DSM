#   ---     ---     ---     ---     ---
# a simple class for any value-type kind of var

class KLS:

    def __init__(self, idex, value_type):

        self.idex    = idex;
        self.types   = value_type.KLS_LIST;

        self.current = self.types[self.idex];

#   ---     ---     ---     ---     ---
# move through value-type list

    def adv_type(self):

        if self.idex < len(self.types) - 1: self.idex += 1;
        else                              : self.idex  = 0;

        self.current = self.types[self.idex];

    def bak_type(self):

        if self.idex > 0: self.idex -= 1;
        else            : self.idex  = len(self.types) - 1;

        self.current = self.types[self.idex];

#   ---     ---     ---     ---     ---
# base class for DarkAge objects

class ESPECTRO:

    KLS_LIST    = ['GHOSTVAL'];

    DEF_NAME    = "ESPECTRO";
    DEF_VALUE   = 0 << 0;

    DESCRIPTION = "DEFAULT KLS";

#   ---     ---     ---     ---     ---

    def __init__(self, name = None, description = None, kls_idex = None, value = None, vital = False):

        self.kls   = KLS(kls_idex, type(self));

        self.name  = name;  self.description = description;
        self.value = value; self.vital       = vital;

        if not name       : self.name        = type(self).DEF_NAME;
        if not description: self.description = type(self).DESCRIPTION;
        if not value      : self.value       = type(self).DEF_VALUE;

#   ---     ---     ---     ---     ---
