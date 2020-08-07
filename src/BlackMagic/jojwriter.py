from .bmutils import *

def texchk(tex):
    if not tex:
        raise STAHP ("No texture on diffuse slot! *.JOJ conversion failed.");

    if (tex.image.size[0] > JOJ_MAX_SIZE) or (tex.image.size[1] > JOJ_MAX_SIZE):
        raise STAHP ("Image size over %i! *.JOJ conversion failed."%JOJ_MAX_SIZE);

JOJ_MAX_SIZE = 1024;

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

    mat.active_texture_index = 0;
    tex = mat.active_texture;
    texchk(tex);

    dim  = tex.image.size[0] * tex.image.size[1] * 4 * 4;
    buff = bytearray(dim);

    buff[0:dim] = ftbarr(tex.image.pixels)[0:dim];
    
    sizes = bytearray(4);
    sizes[0:2] = tex.image.size[0].to_bytes(2, "little");
    sizes[2:4] = tex.image.size[1].to_bytes(2, "little");

    with open(filepath + "\\" + filename + ".joj", "wb+") as file:
        file.write(sizes+buff);

    del buff;

    if texmode != 2:
        
        mat.active_texture_index = 1;
        tex = mat.active_texture;
        texchk(tex);

        acmb = bytearray(dim);
        acmb[0:dim] = ftbarr(tex.image.pixels)[0:dim];

        with open(filepath + "\\" + filename + "_a" + ".joj", "wb+") as file:
            file.write(sizes+acmb);

        del acmb;

    if texmode != 0:

        mat.active_texture_index = 2;
        tex = mat.active_texture;
        texchk(tex);

        glwb = bytearray(dim);
        glwb[0:dim] = ftbarr(tex.image.pixels)[0:dim];

        with open(filepath + "\\" + filename + "_g" + ".joj", "wb+") as file:
            file.write(sizes+glwb);

        del glwb;

    glwb = bytearray(dim);

    end = time.time(); py_execTimer = end - start;
    
    start = time.time();
    os.system(  bmpath   + "\\BlackMagic.exe" + " "  + "joj" + " "
        
              + filepath + "\\" + filename + ".joj"  + " "
              + filepath + "\\" + archive  + ".daf"  + " "
              + writemode                            + " "
              + writeoffset                        );

    if texmode != 2:

        if eval(writemode) == 0: writemode = "1";
        writeoffset = str(eval(writeoffset)+1);


        os.system(  bmpath   + "\\BlackMagic.exe" + " "  + "joj" + " "
        
                  + filepath + "\\" + filename + "_a"    + ".joj"  + " "
                  + filepath + "\\" + archive  + ".daf"  + " "
                  + writemode                            + " "
                  + writeoffset                        );

    if texmode != 0:

        if eval(writemode) == 0: writemode = "1";
        writeoffset = str(eval(writeoffset)+1);

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
