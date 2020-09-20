# add this to "Blender Foundation\Blender\version\scripts\startup"

dsm_srcpath = "D:\\lieb_git\\dsm\\src";

#   ---     ---     ---     ---     ---

import sys, importlib
if dsm_srcpath not in sys.path: sys.path.append(dsm_srcpath);

#   ---     ---     ---     ---     ---

print("\n# - -- - --- - -- - = - -- - --- - -- -\n");
print("Loading up dsm extensions:");

import BlackMagic
print(">BlackMagic v%s\n"%BlackMagic.version)

if BlackMagic.bmpath != dsm_srcpath + "\\\\BlackMagic":

    lines = [];
    with open(dsm_srcpath + "\\BlackMagic" + "\\__init__.py", "r") as file:
        lines = file.readlines();

    lines[0] = 'bmpath      = "%s";\n'%("\\\\".join(dsm_srcpath.split("\\")) + "\\\\BlackMagic")

    with open(dsm_srcpath + "\\BlackMagic" + "\\__init__.py", "r+") as file:
        file.writelines(lines);

    importlib.reload(BlackMagic);


print("Done!");
print("\n# - -- - --- - -- - = - -- - --- - -- -\n");

#   ---     ---     ---     ---     ---

def register(): pass;
