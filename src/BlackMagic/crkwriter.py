original_mesh = None

#   ---     ---     ---     ---     ---

from mathutils import Matrix, Vector
from math import degrees
import bmesh

from .bmutils import *

#   ---     ---     ---     ---     ---

def mesh_triangulate(ob, me):
    bm = bmesh.new()
    bm.from_mesh(me)
    global original_mesh
    original_mesh = bm.copy()
    bm.free()

    bm = bmesh.new()
    bm.from_object(ob, bpy.context.scene, face_normals=False)
    bmesh.ops.triangulate(bm, faces=bm.faces)
    bm.to_mesh(me)
    bm.free()

def mesh_restore(me):
    global original_mesh
    original_mesh.to_mesh(me)
    original_mesh.free()
    original_mesh = None

#   ---     ---     ---     ---     ---

def reset_pose(ob):
    for pb in ob.parent.pose.bones:
        pb.matrix_basis = Matrix()

    bpy.context.scene.update()

#   ---     ---     ---     ---     ---

def writecrk(ob_name):

    from . import bmpath, filepath, filename, archive, writeoffset, writemode

    print("");

    ob = None;

    if ob_name not in bpy.data.objects:
        raise STAHP ("Object %s not found"%ob_name)

    else: ob = bpy.data.objects[ob_name];

    context = bpy.context
    original_object = ob = context.object
    mesh = ob.data    

    if not ob:
        raise STAHP ("No object selected -- export failed.")

    elif not isinstance(mesh, bpy.types.Mesh):
        raise STAHP ("Selected object has no mesh data -- export failed.")

    #   ---     ---     ---     ---     ---

    obstrs = ob.name.split(":")
    if len(obstrs) > 1:
        obname = obstrs[-1]; obkls = "\\".join(obstrs[0:-1])
    else:
        obname = ob.name; obkls = ""

    #   ---     ---     ---     ---     ---

    meshname = mesh.name

    if ob.parent:

        hasProps, merge = (
                    [child for child 
                    in ob.parent.children
                    
                    if child != ob
                    and child.name[0:3] in propNameList],

                    [child for child
                    in ob.parent.children

                    if "apparel" in child.name]

                    )

        if merge:

            for piece in merge: piece.select = True
            
            bpy.ops.object.duplicate()
            bpy.ops.object.join()
            bpy.context.scene.update()

            ob = context.object; mesh = ob.data

    else: hasProps, merge = [], [];
    mesh_triangulate(ob, mesh)

    header, vertBuff, indexBuff = None, None, None;

    try:
        start = time.time()

        if ob.parent: reset_pose(ob)

        numVerts = len(mesh.vertices)
        numIndices = len(mesh.polygons)

        uv = mesh.uv_layers.active.data

        sf = 1 if "actors" in obkls else 1

        gaolerSize = 96; ii = 4;
        header = bytearray(4 + gaolerSize)
        
        header[0:2] = numVerts.to_bytes(2, "little")
        header[2:4] = numIndices.to_bytes(2, "little")

        l = []; i = 0;
        for v in ob.bound_box:
            header[ii:ii+4] = ftb(v[0]/sf)
            header[ii+4:ii+8] = ftb(v[2]/sf)
            header[ii+8:ii+12] = ftb(-v[1]/sf)
            ii += 12;
        
        stride = 20;
        vertBuff = bytearray(numVerts*20);

        for vert in mesh.vertices:
            vi = vert.index * stride;
            vertBuff[vi:vi+4] = ftb(vert.co[0]/sf)
            vertBuff[vi+4:vi+8] = ftb(vert.co[2]/sf)
            vertBuff[vi+8:vi+12] = ftb(-vert.co[1]/sf)
        
        faces = [poly for poly in mesh.polygons]
        indexBuff = bytearray(6*numIndices); i = 0;

        for face in faces:
            for vi, loop_index in zip(face.vertices, face.loop_indices):
                svi = (vi * stride) + 12
                vertBuff[svi:svi+4] = ftb(uv[loop_index].uv[0])
                vertBuff[svi+4:svi+8] = ftb(uv[loop_index].uv[1])

                indexBuff[i:i+2] = vi.to_bytes(2, "little"); i += 2;

        with open(filepath + "\\" + filename + ".crk", "wb+") as file: file.write(header + vertBuff + indexBuff)
        mesh_restore(mesh)

        end = time.time(); py_execTimer = end - start

        start = time.time();
        os.system(  bmpath   + "\\BlackMagic.exe" + " "  + "crk" + " "

                  + filepath + "\\" + filename + ".crk"  + " "
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


    finally:

        global original_mesh;

        if original_mesh:

            if merge:
                original_mesh.free()
                original_mesh = None

                context.scene.objects.unlink(ob)
                bpy.data.objects.remove(ob)

            else:
                mesh_restore(mesh)

        elif merge:
            context.scene.objects.unlink(ob)
            bpy.data.meshes.remove(ob.data, do_unlink = True )

            original_object.select = True
            bpy.context.scene.objects.active = original_object

        del header;
        del vertBuff;
        del indexBuff;
