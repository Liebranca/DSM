from .BM_utils import *
JOJ_MAX_SIZE = 1024;

#   ---     ---     ---     ---     ---

def texchk(tex, slotnum):
    if not tex: return ("No texture on slot %i!"%slotnum);

    elif (tex.image.size[0] > JOJ_MAX_SIZE) or (tex.image.size[1] > JOJ_MAX_SIZE):
        return ("Image size over %i!"%JOJ_MAX_SIZE);

    return None;

#   ---     ---     ---     ---     ---

def writejoj():

    from . import bmpath, filepath, filename, writemode

    print(""); start = time.time();

    scene = bpy.context.scene.BlackMagic;
    mate  = scene.curmat;

    if not mate: return ("No active material!");

#   ---     ---     ---     ---     ---

    archive   = scene.texarch;
    texoffset = mate.BlackMagic.tex_offset;
    matoffset = mate.BlackMagic.mat_offset;
    texcount  = mate.BlackMagic.num_textures;

    fcounter    = 0;

    if os.path.exists(filepath + "\\" + archive  + ".daf"):

        with open(filepath + "\\" + archive  + ".daf", "rb") as f:
            f.seek(8);

            x        = f.read(2);
            fcounter = x[0] + (x[1] << 8);

            if fcounter <= texoffset: writemode = '0x01';
            else:                     writemode = '0x02';

    else:                             writemode = '0x00';

#   ---     ---     ---     ---     ---

    texoffset = str(texoffset);

    mate.active_texture_index = 0;
    tex = mate.active_texture;
    evilstate = texchk(tex, 0);
    if evilstate: return evilstate;

    dim  = tex.image.size[0] * tex.image.size[1] * 4 * 4;
    buff = bytearray();

    for i in range(0, texcount):
        mate.active_texture_index = i;
        tex = mate.active_texture;
        evilstate = texchk(tex, i);
        if evilstate: return evilstate;

        buff.extend(ftbarr(tex.image.pixels)[0:dim]);

#   ---     ---     ---     ---     ---
    
    hed        = bytearray(25);

    hed[ 0: 2] = tex.image.size[0].to_bytes     (2, "little");
    hed[ 2: 4] = tex.image.size[1].to_bytes     (2, "little");
    hed[ 4: 5] = texcount.to_bytes              (1, "little");
    hed[ 5: 6] = mate.BlackMagic.flags.to_bytes (1, "little");

    hed[ 6:10] = ftb(mate.BlackMagic.spec_mult);
    hed[10:14] = ftb(mate.BlackMagic.diff_mult);
    hed[14:18] = ftb(mate.BlackMagic.ref_mult );
    hed[18:22] = ftb(mate.BlackMagic.glow_rea );

    hed[22:24] = mate.BlackMagic.shader.to_bytes(2, "little");
    hed[24:25] = matoffset.to_bytes             (1, "little");

#   ---     ---     ---     ---     ---

    with open(filepath + "\\" + filename + ".joj", "wb+") as file:
        file.write(hed+buff);

    del buff;

#   ---     ---     ---     ---     ---

    end = time.time(); py_execTimer = end - start;
    
    start = time.time();
    os.system(  bmpath   + "\\BlackMagic.exe" + " "  + "joj" + " "
        
              + filepath + "\\" + filename + ".joj"  + " "
              + filepath + "\\" + archive  + ".daf"  + " "
              + writemode                            + " "
              + texoffset                                           );    

    end = time.time(); cpp_execTimer = end - start;

    if debugPrint:
        print("\nExecution time: ")
        print(">Python: %fs"%py_execTimer)
        print(">C: %fs"%cpp_execTimer)
        print(">Total::%fs"%(cpp_execTimer + py_execTimer))

    print("");

    return None;

#   ---     ---     ---     ---     ---

