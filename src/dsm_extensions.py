# add this to "Blender Foundation\Blender\version\scripts\startup"

dsm_srcpath = "D:\\lieb_git\\dsm\\src"; # set this path to the dsm folder

#   ---     ---     ---     ---     ---

import sys, importlib
if dsm_srcpath not in sys.path: sys.path.append(dsm_srcpath);

#   ---     ---     ---     ---     ---

print("\n# - -- - --- - -- - = - -- - --- - -- -\n");
print("Loading up dsm extensions:");

import DarkAge
print(">DarkAge v%s"%DarkAge.version)

if DarkAge.darkpath != dsm_srcpath + "\\\\DarkAge":

    lines = [];
    with open(dsm_srcpath + "\\DarkAge" + "\\__init__.py", "r") as file:
        lines = file.readlines();

    lines[14] = 'darkpath    = "%s";\n'%("\\\\".join(dsm_srcpath.split("\\")) + "\\\\DarkAge")

    with open(dsm_srcpath + "\\DarkAge" + "\\__init__.py", "r+") as file:
        file.writelines(lines);

    importlib.reload(DarkAge);

    from DarkAge import register as DA_register
    DA_register();

import BlackMagic
print(">BlackMagic v%s"%BlackMagic.version)

if BlackMagic.bmpath != dsm_srcpath + "\\\\BlackMagic":

    lines = [];
    with open(dsm_srcpath + "\\BlackMagic" + "\\__init__.py", "r") as file:
        lines = file.readlines();

    lines[0] = 'bmpath      = "%s";\n'%("\\\\".join(dsm_srcpath.split("\\")) + "\\\\BlackMagic")

    with open(dsm_srcpath + "\\BlackMagic" + "\\__init__.py", "r+") as file:
        file.writelines(lines);

    importlib.reload(BlackMagic);

    from BlackMagic import register as bm_register
    bm_register();
    
print("\nDone!");
print("\n# - -- - --- - -- - = - -- - --- - -- -\n");

#   ---     ---     ---     ---     ---

def register(): pass;
