import re;

reg_letters    = re.compile("[^a-zA-Z]");
reg_forbid     = re.compile(r"[^\,\;\:\¨\´\`\<\>\º\ª\[\]\{\}\=\(\)\&\%\$\·\#\!\|\_\^\"\¬\~\'\¡\?\¿\ç\Ç\\]")
reg_ops        = re.compile(r"[\+\-\*\/]");
reg_repeat     = re.compile(r"\.\d\d\d");

op_chars       = ['+', '-', '*', '/'];

#   ---     ---     ---     ---     ---

def ensureNameUnique(colle, idex, search = ''):

    l = {}; i = 0;

    for v in colle:

        if not search:
            if not isinstance(v, str):
                name = v.name

            else:
                name = v

        else:   name = search;

        reptail = reg_repeat.findall(name);
        if reptail:
            name = name.rstrip(reptail[-1]);

#   ---     ---     ---     ---     ---

        if name in l:

            if i == idex:
                count = str(l[name]);
                return name + "." + ( "0" * (3 - len(count)) ) + count;

            l[name] += 1;

        else: l[name] = 1;

        i += 1;

#   ---     ---     ---     ---     ---

    if not search:

        if not isinstance(v, str):
            name = colle[idex].name
        
        else:
            name = colle[idex];

    else:

        name = search;

        if name in l:
            count = str(l[search]);
            return name + "." + ( "0" * (3 - len(count)) ) + count;

#   ---     ---     ---     ---     ---

    reptail = reg_repeat.findall(name);
    if reptail:
        name = name.rstrip(''.join(reptail));

    return name;

#   ---     ---     ---     ---     ---

def read_numex(ex):

    ex = ''.join(reg_letters.findall(ex));
    ex = ''.join(reg_forbid. findall(ex));
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
