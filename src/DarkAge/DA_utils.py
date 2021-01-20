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