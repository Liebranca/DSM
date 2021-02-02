from ..utils.lystr import ensureNameUnique, read_numex;

import DarkAge as DA;

#   ---     ---     ---     ---     ---

PropTypes     = [ 'INT', 'FLOAT', 'STRING', 'BOOL', 'LIST', 'DICT', 'TIMER', 'VECTOR' ];

#   ---     ---     ---     ---     ---

class PropValue:

    def __init__(self, key, kls, value):

        self.key   = key;
        self.kls   = kls;
        self.value = value;

#   ---     ---     ---     ---     ---

    def getVars(self):

        if isinstance(self.kls, PropKls):
            ob = self.kls; return ['KLS', ob.current, ''];

        return [self.kls, self.key.capitalize(), str(self.value)];

    @staticmethod
    def fromDict(ob, from_val):

        l = [];
        for key in from_val:
            kls   = from_val   [key];
            value = ob.__dict__[key];

            l.append(PropValue(key, kls, value))

        return l;

#   ---     ---     ---     ---     ---

class GameProperty:

    def __init__(self, name = 'Property', description = 'A game property', kls = 0, value = 0, vital = False):

        self.name        = name;
        self.description = description;

        self.kls         = PropTypes[kls];
        self.value       = value;

        self.vital       = vital;

    def getEditValues(self):

        return PropValue.fromDict(self, { "name"       :'STRING',
                                          "description":'STRING',

                                          "kls"        :PropKls(self.kls, PropTypes),
                                          "value"      :'PROP'                          });

#   ---     ---     ---     ---     ---

def propStr_toBool(x):

    if x in ["1", "True", "true"]: return True;
    return False;

def propStr_toInt(x):
    
    x = read_numex(x);
    if not x: return 0;
    return int(eval(x));

def propStr_toFloat(x):

    x = read_numex(x);
    if not x: return 0.0;

    return float(eval(x));

#   ---     ---     ---     ---     ---

def read_strList(x):

    if ( x[0] != '[' ) or ( x[-1] != ']' ): return (0);    
    x = (x.lstrip("[")).rstrip("]"); w = x.split(", ");

    l = [];

    for s in w:
        if len(s) <= 1: s = "i0";
        if   s[0] != 's': l.append(eval(s[1:]));
        else            : l.append(     s[1:] );

    return l;

#   ---     ---     ---     ---     ---

def read_strDict(x):

    if ( x[0] != '{' ) or ( x[-1] != '}' ): return (0);    
    x = (x.lstrip("{")).rstrip("}"); w = x.split(", ");

    d = {};

    for s in w:

        key, value = s.split(":"); y = None; z = None;

        if   value[0] != 's': y = eval(value[1:]);
        else                : y =      value[1:] ;

        d[key] = y;

    return d;

#   ---     ---     ---     ---     ---

def morphGamePropValue(x, kls, subcolle):

    if kls in ['INT', 'FLOAT', 'BOOL', 'STRING']:

        value = "%s"%x;

        if   kls == 'INT'  : value = propStr_toInt  (value);
        elif kls == 'FLOAT': value = propStr_toFloat(value);
        elif kls == 'BOOL' : value = propStr_toBool (value);

#   ---     ---     ---     ---     ---

    elif kls == 'LIST':

        value = "["; i = 0;

        for v in subcolle.values:

            if   isinstance(v, int  ): value = value + "i" + str(v);
            elif isinstance(v, float): value = value + "f" + str(v);
            elif isinstance(v, bool ): value = value + "b" + str(v);
            elif isinstance(v, str  ): value = value + "s" + v;

            if i < subcolle.size - 1: value += ', ';
            i += 1;

        value = value + "]";

#   ---     ---     ---     ---     ---

    elif kls == 'DICT':

        value = "{"; i = 0;

        for v in subcolle.values:

            key  = subcolle.value_keys[i] + ":";

            if   isinstance(v, int  ): value = value + key + "i" + str(v);
            elif isinstance(v, float): value = value + key + "f" + str(v);
            elif isinstance(v, bool ): value = value + key + "b" + str(v);
            elif isinstance(v, str  ): value = value + key + "s" + v;

            if i < subcolle.size - 1: value += ', ';
            i += 1;

        value = value + "}";

    return value;

#   ---     ---     ---     ---     ---
