#   ---     ---     ---     ---     ---
# operator methods are prone to working terribly so i wrote this to handle multi-input, multi-type "properties"
# __i think__ it is abstract enough that you could easily adapt it for other tasks

from .DA_utils import ensureNameUnique, BasePropTypes;

#   ---     ---     ---     ---     ---

class Iterprop:

    def __init__(self, from_val = None):

        if from_val:

            self.size        = len(from_val);

            self.values      = [0     for i in range(self.size)];
            self.value_types = ['INT' for i in range(self.size)];
            self.value_keys  = ['Key' for i in range(self.size)];

            i                = 0;

#   ---     ---     ---     ---     ---

            if isinstance(from_val, list):

                for v in from_val:

                    self.values[i] = v;

                    if   isinstance(v, int  ): self.value_types[i] = 'INT';
                    elif isinstance(v, float): self.value_types[i] = 'FLOAT';
                    elif isinstance(v, bool ): self.value_types[i] = 'BOOL';
                    elif isinstance(v, str  ): self.value_types[i] = 'STRING';

                    i += 1;

#   ---     ---     ---     ---     ---

            elif isinstance(from_val, dict):

                for key in from_val:

                    v                  = from_val[key]

                    self.values    [i] = v;
                    self.value_keys[i] = key;

                    if   isinstance(v, int  ): self.value_types[i] = 'INT';
                    elif isinstance(v, float): self.value_types[i] = 'FLOAT';
                    elif isinstance(v, bool ): self.value_types[i] = 'BOOL';
                    elif isinstance(v, str  ): self.value_types[i] = 'STRING';

                    i += 1;

#   ---     ---     ---     ---     ---

        else:

            self.size        = 1;

            self.values      = [0    ];
            self.value_keys  = ['Key'];
            self.value_types = ['INT'];

        self.ptr             = 0;

#   ---     ---     ---     ---     ---

    def ensureKeysUnique(self):

        i = 0;        
        for key in self.value_keys:
            self.value_keys[i] = ensureNameUnique(self.value_keys, i); i += 1;

    def modKey(self, new_key):

        if not new_key: new_key   = 'Key';

        self.value_keys[self.ptr] = new_key;
        self.value_keys[self.ptr] = ensureNameUnique(self.value_keys, self.ptr);

    def modValue(self, new_value):
        self.values[self.ptr] = new_value;

    def curValue(self): return self.values     [self.ptr];
    def curKey  (self): return self.value_keys [self.ptr];
    def curType (self): return self.value_types[self.ptr];

#   ---     ---     ---     ---     ---

    def adv_type(self):

        cur_type = self.value_types[self.ptr];
        idex     = BasePropTypes.index(cur_type);

        if (idex + 1) < len(BasePropTypes):
            idex += 1;

        else: idex = 0;

        self.value_types[self.ptr] = BasePropTypes[idex];

    def bak_type(self):

        cur_type = self.value_types[self.ptr];
        idex     = BasePropTypes.index(cur_type);

        if (idex - 1) >= 0:
            idex -= 1;

        else: idex = len(BasePropTypes) - 1;

        self.value_types[self.ptr] = BasePropTypes[idex];

#   ---     ---     ---     ---     ---

    def getValues(self):
        return self.values[self.ptr], self.value_keys[self.ptr], self.value_types[self.ptr]

    def extend (self):
        self.values.append(0); self.value_keys.append('Key');
        self.value_types.append('INT'); self.ensureKeysUnique()

        self.size += 1;

    def shorten(self):

        if self.size > 1:

            self.values.pop(self.ptr); self.value_keys.pop(self.ptr);
            self.value_types.pop(self.ptr); self.size -= 1;

            if self.ptr >= self.size: self.ptr = self.size - 1;

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
