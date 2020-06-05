#--- --- --- --- --- --- --- --- ---
#   *   L   I   E   B   R   A   N   C   A   *
#
#   -programmer, cg artist.
#   -contact: dantepaola25@gmail.com
#
#--- --- --- --- --- --- --- --- ---
#                 ┤

#--- --- --- --- --- --- --- --- ---
#               CRKALERT
#--- --- --- --- --- --- --- --- ---
#
#                           !
#
#           DO NOT TRY TO COPY MY STYLE.
#
#--- --- --- --- --- --- --- --- ---

#       666.    DONE    .999    ||->    DS
#--- END CRK BLOCK --- --- --- --- --- --- --- ---


#--- --- --- --- --- --- --- --- ---
#   EXCEPTION HANDLING
#--- --- --- --- --- --- --- --- ---
#       0.  INNER WORKINGS
#   ---     ---     ---     ---     ---
#   this is the auto importlib.reload trick
try:
    if onReload: pass
except: onReload = False

import bpy, sys, os

#   classy trick for silent exceptions
class STAHP(Exception): pass
def quiet_hook(kind, message, traceback):
    if kind == STAHP:
        print(message)
    else:
        sys.__excepthook__(kind, message, traceback)

sys.excepthook = quiet_hook
#       666.    DONE    .999    ||->    DS
#--- END CRK BLOCK --- --- --- --- --- --- --- ---

#--- --- --- --- --- --- --- --- ---
#   CONSTANTS & CONTAINERS
#--- --- --- --- --- --- --- --- ---
#       0.  STILL INCOMPLETE
#   ---     ---     ---     ---     ---
propNameList = {"ITM":"misc", "WEP":"weapons"}
shaderDict = {"BS":"bullShader", "SS":"squareShader"}
original_mesh = None

#       666.    DONE    .999    ||->    DS
#--- END CRK BLOCK --- --- --- --- --- --- --- ---


#--- --- --- --- --- --- --- --- ---
#   CRK OPS
#--- --- --- --- --- --- --- --- ---
#       0.  QUICK MOD FOR UTILS
#   ---     ---     ---     ---     --
from mathutils import Matrix, Vector
from math import degrees
import bmesh, struct

def ftb(num): return struct.pack('<f', num);


#       1.  TRIANGULATION & RESTORING
#   ---     ---     ---     ---     --
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

#       2.  ARMATURE RESET
#   ---     ---     ---     ---     --
def reset_pose(ob):
    for pb in ob.parent.pose.bones:
        pb.matrix_basis = Matrix()

    bpy.context.scene.update()

#       666.    DONE    .999    ||->    DS
#--- END CRK BLOCK --- --- --- --- --- --- --- ---


#--- --- --- --- --- --- --- --- ---
#   PATH SOLVING
#--- --- --- --- --- --- --- --- ---
#       0.  ABSOLUTE PATHS - CHANGE ON RELEASE
#   ---     ---     ---     ---     ---
path = "D:\\lieb_git\\dsm\\src\\BlackMagic"

print("\n#CRKWRITE")

context = bpy.context
original_object = ob = context.object
mesh = ob.data

#       1.  CHECK THAT SELECTION IS VALID
#   ---     ---     ---     ---     ---
if not ob:
    raise STAHP ("No object selected -- export failed.")

elif not isinstance(mesh, bpy.types.Mesh):
    raise STAHP ("Selected object has no mesh data -- export failed.")

#       2.  EXTRACT SOME STRINGS
#   ---     ---     ---     ---     ---
obstrs = ob.name.split(":")
if len(obstrs) > 1:
    obname = obstrs[-1]; obkls = "\\".join(obstrs[0:-1])
else:
    obname = ob.name; obkls = ""

#       3.  MKDIR IF NEED BE
#   ---     ---     ---     ---     ---
meshname = mesh.name.split(".")[0]
filename = path+obkls+"\\"+meshname+".crkraw"

saveTex = "joj" in ob.game.properties;
if(saveTex): saveTex = ob.game.properties["joj"];

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

import time
debugPrint = True

try:
    start = time.time()

    if ob.parent: reset_pose(ob)

    numVerts = len(mesh.vertices)
    numIndices = len(mesh.polygons)

    uv = mesh.uv_layers.active.data

    hasShader = mesh.materials[0]
    hasTextures = {}

    sf = 1 if "actors" in obkls else 1

    gaolerSize = 96; ii = 8;
    header = bytearray(4 + gaolerSize)
    
    header[0:2] = numVerts.to_bytes(2, "little")
    header[2:4] = numIndices.to_bytes(2, "little")

    l = []; i = 0;
    for v in ob.bound_box:
        header[ii:ii+4] = ftb(v[0]/sf)
        header[ii+4:ii+8] = ftb(v[2]/sf)
        header[ii+8:ii+12] = ftb(-v[1]/sf)
        ii += 12;
    
    stride = 32;
    vertBuff = bytearray(numVerts*32);

    for vert in mesh.vertices:
        vi = vert.index * stride;
        vertBuff[vi:vi+stride] = ftb(vert.co[0]/sf)
        vertBuff[vi+4:vi+stride+8] = ftb(vert.co[2]/sf)
        vertBuff[vi+8:vi+stride+12] = ftb(-vert.co[1]/sf)
        vertBuff[vi+12:vi+stride+16] = ftb(vert.normal[0]/sf)
        vertBuff[vi+20:vi+stride+24] = ftb(vert.normal[2]/sf)
        vertBuff[vi+24:vi+stride+28] = ftb(-vert.normal[1]/sf)
    
    faces = [poly for poly in mesh.polygons]
    indexBuff = bytearray(6*numIndices); i = 0;

    for face in faces:
        for vi, loop_index in zip(face.vertices, face.loop_indices):
            svi = (vi * stride) + 24
            vertBuff[svi:svi+4] = ftb(uv[loop_index].uv[0])
            vertBuff[svi+4:svi+8] = ftb(uv[loop_index].uv[1])

            indexBuff[i:i+2] = vi.to_bytes(2, "little"); i += 2;

    with open(filename, "wb+") as crk: crk.write(header + vertBuff + indexBuff)
    mesh_restore(mesh)

    end = time.time(); execTimer = end - start
    if debugPrint: print("\nExecution time: %fs"%execTimer)

finally:

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
