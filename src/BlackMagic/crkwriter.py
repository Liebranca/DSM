#--- --- --- --- --- --- --- --- ---
#	* 	L 	I 	E 	B 	R 	A 	N 	C 	A 	*
#
#	-programmer, cg artist.
#	-contact: dantepaola25@gmail.com
#
#--- --- --- --- --- --- --- --- ---
#				  ┤

#--- --- --- --- --- --- --- --- ---
#				CRKALERT
#--- --- --- --- --- --- --- --- ---
#
#							!
#
#			DO NOT TRY TO COPY MY STYLE.
#
#--- --- --- --- --- --- --- --- ---

#		666.	DONE	.999 	||->	DS
#--- END CRK BLOCK --- --- --- --- --- --- --- ---


#--- --- --- --- --- --- --- --- ---
#	EXCEPTION HANDLING
#--- --- --- --- --- --- --- --- ---
#		0.	INNER WORKINGS
#	--- 	--- 	--- 	--- 	---
#	this is the auto importlib.reload trick
try:
	if onReload: pass
except: onReload = False

import bpy, sys, os, joj
import crkutils as utils

#	classy trick for silent exceptions
class STAHP(Exception): pass
def quiet_hook(kind, message, traceback):
	if kind == STAHP:
		print(message)
	else:
		sys.__excepthook__(kind, message, traceback)

sys.excepthook = quiet_hook
#		666.	DONE	.999 	||->	DS
#--- END CRK BLOCK --- --- --- --- --- --- --- ---

#--- --- --- --- --- --- --- --- ---
# 	CONSTANTS & CONTAINERS
#--- --- --- --- --- --- --- --- ---
#		0.	STILL INCOMPLETE
#	--- 	--- 	--- 	--- 	---
propNameList = {"ITM":"misc", "WEP":"weapons"}
shaderDict = {"BS":"bullShader", "SS":"squareShader"}
original_mesh = None

#		666.	DONE	.999 	||->	DS
#--- END CRK BLOCK --- --- --- --- --- --- --- ---


#--- --- --- --- --- --- --- --- ---
#	CRK OPS
#--- --- --- --- --- --- --- --- ---
#		0.	QUICK MOD FOR UTILS
#	--- 	--- 	--- 	--- 	--
from mathutils import Matrix, Vector
from math import degrees
import bmesh

#		1.	TRIANGULATION & RESTORING
#	--- 	--- 	--- 	--- 	--
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

#		2.	ARMATURE RESET
#	--- 	--- 	--- 	--- 	--
def reset_pose(ob):
	for pb in ob.parent.pose.bones:
		pb.matrix_basis = Matrix()

	bpy.context.scene.update()

#		666.	DONE	.999 	||->	DS
#--- END CRK BLOCK --- --- --- --- --- --- --- ---


#--- --- --- --- --- --- --- --- ---
# 	PATH SOLVING
#--- --- --- --- --- --- --- --- ---
#		0.	ABSOLUTE PATHS - CHANGE ON RELEASE
#	--- 	--- 	--- 	--- 	---
path = "D:\\lieb_git\\dsm\\Release\\data\\models\\"
texpath = "D:\\lieb_git\\dsm\\Release\\data\\textures\\"
anspath = "D:\\lieb_git\\dsm\\Release\\data\\ans\\"

print("\n#CRKWRITE")

context = bpy.context
original_object = ob = context.object
mesh = ob.data

#		1.	CHECK THAT SELECTION IS VALID
#	--- 	--- 	--- 	--- 	---
if not ob:
	raise STAHP ("No object selected -- export failed.")

elif not isinstance(mesh, bpy.types.Mesh):
	raise STAHP ("Selected object has no mesh data -- export failed.")

#		2.	EXTRACT SOME STRINGS
#	--- 	--- 	--- 	--- 	---
obstrs = ob.name.split(":")
if len(obstrs) > 1:
	obname = obstrs[-1]; obkls = "\\".join(obstrs[0:-1])
else:
	obname = ob.name; obkls = ""

#		3.	MKDIR IF NEED BE
#	--- 	--- 	--- 	--- 	---
meshname = mesh.name.split(".")[0]
filename = path+obkls+"\\"+meshname+".crk"

if not os.path.exists(path+obkls):
	os.mkdir(path+obkls)

#	node flags
isAnsMesh = "ans" in ob.game.properties;
isMapping = "map" in ob.game.properties;
hasInventory = "inv" in ob.game.properties;
isToggler = "tog" in ob.game.properties;
hasDialogue = "diag" in ob.game.properties;
isDamage = "dam" in ob.game.properties;

saveTex = "joj" in ob.game.properties;
if(saveTex): saveTex = ob.game.properties["joj"];

#		4.	CHECK IF OBJECT HAS SIBLINGS TO MERGE
#	--- 	--- 	--- 	--- 	---
if ob.parent:

#		LIST COMP

	hasProps, merge = (
				[child for child 
				in ob.parent.children
				
				if child != ob
				and child.name[0:3] in propNameList],
#					^ hasProps

				[child for child
				in ob.parent.children

				if "apparel" in child.name]

				)
#					^merge

#		4.1		DO MERGING
#	--- 	--- 	--- 	--- 	---
	if merge:

		for piece in merge:	piece.select = True
		
		bpy.ops.object.duplicate()
		bpy.ops.object.join()
		bpy.context.scene.update()

		ob = context.object; mesh = ob.data

#		4.2		TRIANGULATE ORIGINAL OR MERGED MESH 
#	--- 	--- 	--- 	--- 	---
else: hasProps, merge = [], [];
mesh_triangulate(ob, mesh)

#		666.	DONE	.999 	||->	INIT DONE
#--- END CRK BLOCK --- --- --- --- --- --- --- ---


#	--- 	--- 	--- 	--- 	---
#
# 	*	M 	A 	I 	N 				->
#
#	--- 	--- 	--- 	--- 	---
import time
debugPrint = True

try:
	start = time.time()


#--- --- --- --- --- --- --- --- ---
#	PREP MESH FOR EXTRACTION
#--- --- --- --- --- --- --- --- ---
#		-1.	REWIND
#	--- 	--- 	--- 	--- 	---
	if ob.parent: reset_pose(ob)

#		1.	GET MESH ATTRS
#	--- 	--- 	--- 	--- 	---

	numVerts = len(mesh.vertices)
	numIndices = len(mesh.polygons)
	stride = 16

	uv = mesh.uv_layers.active.data

	hasShader = mesh.materials[0]
	hasTextures = {}
	sf = 1 if "actors" in obkls else 1	

#		2.	READ MATERIAL
#	--- 	--- 	--- 	--- 	---
	if hasShader:

		textures = [tex.texture for tex in hasShader.texture_slots if tex]
		buff = None; ti = 0;

		nbuff = None; rbuff = None;
		
		for texture in textures:

#			get subfolder\\texname
			if not buff:
				value = texture.image.name.replace("\\", "/").split(".")[0] + ".joj"

				subdir = "\\".join(texture.name.split(":")[0:-1])
				hasTextures[0] = subdir + ("\\" if subdir else "") + value

#			pixels to buff
				if saveTex: buff = Vector(texture.image.pixels)
				else: break

			elif saveTex:

#				apply additional texturemaps
#				currently only works for ao

				"""mixfac = hasShader.texture_slots[ti].diffuse_color_factor
				ao = Vector(texture.image.pixels) * mixfac
				for pix in range(0, len(buff)):
					buff[pix] *= ao[pix]"""

				if "_n" in texture.name:
					nbuff = Vector(texture.image.pixels)

				elif "_r" in texture.name:
					rbuff = Vector(texture.image.pixels)

			ti += 1

#		encode color buffer as joj
		if buff: 
			if not os.path.exists(texpath + subdir):
				os.mkdir(texpath + subdir)

			with open(texpath + hasTextures[0], "wb+") as crk:
				crk.write(joj.encode(buff, nbuff, rbuff))

#		write shader tag
		try: 
			hasShader = shaderDict[hasShader.name.split(":")[0]]

		except: hasShader = "squareShader"

	else:
		hasShader = "squareShader"


#		3.	CALCULATE DSM PHYSICS ATTRS FROM BGE PHYSICS SETTINGS
#	--- 	--- 	--- 	--- 	---
	phystype = ob.game.physics_type
	phystypes = ["STATIC", "DYNAMIC", "SENSOR"]

	physmode = 0; weightmult = 0.0; speedmult = 1.0;
	
	if phystype in phystypes:
		physmode = phystypes.index(phystype)+1
		if physmode == 2 :
			weightmult = max(0.1, min(ob.game.mass, 12.5) )
			speedmult = 10*weightmult

#		666.	DONE	.999 	||->	ALL DATA COLLECTED
#--- END CRK BLOCK --- --- --- --- --- --- --- ---


#--- --- --- --- --- --- --- --- ---
#	DEFINE HEADER
#--- --- --- --- --- --- --- --- ---
#		0.	FIRST BIT: ALWAYS 9 BYTES
#	--- 	--- 	--- 	--- 	---	
	bit1 = bytearray(9)

	isAnsMesh = "ans" in ob.game.properties;
	isMapping = "map" in ob.game.properties;
	hasInventory = "inv" in ob.game.properties;
	isToggler = "tog" in ob.game.properties;
	hasDialogue = "diag" in ob.game.properties;
	isDamage = "dam" in ob.game.properties;
	
	bit1[0:2] = utils.uint2(numVerts)
	bit1[2:4] = utils.uint2(3*numIndices)
	
	bit1[4] = utils.bool1([
						   int(physmode),
#						   ^ 2-bit physmode flag first

						   bool(isDamage), bool(hasDialogue), bool(isToggler),
						   bool(hasInventory), bool(isMapping), bool(isAnsMesh)
#						   ^ additional 1-bit behaviour flags

						])

	bit1[5:7] = utils.float2(weightmult)
	bit1[7:9] = utils.float2(speedmult)

	if len(bit1) > 9: bit1 = bit1[0:9]

#		1.	SECOND BIT: ALWAYS 48 BYTES -> 3 FLOATS FOR 8 VERTEX COS
#											 ^2 bytes each

#			GAOLER ONLY WORKS WITH CUBICAL SHAPES
#			THE OBJECT'S BOUNDING BOX IS USED FOR THIS PURPOSE
#	--- 	--- 	--- 	--- 	---
	bit2 = bytearray(48); l = [];
	for v in ob.bound_box: l.extend([v[0]/sf, v[2]/sf, -v[1]/sf])

	bit2 = utils.float2_dump(l)

	if len(bit2) > 48: bit2 = bit2[0:48]

#		2.	THIRD BIT: 17 BYTES PER PROP

#			MESH INDEX, NODE CO & ROT
#	--- 	--- 	--- 	--- 	---
	bit3 = bytearray(17*len(hasProps)); i = 0;
	for prop in hasProps:
		pos = prop.matrix_world.translation
		rot = prop.matrix_world.to_quaternion()
		
		bit3[i:i+3] = utils.uint3(utils.writeToReg(0,
		 propNameList[prop.name.split(":")[0]] + "\\" + prop.data.name))
		
		bit3[i+3:i+17] = utils.float2_dump([pos[0], pos[2], -pos[1], rot.x, rot.z, -rot.y, rot.w])
		i += 17

	if len(bit3) > 17*len(hasProps): bit3 = bit3[0:17*len(hasProps)]

#		3.	FOURTH BIT: 3 BYTES -> SHADER ID + TEXTURE ID
#								   ^uint 1bytes  ^uint 2bytes

#			THIS MEANS WE CAN'T HAVE MORE THAN 65K TEXTURES
#			AND 256 SHADERS. BUT IT SHOULD SUFFICE!
#	--- 	--- 	--- 	--- 	---
	bit4 = bytearray(3)
	bit4[0] = utils.writeToReg(1, hasShader)
	bit4[1:] = utils.uint2(utils.writeToReg(2, hasTextures[0]))

	if len(bit4) > 3: bit4 = bit4[0:3]

#		3.	FIFTH BIT: 4 BYTES -> HEDSIZE + SIZE OF THIRD BIT

#			WE WRITE THE SIZE OF THE THIRD BIT, WHICH VARIES FROM FILE TO FILE
#			NECESSARY FOR READING THE DATA.

#			TOTAL HEDSIZE IS ALSO NEEDED TO KNOW WHEN THE VERT BLOCK STARTS
#	--- 	--- 	--- 	--- 	---

	bit5 = bytearray(4);
	hedsize = len(bit1) + len(bit2) + len(bit3) + len(bit4) + len(bit5)
	bit5[0:2] = utils.uint2(hedsize)
	bit5[2:4] = utils.uint2(len(bit3))

	if len(bit5) > 4: bit5 = bit5[0:4]

#		4. FUSE BITS INTO A SINGLE BYTEARRAY
#	--- 	--- 	--- 	--- 	---
	hed = bytearray(hedsize)
	hed[0:4] = bit5[0:]; i = 4;

	hed[i:i+len(bit1)] = bit1[0:]; i += len(bit1);
	hed[i:i+len(bit2)] = bit2[0:]; i += len(bit2);
	hed[i:i+len(bit3)] = bit3[0:]; i += len(bit3);
	hed[i:] = bit4[0:]

	if len(hed) > hedsize: hed = hed[0:hedsize]

#		666.	DONE	.999 	||->	HEADER WRITTEN
#--- END CRK BLOCK --- --- --- --- --- --- --- ---	


#--- --- --- --- --- --- --- --- ---
#	MESH LOOP THROUGH
#--- --- --- --- --- --- --- --- ---
#		0.	INITIAL LOOP

#			FIRST THINGS FIRST, LETS GO VERT BY VERT.
#	--- 	--- 	--- 	--- 	---
	arr = bytearray(numVerts*stride);

	for vert in mesh.vertices:
		vi = vert.index * stride;
		arr[vi:vi+12] = utils.float2_dump([
						vert.co[0]/sf, vert.co[2]/sf, -vert.co[1]/sf,
						vert.normal[0]/sf, vert.normal[2]/sf, -vert.normal[1]/sf
						])

#		1.	SECONDARY LOOP

#			NOW GO FACE BY FACE TO GET INDEX BUFFER
#			ALSO UVS FROM LOOPS
#	--- 	--- 	--- 	--- 	---
	
	faces = [poly for poly in mesh.polygons]
	indexBuff = bytearray(6*numIndices); i = 0;

	for face in faces:
		for vi, loop_index in zip(face.vertices, face.loop_indices):
			svi = (vi * stride) + 12
			arr[svi:svi+4] = utils.float2_dump([uv[loop_index].uv[0], uv[loop_index].uv[1]])

			indexBuff[i:i+2] = utils.uint2(vi); i += 2;

	if len(arr) > numVerts * stride: arr = arr[0:numVerts*stride];
	if len(indexBuff) > 6 * numIndices: indexBuff = indexBuff[0:6*numIndices];

#		3.	MERGE INTO A SINGLE BYTEARRAY AND WRITE TO FILE
#			RESTORE ORIGINAL MESH ONCE WE'RE DONE
#	--- 	--- 	--- 	--- 	---
	result = bytearray(hedsize + len(arr) + len(indexBuff))
	result[0:hedsize] = hed[0:]; i = hedsize;

	result[i:i+len(arr)] = arr[0:]; i += len(arr);
	result[i:i+len(indexBuff)] = indexBuff[0:]

	with open(filename, "wb+") as crk: crk.write(result)
	utils.writeToReg(0, obkls+"\\"+meshname)
	mesh_restore(mesh)

#		666.	DONE	.999 	||->	MESH WAS LOOPED ALL THE WAY THROUGH
#--- END CRK BLOCK --- --- --- --- --- --- --- ---


#--- --- --- --- --- --- --- --- ---
#	SCAN FOR ANS DATA
#--- --- --- --- --- --- --- --- ---
#		0.	PATH & NAME SOLVING
#	--- 	--- 	--- 	--- 	---
	if isAnsMesh:

		#reset_pose(ob)

		if not os.path.exists(anspath+meshname):
			os.mkdir(anspath+meshname)

		ansdir = anspath+meshname+"\\"; header = ""
		actions = (
				[action for action
				in bpy.data.actions
				if obname+"_" in action.name]
				)

#		1.	ACTION LOOP THROUGH
#	--- 	--- 	--- 	--- 	---
		for action in actions:
			actname = action.name.replace(obname+"_", "")

			if not os.path.exists(ansdir+actname):
				os.mkdir(ansdir+actname)

			ob.parent.animation_data.action = action

#			frame is rewound back to zero at action change
			bpy.context.scene.frame_set(0)
			framesize = int(action.frame_range[1])
			header += "%s %d\n"%(actname, framesize)

#		1.1	FRAME LOOP THROUGH
#	--- 	--- 	--- 	--- 	---
			for frame in range(framesize):

#		1.1.1	CREATE BMESH FROM OB
#	--- 	--- 	--- 	--- 	---
				deform = bmesh.new()
				deform.from_object(ob, bpy.context.scene, face_normals=False)
				numVerts = len(deform.verts)
				stride = 12; arr = bytearray(numVerts*stride); i = 0

#		1.1.2	DO THE GAOL BLOCK
#	--- 	--- 	--- 	--- 	---
				bit1 = bytearray(48); l = [];
				for v in ob.bound_box:
					l.extend([v[0]/sf, v[2]/sf, -v[1]/sf])
				
				bit1[0:] = utils.float2_dump(l)

#		1.1.3	ANS FILES ONLY COLLECT VERTEX POSITION AND NORMAL
#				UVS ARE COPIED FROM CRK
#	--- 	--- 	--- 	--- 	---
				for vert in deform.verts:
					arr[i:i+stride] = utils.float2_dump([
							vert.co[0]/sf, vert.co[2]/sf, -vert.co[1]/sf,
							vert.normal[0]/sf, vert.normal[2]/sf, -vert.normal[1]/sf
							])
					
					i += stride
				
#		1.1.4	CHILDREN NODE COORDINATES NEEDED FOR PROPS
#	--- 	--- 	--- 	--- 	---
				bit2 = bytearray(14*len(hasProps)); i = 0;
				for prop in hasProps:
					pos = prop.matrix_world.translation
					rot = prop.matrix_world.to_quaternion()
					
					bit2[i:i+14] = utils.float2_dump([
						pos[0], pos[2], -pos[1], rot.x, rot.z, -rot.y, rot.w])

					i += 14

#		1.1.5	COMBINE ARRAYS
#	--- 	--- 	--- 	--- 	---
				result = bytearray(len(bit1) + len(bit2) + len(arr))
				result[0:len(bit1)] = bit1[0:]; i = len(bit1);
				result[i:i+len(bit2)] = bit2[0:]; i = i+len(bit2);
				result[i:i+len(arr)] = arr[0:]
				
#		1.1.6	WRITE TO ANS FILE, ADVANCE FRAME, FREE BMESH
#	--- 	--- 	--- 	--- 	---
				with open(ansdir+actname+"\\frame%d.ans"%int(frame), "wb+") as ans: ans.write(result)
				bpy.context.scene.frame_set(bpy.context.scene.frame_current+1)
				deform.free()

#		1.2	WRITE ANS HEADER, REWIND FRAME & RESET POSE
#	--- 	--- 	--- 	--- 	---
		with open(ansdir+"%s.hed"%meshname, "w+") as hed: hed.write(header)
		bpy.context.scene.frame_set(0)
		reset_pose(ob)

#		2.	WAIT FOR PYTHONIC SPEED
#	--- 	--- 	--- 	--- 	---
	end = time.time(); execTimer = end - start
	if debugPrint: print("\nExecution time: %fs"%execTimer)

#		666.	DONE	.999 	||->	ANS TREE WRITTEN
#--- END CRK BLOCK --- --- --- --- --- --- --- ---	


#--- --- --- --- --- --- --- --- ---
#	FINALLY
#--- --- --- --- --- --- --- --- ---
#		0.	RESTORE THE ORIGINAL MESH // DELETE MERGED
#	--- 	--- 	--- 	--- 	---
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

	del bit1;
	del bit2;
	del bit3;
	del bit4;
	del bit5;

	del arr;
	del result;


#		666.	DONE	.999 	||->	THAT'S ALL FOLKS
#--- END CRK BLOCK --- --- --- --- --- --- --- ---

#--- 	E 	N 	D 	 --- 	O 	F 	 --- 	M 	A 	I 	N

#	--- 	--- 	--- 	--- 	---
