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
#			JOJ IMAGE COMPRESSION DOESN'T WORK ON RESOLUTIONS OTHER THAN 256X256
#			SIMILARLY, USING A COLOR PALLETE BIGGER THAN 256 ISN'T POSSIBLE
#
#			JOJ IS A LOSSY FORMAT MEANT TO BE SHITTY TO YOUR TEXTURES. ALL RIGHTS RESERVED.
#
#--- --- --- --- --- --- --- --- ---


#--- --- --- --- --- --- --- --- ---
#	CRK BLOCK
#--- --- --- --- --- --- --- --- ---
#		0.	DSM
#	--- 	--- 	--- 	--- 	---
#		0.0		LIBRARIES
#	--- 	--- 	--- 	--- 	---
import numpy as np
from crkutils import take_closest, tobits, todecimal
from sys import getsizeof

#--- --- --- --- --- --- --- --- ---
#		0.0.1	CONTAINERS
#	--- 	--- 	--- 	--- 	---

#		0.1.0	UTILS
#	--- 	--- 	--- 	--- 	---
def jojcrunch(x):
	x = "%.1f"%x
	return int(max(0, min(float(x)*255, 255)))
#--- --- --- --- --- --- --- --- ---
#		2.	CRK
#	--- 	--- 	--- 	--- 	---
try:
	if onReload: pass
except: onReload = False

#		666.	DONE	.999 	||->	DS
#--- END CRK BLOCK --- --- --- --- --- --- --- ---


#--- --- --- --- --- --- --- --- ---
#	MAIN
#--- --- --- --- --- --- --- --- ---
def encode(color, normal, surface):

#		0.	CONTAINERS
#	--- 	--- 	--- 	--- 	---
	color_indices = [i for i in range(256*256*3)];
	index_leap = 256*256; ii = 0;

	color_dict = {}; ci = 0;
	normal_dict = {}; ni = 0;
	surface_dict = {}; si = 0;

#		1.	INITIAL LOOP
#			WE CONVERT AND COLOR SQUISH
#	--- 	--- 	--- 	--- 	---
	for i in range(0, len(color), 4):

#		from 1.0f to 255, we're writting to bytearray.
		r = jojcrunch(color[i])
		nr = jojcrunch(normal[i])
		sr = jojcrunch(surface[i])

		g = jojcrunch(color[i+1])
		ng = jojcrunch(normal[i+1])
		sg = jojcrunch(surface[i+1])

		b = jojcrunch(color[i+2])
		sb = jojcrunch(surface[i+2])

		r = tobits(r, 8); nr = tobits(nr, 8); sr = tobits(sr, 8);
		g = tobits(g, 8); ng = tobits(ng, 8); sg = tobits(sg, 8);
		b = tobits(b, 8); sb = tobits(sb, 8);

		key = r+g+b; nkey = nr+ng; skey = sr+sg+sb;

		if key not in color_dict:
			color_dict[key] = ci; ci += 1;

		if nkey not in normal_dict:
			normal_dict[nkey] = ni; ni += 1;

		if skey not in surface_dict:
			surface_dict[skey] = si; si += 1;

		color_indices[ii] = key;
		color_indices[ii+index_leap] = nkey;
		color_indices[ii+index_leap*2] = skey;
		
		ii += 1;


#		2.	SECOND LOOP
#			COMPRESSION
#	--- 	--- 	--- 	--- 	---
	last_index = color_indices[0]; count = 0; compressed = [];
	for index in color_indices:
		
#		if color is same as last, increase count
		if last_index == index and count < 255:
			count += 1;

#		if no repeats or count over 255, write color index and count
		else:
			if last_index == -1: last_index = index;
			compressed.extend([color_dict[index], count]);
			count = 1;

		last_index = index

#	spit color dict length for sanity debug
	print(len(color_dict));

#		2.	THIRD LOOP
#			WRITE TO BUFFER
#	--- 	--- 	--- 	--- 	---
	buff = bytearray(len(compressed));
	for i in range(0, len(compressed), 2):
		index = compressed[i]; count = compressed[i+1];
		buff[i:i+2] = [index, count];

	hedlen = len(color_dict)
	hed = bytearray( 1 + (hedlen * 3) ); i = 1;
	for key in color_dict:
		hed[i] = todecimal(key[0:8])
		hed[i+1] = todecimal(key[8:16])
		hed[i+2] = todecimal(key[16:24])

	hed[0] = hedlen
	return hed+buff

#		666.	DONE	.999 	||->	DS
#--- END CRK BLOCK --- --- --- --- --- --- --- ---
