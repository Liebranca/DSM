
def ensureNameUnique(colle, idex):

    l = {}; i = 0;

    for v in colle:

        name = v.name.split(".")[0];

        if name in l:

            if i == idex:
                count = str(l[name]);
                return name + "." + ( "0" * (3 - len(count)) ) + count;

            l[name] += 1;

        else: l[name] = 1;

        i += 1;

    return colle[idex].name;

#   ---     ---     ---     ---     ---

# TODO START
# work out string to operator funcs
# game props are all interpreted from strings so we might need to bring regex in as well
import operator;
opchars = {"+" : operator.add, "-" : operator.sub, "*" : operator.mul, "/" : operator.truediv}
# TODOO END


def propStr_toBool(x):

    if x in ["1", "True", "true"]: return True;
    return False;

def propStr_toInt(x):

    if not x: return 0;
    sign = -1 if x[0] == '-' else 1;
    x = ''.join(c for c in x.split(".")[0] if c.isdigit());

    if not x: return 0;
    return int(x) * sign;

def propStr_toFloat(x):

    if not x: return 0.0;
    sign = -1 if x[0] == '-' else 1;

    w = x.split(".");
    i = ''.join(c for c in w[0] if c.isdigit());
    d = '.'

    if(len(w) > 1): d += ''.join(c for c in x.split(".")[1] if c.isdigit());

    if not i   : i  = '0';
    if d == '.': d += '0';

    return float(i+d) * sign;

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

    return '[' + (', '.join(s for s in w)) + ']'

#   ---     ---     ---     ---     ---

def read_strList(x):

    if ( x[0] != '[' ) or ( x[-1] != ']' ): return (0);    
    x = (x.lstrip("[")).rstrip("]"); w = x.split(", ");

    l = [];

    for s in w:

        if   s[0] == 'i': l.append(propStr_toInt  (s[1:]));
        elif s[0] == 'f': l.append(propStr_toFloat(s[1:]));
        elif s[0] == 'b': l.append(propStr_toBool (s[1:]));

        else            : l.append(                s[1:] );

    return l;

#   ---     ---     ---     ---     ---

PropTypes     = [
                    ("INT",    "Integer", "Integer property"),
                    ("FLOAT",  "Float",   "Float property"  ),
                    ("TIMER",  "Timer",   "Timer property"  ),
                    ("VECTOR", "Vector",  "Vector property" ),
                    ("LIST",   "List",    "List property"   ),
                    ("STRING", "String",  "String property" ),
                    ("BOOL",   "Boolean", "Boolean property")
                ]

BasePropTypes = [
                    ("INT",    "Integer", "Integer property"),
                    ("FLOAT",  "Float",   "Float property"  ),
                    ("STRING", "String",  "String property" ),
                    ("BOOL",   "Boolean", "Boolean property")
                ]

#   ---     ---     ---     ---     ---

def baseGamePropTrans(kls, value):

    if   kls == 'INT'  : value = str(propStr_toInt  (value));
    elif kls == 'FLOAT': value = str(propStr_toFloat(value));
    elif kls == 'BOOL' : value = str(propStr_toBool (value));

    return value;

#   ---     ---     ---     ---     ---

def morphGamePropValue(self, context):

    ob   = context.object.DarkAge;
    prop = ob.props[ob.curprop];

    if self.kls in ['INT', 'FLOAT', 'BOOL', 'STRING']:

        if   (self.slot0 == prop.value) and (self.kls == prop.kls): return None;
        value = "%s"%self.slot0;

        if   self.kls == 'INT'  : value = str(propStr_toInt  (value));
        elif self.kls == 'FLOAT': value = str(propStr_toFloat(value));
        elif self.kls == 'BOOL' : value = str(propStr_toBool (value));

    elif self.kls == 'LIST':

        if (self.listUpdated) and (self.kls == prop.kls): return None;
        value = "";

        for i in range(1, self.lval_num+1):

            subkls = getattr(self, "lval_%i"%i);
            subval = getattr(self, "slot%i"%i );

            if subkls   == 'INT'   : value = value + "i" + subval;
            elif subkls == 'FLOAT' : value = value + "f" + subval;
            elif subkls == 'BOOL'  : value = value + "b" + subval;
            elif subkls == 'STRING': value = value + "s" + subval;

            if i < self.lval_num: value += ', ';

        value = propStr_toList(value); self.listUpdated = True;

    prop.kls   = self.kls;
    prop.value = value;

    super(type(self), self).__setattr__("slot0", value);

#   ---     ---     ---     ---     ---
