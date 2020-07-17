from .bmutils import *

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

    if (tex.image.size[0] > 256) or (tex.image.size[1] > 256):
        raise STAHP ("Image size over 256! *.JOJ conversion failed.");

    dim  = tex.image.size[0] * tex.image.size[1] * 3 * 4;
    buff = bytearray(dim);
    j    = 0;

    for i in range(0, dim, 12):
        buff[i+0:i+4 ] = ftb(tex.image.pixels[j+0]);
        buff[i+4:i+8 ] = ftb(tex.image.pixels[j+1]);
        buff[i+8:i+12] = ftb(tex.image.pixels[j+2]);

        j += 4;

    sizes = bytearray(2);
    sizes[0:2] = [tex.image.size[0], tex.image.size[1]]

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
