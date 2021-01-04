from math import degrees, sqrt
import bmesh, itertools

from .bmutils import *

#   ---     ---     ---     ---     ---

original_mesh = None
obj_proplist  = ["obarc", "obarci", "matid"]

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

def writecrk():

    from . import bmpath, filepath, filename, writemode

    print("");

    scene = bpy.context.scene.BlackMagic;
    ob    = scene.curobj;

    if not ob: return ("No active object!");

#   ---     ---     ---     ---     ---

    archive         = scene.mesharch;
    writeoffset     = ob.BlackMagic.arc_offset;

    context         = bpy.context
    original_object = ob;
    mesh            = ob.data

    if not isinstance(mesh, bpy.types.Mesh):
        raise STAHP ("Selected object has no mesh data -- export failed.")

    if os.path.exists(filepath + "\\" + archive  + ".daf"):

        with open(filepath + "\\" + archive  + ".daf", "rb") as f:
            f.seek(8);

            x        = f.read(2);
            fcounter = x[0] + (x[1] << 8);

            if fcounter <= writeoffset: writemode = '0x01';
            else:                       writemode = '0x02';

    else:                               writemode = '0x00';

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

    sumdic = {};
    for vert in mesh.vertices:
        sumdic[(vert.co[0], vert.co[1], vert.co[2])] = [vert.normal[0], vert.normal[1], vert.normal[2]];

    mesh_triangulate(ob, mesh)

    header, vertBuff, indexBuff = None, None, None;

#   ---     ---     ---     ---     ---

    try:
        start = time.time();

        if ob.parent: reset_pose(ob);

#placeholder start
        numBoxes   = 1;
        numFrames  = 1;
        numBinds   = 0;
#placeholder end

        numVerts   = len(mesh.vertices);
        numIndices = len(mesh.polygons);

        uv         = mesh.uv_layers.active.data;

        sf = 1/1 if "actors" in obkls else 1/1;

        header                 = bytearray(208 + 96);

        header[ 0: 2]          = numBoxes.  to_bytes(2, "little");
        header[ 2: 4]          = numFrames. to_bytes(2, "little");
        header[ 4: 6]          = numVerts.  to_bytes(2, "little");
        header[ 6: 8]          = numIndices.to_bytes(2, "little");

        header[ 8:12]          = (mesh.materials[0].BlackMagic.matid).to_bytes(4, "little");

        header[12:16]          = numBinds.  to_bytes(4, "little");
        ii                     = 208;

        l = []; i = 0;
        for v in ob.bound_box:
            header[ii+0:ii+4 ] = ftb( v[0]*sf);
            header[ii+4:ii+8 ] = ftb( v[2]*sf);
            header[ii+8:ii+12] = ftb(-v[1]*sf);
            ii                += 12;
        
        faces        = [poly  for poly in mesh.polygons];
        olverts      = {};

        stride       = 48;
        indexBuff    = bytearray(6*numIndices); i = 0;
        vertBuff     = bytearray(numVerts*stride);

#   ---     ---     ---     ---     ---

        for face in faces:

            for vi, loop_index in zip(face.vertices, face.loop_indices):

                loop                    = mesh.loops[loop_index];
                vert                    = mesh.vertices[vi];
                svi                     = vi * stride;
                n                       = sumdic[(vert.co[0], vert.co[1], vert.co[2])]

                vertBuff[svi+0 :svi+4 ] = ftb( vert.co[0]*sf);
                vertBuff[svi+4 :svi+8 ] = ftb( vert.co[2]*sf);
                vertBuff[svi+8 :svi+12] = ftb(-vert.co[1]*sf);

                vertBuff[svi+12:svi+16] = ftb( n[0]*sf);
                vertBuff[svi+16:svi+20] = ftb( n[2]*sf);
                vertBuff[svi+20:svi+24] = ftb(-n[1]*sf);

                vertBuff[svi+40:svi+44] = ftb(uv[loop_index].uv[0]);
                vertBuff[svi+44:svi+48] = ftb(uv[loop_index].uv[1]);

                indexBuff[i:i+2] = vi.to_bytes(2, "little"); i += 2;

        mesh.calc_tangents();

#   ---     ---     ---     ---     ---

        for face in faces:

            for vi, loop_index in zip(face.vertices, face.loop_indices):

                loop                    = mesh.loops[loop_index];
                vert                    = mesh.vertices[vi];
                svi                     = vi * stride;
                n                       = sumdic[(vert.co[0], vert.co[1], vert.co[2])]

                vertBuff[svi+24:svi+28] = ftb( loop.tangent[0]*sf);
                vertBuff[svi+28:svi+32] = ftb( loop.tangent[2]*sf);
                vertBuff[svi+32:svi+36] = ftb(-loop.tangent[1]*sf);

                t                       = [loop.tangent  [0]*sf, loop.tangent  [2]*sf, -loop.tangent  [1]*sf]
                b                       = [loop.bitangent[0]*sf, loop.bitangent[2]*sf, -loop.bitangent[1]*sf]
                n                       = [n             [0]*sf, n             [2]*sf, -n             [1]*sf]

                vertBuff[svi+36:svi+40] = ftb(bhand(t, b, n));

        del sumdic;

#   ---     ---     ---     ---     ---

        with open(filepath + "\\" + filename + ".crk", "wb+") as file: file.write(header + indexBuff + vertBuff)
        mesh_restore(mesh)

        end = time.time(); py_execTimer = end - start

        start = time.time();
        os.system(  bmpath   + "\\BlackMagic.exe" + " "  + "crk" + " "

                  + filepath + "\\" + filename + ".crk"  + " "
                  + filepath + "\\" + archive  + ".daf"  + " "
                  + writemode                            + " "
                  + str(writeoffset)                                    );

        end = time.time(); cpp_execTimer = end - start;

        if debugPrint:
            print("\nExecution time: ")
            print(">Python: %fs"%py_execTimer)
            print(">C: %fs"%cpp_execTimer)
            print(">Total::%fs"%(cpp_execTimer + py_execTimer))

        print("");

#   ---     ---     ---     ---     ---

    finally:

        mesh.free_tangents();

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

        return None;

def masscrk():

    print("Writing selected objects to DAF$CRK. Hold on.")

    d = {ob.name:ob.game.properties["obarci"].value for ob in bpy.context.selected_objects}
    d = sorted(d.items(), key=lambda x : x[1], reverse = False);

    for obname, meshid in d:
        writecrk(obname);
