//--- --- --- --- --- --- --- --- ---
//	* 	L 	I 	E 	B 	R 	A 	N 	C 	A 	*
//
//	-programmer, cg artist.
//	-contact: dantepaola25@gmail.com
//
//--- --- --- --- --- --- --- --- ---

//--- --- --- --- --- --- --- --- ---
//	CRK BLOCK
//--- --- --- --- --- --- --- --- ---
//		0.	LIBS
//	--- 	--- 	--- 	--- 	---
#include "../lymath/utils.h"
#include "joj.h"

//		0.0		CONTS
//	--- 	--- 	--- 	--- 	---
std::vector<jojcolor> jojscale;

//		0.1		FUNKS
//	--- 	--- 	--- 	--- 	---
void addColor(jojcolor color) {
	lymath::clampi(color.r, 0, 255);
	lymath::clampi(color.g, 0, 255);
	lymath::clampi(color.b, 0, 255);
	jojscale.push_back(color);
}

void buildColorSpectrum() {
	for (int x = 0; x < 16; x++) {
		for (int y = 0; y < 16; y++) {
			for (int z = 0; z < 16; z++) {
				addColor({ x * 16, y * 16, z * 16 });
			}
		}
	} 
	
	addColor({ 256, 256, 256 });

}

//		666.	DONE	.999 || ->DS
//--- END CRK BLOCK --- --- --- --- --- --- --- ---
