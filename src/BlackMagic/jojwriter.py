from .bmutils import *

def texchk(tex, slotname = "Diffuse"):
    if not tex:
        raise STAHP ("No texture on %s slot! *.JOJ conversion failed."%slotname);

    if (tex.image.size[0] > JOJ_MAX_SIZE) or (tex.image.size[1] > JOJ_MAX_SIZE):
        raise STAHP ("Image size over %i! *.JOJ conversion failed."%JOJ_MAX_SIZE);

JOJ_MAX_SIZE = 1024;

tex_proplist = ["texarc", "texarci", "texmode"]

def writejoj(ob_name):

    from . import bmpath, filepath, filename, archive, writeoffset, writemode, texmode

    print("");

    ob = None;

    if ob_name not in bpy.data.objects:
        raise STAHP ("Object %s not found"%ob_name)

    else: ob = bpy.data.objects[ob_name];

    start = time.time();

    mat = ob.active_material;
    if not mat:
        raise STAHP ("No active material! *.JOJ conversion failed.");

    for propname in tex_proplist:
        if propname not in ob.game.properties: raise STAHP ("Object %s missing property %s"%(ob.name, propname));

    archive     = ob.game.properties["texarc"].value;
    writeoffset = "0x%X"%ob.game.properties["texarci"].value;
    texmode     = ob.game.properties["texmode"].value;
    fcounter    = 0;

    if os.path.exists(filepath + "\\" + archive  + ".daf"):

        with open(filepath + "\\" + archive  + ".daf", "rb") as f:
            f.seek(8);

            x        = f.read(2);
            fcounter = x[0] + (x[1] << 8);

            if fcounter <= int(writeoffset, 16): writemode = '0x01';
            else:                                writemode = '0x02';

    else:                                        writemode = '0x00';

    mat.active_texture_index = 0;
    tex = mat.active_texture;
    texchk(tex);

    dim  = tex.image.size[0] * tex.image.size[1] * 4 * 4;
    buff = bytearray(dim);

    buff[0:dim] = ftbarr(tex.image.pixels)[0:dim];
    
    hed      = bytearray(5);
    hed[0:2] = tex.image.size[0].to_bytes(2, "little");
    hed[2:4] = tex.image.size[1].to_bytes(2, "little");
    hed[4:5] = (texmode == 3).to_bytes(1, "little");

    with open(filepath + "\\" + filename + ".joj", "wb+") as file:
        file.write(hed+buff);

    del buff;

    if texmode != 3:

        hed[4:5] = (1).to_bytes(1, "little");

        if texmode != 2:
            
            mat.active_texture_index = 1;
            tex = mat.active_texture;
            texchk(tex, "ShadingInfo");

            acmb = bytearray(dim);
            acmb[0:dim] = ftbarr(tex.image.pixels)[0:dim];

            with open(filepath + "\\" + filename + "_a" + ".joj", "wb+") as file:
                file.write(hed+acmb);

            del acmb;

            mat.active_texture_index = 2;
            tex = mat.active_texture;
            texchk(tex, "Normal");

            norb = bytearray(dim);
            norb[0:dim] = ftbarr(tex.image.pixels)[0:dim];

            with open(filepath + "\\" + filename + "_n" + ".joj", "wb+") as file:
                file.write(hed+norb);

            del norb;

        if texmode != 0:

            mat.active_texture_index = 3;
            tex = mat.active_texture;
            texchk(tex, "Glow/Alpha");

            glwb = bytearray(dim);
            glwb[0:dim] = ftbarr(tex.image.pixels)[0:dim];

            with open(filepath + "\\" + filename + "_g" + ".joj", "wb+") as file:
                file.write(hed+glwb);

            del glwb;

    glwb = bytearray(dim);

    end = time.time(); py_execTimer = end - start;
    
    start = time.time();
    os.system(  bmpath   + "\\BlackMagic.exe" + " "  + "joj" + " "
        
              + filepath + "\\" + filename + ".joj"  + " "
              + filepath + "\\" + archive  + ".daf"  + " "
              + writemode                            + " "
              + writeoffset                        );    

    if texmode != 3:

        if texmode != 2:

            if int(writemode, 16) < 2: fcounter += 1;

            writeoffset  = str(eval(writeoffset)+1);
            if fcounter <= int(writeoffset, 16): writemode = '0x01';
            else:                                writemode = '0x02';

            os.system(  bmpath   + "\\BlackMagic.exe" + " "  + "joj" + " "
            
                      + filepath + "\\" + filename + "_a"    + ".joj"  + " "
                      + filepath + "\\" + archive  + ".daf"  + " "
                      + writemode                            + " "
                      + writeoffset                        );

            if int(writemode, 16) < 2: fcounter += 1;
            writeoffset = str(eval(writeoffset)+1);

            if fcounter <= int(writeoffset, 16): writemode = '0x01';
            else:                                writemode = '0x02';

            os.system(  bmpath   + "\\BlackMagic.exe" + " "  + "joj" + " "
            
                      + filepath + "\\" + filename + "_n"    + ".joj"  + " "
                      + filepath + "\\" + archive  + ".daf"  + " "
                      + writemode                            + " "
                      + writeoffset                        );

        if texmode != 0:

            if int(writemode, 16) < 2: fcounter += 1;

            writeoffset  = str(eval(writeoffset)+1);
            if fcounter <= int(writeoffset, 16): writemode = '0x01';
            else:                                writemode = '0x02';

            os.system(  bmpath   + "\\BlackMagic.exe" + " "  + "joj" + " "
        
                      + filepath + "\\" + filename + "_g"    + ".joj"  + " "
                      + filepath + "\\" + archive  + ".daf"  + " "
                      + writemode                            + " "
                      + writeoffset                        );

    end = time.time(); cpp_execTimer = end - start;

    if debugPrint:
        print("\nExecution time: ")
        print(">Python: %fs"%py_execTimer)
        print(">C: %fs"%cpp_execTimer)
        print(">Total::%fs"%(cpp_execTimer + py_execTimer))

    print("");
