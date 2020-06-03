#--- --- --- --- --- --- --- --- ---
#	* 	L 	I 	E 	B 	R 	A 	N 	C 	A 	*
#
#	-programmer, cg artist.
#	-contact: dantepaola25@gmail.com
#
#--- --- --- --- --- --- --- --- ---
#				  ┤
#
#	THIS FILE HANDLES REGISTRY OPERATIONS
#	& TYPE CONVERSIONS FOR CUSTOM DATA
#


#--- --- --- --- --- --- --- --- ---
#	SETUP
#--- --- --- --- --- --- --- --- ---
#		0.	LIBRARIES
#	--- 	--- 	--- 	--- 	---
import json, os, glob

#		1.	CONTAINERS
#	--- 	--- 	--- 	--- 	---
internal_registry = {
					"modreg":{},
					"shdreg":{},
					"texreg":{}
					}

#		2.	CONSTANTS
#	--- 	--- 	--- 	--- 	---
crkpath = "D:\\lieb_git\\dsm\\src\\crkhed\\"
datadir = (
			"D:\\lieb_git\\dsm\\Release\\data\\models\\",
			"D:\\lieb_git\\dsm\\Release\\data\\shaders\\",
			"D:\\lieb_git\\dsm\\Release\\data\\textures\\"
		  )

reglast = [["",True], ["",True], ["",True], ["",True]]
floatRange = [i*0.0078125 for i in range(128)]

#		3.	FILECHECKING
#	--- 	--- 	--- 	--- 	---
regkey = 0;
for regfile in ["modreg", "shdreg", "texreg"]:
	filename = crkpath + regfile + ".json"

#			3.1		CHECK INTERNAL REGISTRY
#					THIS ISN'T USED BY THE ENGINE
#					ITS JUST FOR ID GENERATION
#	--- 	--- 	--- 	--- 	---
	if not os.path.exists(filename):
		with open(filename, "w") as out: json.dump({}, out)
	else:
		with open(filename) as json_file: internal_registry[regfile] = json.load(json_file)

#			3.2		MAKE SURE WE HAVE FILES TO WRITE IDS TO
#					CREATE NEW IF LATEST IS FULL -> HAS 256 ITEMS
#					NOT STRICTLY NECESSARY, BUT IT SPEEDS THINGS UP
#	--- 	--- 	--- 	--- 	---
	target = datadir[regkey]; irfCount = len(glob.glob1(target,"*.irf"))
	irfCount = max(0, irfCount-1); filename = target + "reg%d.irf"%irfCount
	if not os.path.exists(filename):
		with open(filename, "w+") as out: out.write("");
		reglast[regkey][1] = True;
	else:
		reglast[regkey][1] = False;
		with open(filename, "r") as f:
			if len([0 for line in f]) == 256:
				filename = target + "reg%d.irf"%irfCount+1
				with open(filename, "w+") as out: out.write("");
				reglast[regkey][1] = True;

	reglast[regkey][0] = filename
	regkey += 1;

#		666.	DONE	.999 	||->	DS
#--- END CRK BLOCK --- --- --- --- --- --- --- ---


#--- --- --- --- --- --- --- --- ---
#	BYTEARRAY OPS
#--- --- --- --- --- --- --- --- ---
#		0.	MISC
#	--- 	--- 	--- 	--- 	---
#	these are taken from stack overflow!

from bisect import bisect_left
def take_closest(myList, myNumber):
#	Assumes myList is sorted. Returns closest value to myNumber.
#	If two numbers are equally close, return the smallest number.
	
	if myNumber in myList: return myNumber

	pos = bisect_left(myList, myNumber)
	if pos == 0:
		return myList[0]
	if pos == len(myList):
		return myList[-1]
	before = myList[pos - 1]
	after = myList[pos]
	if after - myNumber < myNumber - before:
		return after
	else:
		return before

#	--- 	--- 	--- 	--- 	---
def cmodus(a, b): return int(abs(a)%abs(b)*(1,-1)[a<0])

#		1.	BINARY OPS
#	--- 	--- 	--- 	--- 	---
def tobits(x, bytesize):
	x = min(x, sum([2**i for i in range(bytesize)]))
	l = [0 for bit in range(bytesize)]
	for i in range(len(l)):
		l[i] = cmodus(x, 2)
		x = x/2

	return "".join([str(bit) for bit in l])
	
def todecimal(byte):
	x = 0; i = 0;
	for bit in byte:
		x += int(bit)*(2**i); i += 1;

	return x

#		3.	FLOAT CONVERSION
#	--- 	--- 	--- 	--- 	---
def float2(value):
	ivalue, fvalue = [f for f in ("%f"%value).split(".")]

	signbit = "1" if value < 0 else "0";
	zerobit = "00"; fvalue_w = float("0.%s"%fvalue)
	
	if fvalue[0] == "0":
		if fvalue[1] != "0":
			zerobit = "11"
			fvalue_i = floatRange.index(
					take_closest(floatRange, float("0.%s"%fvalue[1:]))
					)

		else: fvalue_i = 0;

	else:
		fvalue_i = floatRange.index(
				take_closest(floatRange, fvalue_w)
				)

		if floatRange[fvalue_i] > fvalue_w:
			zerobit = "10"
			
		elif floatRange[fvalue_i] < fvalue_w:
			zerobit = "01"


	floatbit = tobits(fvalue_i, 7); intbit = tobits(abs(int(ivalue)), 6)
	return [todecimal(signbit + floatbit), todecimal(zerobit + intbit)]

#	--- 	--- 	--- 	--- 	---
def float2_dump(l):
	arrsize = len(l)*2
	buff = bytearray(arrsize)
	
	for i in range(0, arrsize, 2):
		l_index = int(i/2)
		buff[i:i+2] = float2(l[l_index])
		
	return buff

#		2.	UINT CONVERSION
#	--- 	--- 	--- 	--- 	---
def uint2(value):
	bits = tobits(value, 16);
	return [todecimal(bits[0:8]), todecimal(bits[8:16])]

#	--- 	--- 	--- 	--- 	---
def uint3(value):
	bits = tobits(value, 24);
	return [todecimal(bits[0:8]), todecimal(bits[8:16]), todecimal(bits[16:24])]

#		2.	BOOLS
#	--- 	--- 	--- 	--- 	---
def bool1(values):
	s = "";
	for value in values:
		size = 1 if str(value) in ["False", "True"] else 2
		s += tobits(value, size);

	return todecimal(s)

#		666.	PRINTING
#	--- 	--- 	--- 	--- 	---
def p_float2(buff):
	bit1 = tobits(buff[0], 8); bit2 = tobits(buff[1], 8)
	
	signbit = bit1[0]; floatbit = bit1[1:];
	zerobit = bit2[0:2]; intbit = bit2[2:];

	sign = -1 if todecimal(signbit) else 1;
	ivalue = todecimal(intbit); fvalue = floatRange[todecimal(floatbit)]

	zero = todecimal(zerobit)
	if zero:
		if zero == 1: fvalue -= 0.01;
		elif zero == 2: fvalue += 0.01;
		elif zero == 3: fvalue *= 0.1;

	return sign * (ivalue + fvalue);

def p_uint2(buff): return buff[0] + (buff[1] * 256)

#		666.	DONE	.999 	||->	DS
#--- END CRK BLOCK --- --- --- --- --- --- --- ---


#--- --- --- --- --- --- --- --- ---
#	REGISTER OPS
#--- --- --- --- --- --- --- --- ---
#		0.	WRITING
#	--- 	--- 	--- 	--- 	---
def writeToReg(datatype, object_id):
	key = ["modreg", "shdreg", "texreg"][datatype]
	object_id = object_id.split(".")[0]
	source = internal_registry[key]
	target = datadir[datatype]
	
	try: source[object_id] = source[object_id];
	except KeyError:
		source[object_id] = len(source); filename = crkpath + key + ".json"
		s = "\n" if not reglast[datatype][1] else ""
		with open(filename, "w+") as out: json.dump(source, out)
		with open(reglast[datatype][0], 'a') as regf: regf.write("%s"%(s+object_id))

	return source[object_id];
