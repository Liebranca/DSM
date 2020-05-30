#//--------------------------
import bpy, sys

try:
	if onReload: pass
except: onReload = False

class STAHP(Exception): pass
def quiet_hook(kind, message, traceback):
	if kind == STAHP:
		print(message)
	else:
		sys.__excepthook__(kind, message, traceback)

sys.excepthook = quiet_hook


#//--------------------------
path = "D:\\lieb_git\\dsm\\Release\\data\\ans\\"

print("\n#SKULWRITE")

context = bpy.context
ob = context.object
arma = ob.data
if not ob:
	raise STAHP ("No object selected -- export failed.")

elif not isinstance(arma, bpy.types.Armature):
	raise STAHP ("Selected object is no armature -- export failed.")


#//--------------------------
def matrix_world(bone):
	local = arma.bones[bone.name].matrix

	parent = arma.bones[bone.name].parent
	if parent == None:
		return  local
	else:
		parent_local = arma.bones[parent.name].matrix
		return matrix_world(parent) * (parent_local.inverted() * local)


#//--------------------------
from math import radians, degrees
from mathutils import Matrix
indices = {bone.name:i for i, bone in enumerate(arma.bones)}

em = bpy.data.objects.new( "EM", None )
bpy.context.scene.objects.link( em )
bpy.context.scene.objects.active = em

bpy.ops.object.constraint_add(type='COPY_TRANSFORMS')
em.constraints["Copy Transforms"].target = ob

try:
	armaname = arma.name
	usebones = [bone for bone in arma.bones] # if bone.layers[0]
	result = "#SKULHED %s %d\n"%(armaname, len(usebones));
	result += "#SET "

	for bone in usebones:
		em.constraints["Copy Transforms"].subtarget = bone.name
		result = result[:-1]+"\n#PAR%d "%(indices[bone.parent.name] if bone.parent else -1)
		
		bpy.ops.object.visual_transform_apply(); bpy.context.scene.update()
		twat = bone.matrix.to_euler().copy()
		pos = bone.matrix_local.to_translation().copy()

		if bone.parent:
			pos -= bone.parent.matrix_local.to_translation().copy()
		else:
			twat.x -= 1.5708

		result += "%.4f/%.4f/%.4f "%(pos[0], pos[2], -pos[1])
		result += "%.4f/%.4f/%.4f "%(twat.x,twat.z,-twat.y)

	result = result[:-1]

	filename = path+armaname+".skul"
	with open(filename, "w+") as skul: skul.write(result)

finally:
	bpy.data.objects.remove(em, do_unlink=True)
	bpy.context.scene.objects.active = ob
