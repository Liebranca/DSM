#   ---     ---     ---     ---     ---
# operator methods are prone to working terribly so i wrote this to handle multi-input, multi-type "properties"
# __i think__ it is abstract enough that you could easily adapt it for other tasks

from ..utils.lystr import ensureNameUnique;

#   ---     ---     ---     ---     ---

class DA_Collection:

    def __init__(self, from_val = [0], valtype = int, keystr = 'Key'):

        self.size    = len(from_val);
        self.values  = { v.name:v for v in from_val };
        self.valtype = valtype;

        self.keystr  = keystr;
        self.ptr     = 0;

    def __contains__(self, item):

        if   type(item) != str: return item in list(self.values.values());
        else                  : return item in self.values;

        return False;

    def __iter__(self): return self.values.__iter__();

    def __getitem__(self, key):
        if   type(key) == int: return list(self.values.values())[key];
        elif type(key) == str: return self.values[key];

        return False;

    def __setitem__(self, key, item):
        if   type(key)  == int: key = list(self.values.keys())[key];
        self.values[key] = item;

    def __delitem__(self, key):
        if   type(key)  == int: key = list(self.values.keys())[key];
        del self.values[key];

#   ---     ---     ---     ---     ---

    def ensureKeysUnique(self):

        i = 0;        
        for v in self.values:
            self[i].name = ensureNameUnique(self.values, i); i += 1;

    def modKey(self, new_key = None):
        if not new_key: new_key = 'Key';
        self[self.ptr].name     = new_key; self.ensureKeysUnique();

    def modValue(self, new_value):
        self[self.ptr] = new_value;

    def curValue(self): return self[self.ptr];

    def positionSwap(self, idex):
        self[self.ptr], self[idex] = self[idex], self[self.ptr];

#   ---     ---     ---     ---     ---

    def extend (self, args = [], kwargs = {}):

        new_item      = self.valtype(*args, **kwargs);
        new_item.name = ensureNameUnique(self.values, -1, new_item.name);

        self[new_item.name] = new_item; self.size += 1;

    def shorten(self):

        if self.size > 0:

            if not self[self.ptr].vital:
                del self[self.ptr]; self.size -= 1;
                if self.ptr >= self.size: self.ptr = self.size - 1;

                return 2; # SUCCESS
            return 1;     # CANT DELETE THIS VALUE
        return 0;         # ARRAY IS EMPTY

#   ---     ---     ---     ---     ---

    def adv_ptr(self):

        if (self.ptr) < (self.size - 1):
            self.ptr += 1;

        else: self.ptr = 0;

    def bak_ptr(self):

        if (self.ptr) > 0:
            self.ptr -= 1;

        else: self.ptr = self.size - 1;

#   ---     ---     ---     ---     ---
