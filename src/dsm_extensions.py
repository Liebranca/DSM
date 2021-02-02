# add this to "Blender Foundation\Blender\version\scripts\startup"

dsm_srcpath = "D:\\lieb_git\\dsm\\src"; # set this path to the dsm folder

#   ---     ---     ---     ---     ---

import sys, importlib;
if dsm_srcpath not in sys.path: sys.path.append(dsm_srcpath);

dsm_module_list = [ ("ESPECTRO",   "pectropath",  0),
                    ("DarkAge" ,   "darkpath"  , 14),
                    ("BlackMagic", "bmpath",      0) ];

def genspace(width, s): return s + (" " * ( width - len(s) ))

def dsm_alignprint_module(module):

    n = genspace(23, ">" + module.__name__);
    v = genspace( 8, "v" + module.version );

    print(n + v);

def dsm_rektpath_module(modulename, pathvar, pathline):

    module = importlib.import_module(modulename);
    n      = "\\%s"%(module.__name__);

    dsm_alignprint_module(module);

#   ---     ---     ---     ---     ---

    if module.__dict__[pathvar] != dsm_srcpath + n:

        lines = [];

        with open(dsm_srcpath + n + "\\__init__.py", "r") as file:
            lines = file.readlines();

        l               = lines[pathline].split("=");
        l[1]            = '"%s";\n'%("\\\\".join(dsm_srcpath.split("\\")) + "\\" + n)

        lines[pathline] = '= '.join(line for line in l);

#   ---     ---     ---     ---     ---

        with open(dsm_srcpath + n + "\\__init__.py", "r+") as file:
            file.writelines(lines);

        importlib.reload(module);

#   ---     ---     ---     ---     ---

    regsub = getattr(module, 'register'); regsub();

#   ---     ---     ---     ---     ---

print("\n# - -- - --- - -- - = - -- - --- - -- -\n");
print("Loading up dsm extensions:");

for mod in dsm_module_list:
    dsm_rektpath_module(*mod);

print("\nDone!");
print("\n# - -- - --- - -- - = - -- - --- - -- -\n");

#   ---     ---     ---     ---     ---

def register(): pass;
