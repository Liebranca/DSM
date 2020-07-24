from .bmutils import *

JOJ_MAX_SIZE = 1024;

def writejoj(ob_name):

    from . import bmpath, filepath, filename, archive, writeoffset, writemode

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
    if not tex:
        raise STAHP ("No texture on diffuse slot! *.JOJ conversion failed.");

    if (tex.image.size[0] > JOJ_MAX_SIZE) or (tex.image.size[1] > JOJ_MAX_SIZE):
        raise STAHP ("Image size over %i! *.JOJ conversion failed."%JOJ_MAX_SIZE);

    dim  = tex.image.size[0] * tex.image.size[1] * 4 * 4;
    buff = bytearray(dim);
    j    = 0;

    buff[0:dim] = ftbarr(tex.image.pixels)[0:dim]

    sizes = bytearray(4);
    sizes[0:2] = tex.image.size[0].to_bytes(2, "little");
    sizes[2:4] = tex.image.size[1].to_bytes(2, "little");

    with open(filepath + "\\" + filename + ".joj", "wb+") as file:
        file.write(sizes+buff);

    del buff;

    end = time.time(); py_execTimer = end - start;
    
    start = time.time();
    os.system(  bmpath   + "\\BlackMagic.exe" + " "  + "joj" + " "
        
              + filepath + "\\" + filename + ".joj"  + " "
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
