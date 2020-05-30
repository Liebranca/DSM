//--- --- --- --- --- --- --- --- ---
//	* 	L 	I 	E 	B 	R 	A 	N 	C 	A 	*
//
//	-programmer, cg artist.
//	-contact: dantepaola25@gmail.com
//
//--- --- --- --- --- --- --- --- ---

#ifndef JOJ_H
#define JOJ_H

//--- --- --- --- --- --- --- --- ---
//	CRK BLOCK
//--- --- --- --- --- --- --- --- ---
//		0.	LIBRARIES
//	--- 	--- 	--- 	--- 	---
#include <vector>

//		0.	BASE STRUCT
//	--- 	--- 	--- 	--- 	---
struct jojcolor {
	int r = 0, g = 0, b = 0;
};
//		0.0		CONTS
//	--- 	--- 	--- 	--- 	---
extern std::vector<jojcolor> jojscale;

//		0.1	PROTOS
//	--- 	--- 	--- 	--- 	---
void buildColorSpectrum();
void addColor();

//		666.	DONE	.999 || ->DS
//--- END CRK BLOCK --- --- --- --- --- --- --- ---

#endif // JOJ_H
