bmpath      = "D:\\lieb_git\\dsm\\src\\BlackMagic";
version     = "0.1 BETA";

#   ---     ---     ---     ---     ---

filepath    = "D:\\lieb_git\\dsm\\src\\OrangeBox";
filename    = "dsm_hexdump";
archive     = "Textures";
writemode   = "0x00";
writeoffset = "0x00";

#   ---     ---     ---     ---     ---

def set_bm_filepath(new_path):
    global filepath;
    filepath = new_path;

def set_bm_filename(new_name):
    global filename;
    filename = new_name;

def set_bm_archive(new_archive):
    global archive;
    archive = new_archive;

def set_bm_writemode(new_mode):
    global writemode;
    writemode = new_mode;

def set_bm_writeoffset(new_offset):
    global writeoffset;
    writeoffset = new_offset;

def set_bm_ars(setpath = "", setname = "", setarch = "", setmode = "", setoffs = ""):
    if setpath: set_bm_filepath    (setpath);
    if setname: set_bm_filename    (setname);
    if setarch: set_bm_archive     (setarch);
    if setmode: set_bm_writemode   (setmode);
    if setoffs: set_bm_writeoffset (setoffs);
