import re;

reg_letters = re.compile("[^a-zA-Z]");
reg_ops     = re.compile(r"[\+\-\*\/]");

op_chars    = ['+', '-', '*', '/'];

#   ---     ---     ---     ---     ---

def ensureNameUnique(colle, idex):

    l = {}; i = 0;

    for v in colle:

        if not isinstance(v, str):
            name = v.name.split(".")[0];

        else:
            name = v.split(".")[0];

        if name in l:

            if i == idex:
                count = str(l[name]);
                return name + "." + ( "0" * (3 - len(count)) ) + count;

            l[name] += 1;

        else: l[name] = 1;

        i += 1;

    if not isinstance(v, str):
        return colle[idex].name;
    
    return colle[idex];

#   ---     ---     ---     ---     ---

def read_numex(ex):

    ex = ''.join(reg_letters.findall(ex));
    s  = ''; i = 0;

    while i < len(ex):

        c = ex[i];

#   ---     ---     ---     ---     ---
# whitespace-breaker. do not allow two separated numbers
# (eg 0 1) unless theres an operator in the middle (0 + 1)

        if c == ' ':

            if (i+1) >= len(ex): break;

            if ex[i+1] != ' ':

                if ex[i+1].isdigit():
                    s  = s + ex[i+1];
                    i += 2;

                else:

                    m  = reg_ops.search(ex[i+1:]); i += 1;

                    if m and ( i < (len(ex) - 1) ):
                        s += ex[i];
                        i += 1;

                    else: break;

            else: i += 1;

#   ---     ---     ---     ---     ---
# also check that there's no lone operators @ end of string

        elif c in op_chars:
            if (i+1) >= len(ex): break;

            s  = s + c;
            i += 1;

        else:
            s  = s + c;
            i += 1;

    i = len(s) - 1;
    while len(s):

        if s[i] in op_chars:
            s  = s[0:-1];
            i -= 1

        else: break;

    return s;

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

def propStr_toList(x):

    w = [s for s in x.split(", ")];

    for s in w:

        if len(s) == 1: s = s + "0";

        if   s[0] == 'i':
            s = s[0] + str( propStr_toInt  (s[1:]) );

        elif s[0] == 'f':
            s = s[0] + str( propStr_toFloat(s[1:]) );

        elif s[0] == 'b':
            s = s[0] + str( propStr_toBool (s[1:]) );

    return '[' + (', '.join(s for s in w)) + ']';

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

PropTypes     = [
                    ("INT",    "Integer", "Integer property"   ),
                    ("FLOAT",  "Float",   "Float property"     ),
                    ("TIMER",  "Timer",   "Timer property"     ),
                    ("VECTOR", "Vector",  "Vector property"    ),
                    ("LIST",   "List",    "List property"      ),
                    ("DICT",   "Dict",    "Dictionary property"),
                    ("STRING", "String",  "String property"    ),
                    ("BOOL",   "Boolean", "Boolean property"   )
                ];

BasePropTypes = [ "INT", "FLOAT", "STRING", "BOOL" ];

#   ---     ---     ---     ---     ---

def baseGamePropTrans(kls, value):

    if   kls == 'INT'   : value = str(propStr_toInt  (value));
    elif kls == 'FLOAT' : value = str(propStr_toFloat(value));
    elif kls == 'BOOL'  : value = str(propStr_toBool (value));

    return value;

#   ---     ---     ---     ---     ---

def morphGamePropValue(self, context, propiter):

    ob   = context.object.DarkAge;
    prop = ob.props[ob.curprop];

    if self.kls in ['INT', 'FLOAT', 'BOOL', 'STRING']:

        value = "%s"%self.input_block;

        if   self.kls == 'INT'  : value = str(propStr_toInt  (value));
        elif self.kls == 'FLOAT': value = str(propStr_toFloat(value));
        elif self.kls == 'BOOL' : value = str(propStr_toBool (value));

        self.input_block = value;

#   ---     ---     ---     ---     ---

    elif self.kls == 'LIST':

        value = "["; i = 0;

        for v in propiter.values:

            if i == propiter.ptr: self.input_block = str(v);

            if   isinstance(v, int  ): value = value + "i" + str(v);
            elif isinstance(v, float): value = value + "f" + str(v);
            elif isinstance(v, bool ): value = value + "b" + str(v);
            elif isinstance(v, str  ): value = value + "s" + v;

            if i < propiter.size - 1: value += ', ';
            i += 1;

        value = value + "]";

#   ---     ---     ---     ---     ---

    elif self.kls == 'DICT':

        value = "{"; i = 0;

        for v in propiter.values:

            if i == propiter.ptr: self.input_block = str(v);

            key  = propiter.value_keys[i] + ":";

            if   isinstance(v, int  ): value = value + key + "i" + str(v);
            elif isinstance(v, float): value = value + key + "f" + str(v);
            elif isinstance(v, bool ): value = value + key + "b" + str(v);
            elif isinstance(v, str  ): value = value + key + "s" + v;

            if i < propiter.size - 1: value += ', ';
            i += 1;

        value = value + "}";


    prop.kls         = self.kls;
    prop.value       = value;

    super(type(self), self).__setattr__("value", value);

#   ---     ---     ---     ---     ---
