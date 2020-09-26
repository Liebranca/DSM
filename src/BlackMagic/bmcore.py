from . import bmpath, set_bm_ars
from .bmutils   import meshfrac
from .crkwriter import writecrk, masscrk
from .jojwriter import writejoj
from .ssxwriter import writessx

#   ---     ---     ---     ---     ---

isrunning   = False;

writerlist  = ["writecrk", "writejoj", "writessx", "meshfrac"];
commlist    = ["masscrk"]
setterlist  = ["setpath", "setname", "setarch", "setmode", "setoffs", "settexm"];
systemlist  = ["quit", "help"];

#   ---     ---     ---     ---     ---

def run():

    global isrunning;
    isrunning = True;

    while(isrunning):
        s = request_input();
        read_input(s);

#   ---     ---     ---     ---     ---

def bm_quit():
    global isrunning;
    isrunning = False;

def bm_help():
    print("\nUsage: <command> <argument>\n");
    print("Command list:\n%s"%(    "  -Exporters: "   + ", ".join(writerlist)
                                 + "\n  -Commands : " + ", ".join(commlist)
                                 + "\n  -Setters  : " + ", ".join(setterlist)
                                 + "\n  -Utility  : " + ", ".join(systemlist) + "\n"  ));

    print("Argument is either a string or a variable name, when the format var:name is used\n")

#   ---     ---     ---     ---     ---

def update_settings(comm, arg, arg_is_var):

    if arg_is_var:
        if isinstance(arg, str): arg = "\\\\".join(arg.split("\\"));

    off  = 5;
    newl = 'filepath    = "%s";\n'%arg;

    if comm == "setname":
        off += 1;
        newl = 'filename    = "%s";\n'%arg;

    if comm == "setarch":
        off += 2;
        newl = 'archive     = "%s";\n'%arg;

    lines = [];

    with open(bmpath + "\\__init__.py", "r") as file:
        lines = file.readlines();

    lines[off] = newl;

    with open(bmpath + "\\__init__.py", "r+") as file:
        file.writelines(lines);


#   ---     ---     ---     ---     ---

def request_input():
    print("Issue your command, master.\n");
    s = input(">> ");

    return s.split(" ");

def read_input(s):

    comm, arg  = None, None;
    arg_is_var = False;

    if   len(s) == 2:
        comm, arg = s;

        if("var:" in arg):            
            arg        = arg.replace("var:", "")
            arg_is_var = True;

    elif len(s) == 1: comm = s[0];

    else:
        bm_help();
        return 0;

    if comm in writerlist:
        eval(comm + "('" + arg + "')");

    elif comm in commlist:
        eval(comm + "()")

    elif comm in setterlist:

        if arg_is_var: 
            eval("set_bm_ars(" + comm + " = "  + arg + ")" )
            arg = eval(arg)

        else: eval("set_bm_ars(" + comm + " = '" + arg + "')")

        if comm in ["setpath", "setname", "setarch" ]: update_settings(comm, arg, arg_is_var);

    elif comm in systemlist: eval("bm_" + comm + "()");
    else:                    bm_help();


#   ---     ---     ---     ---     ---
