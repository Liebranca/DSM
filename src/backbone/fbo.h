//--- --- --- --- --- --- --- --- ---
//	* 	L 	I 	E 	B 	R 	A 	N 	C 	A 	*
//
//	-programmer, cg artist.
//	-contact: dantepaola25@gmail.com
//
//--- --- --- --- --- --- --- --- ---

#ifndef FBO_H
#define FBO_H

//--- --- --- --- --- --- --- --- ---
//	CRK BLOCK
//--- --- --- --- --- --- --- --- ---
//		0.	LIBRARIES
//	--- 	--- 	--- 	--- 	---
#include "display.h"
#include "../rend/texture.h"
#include <vector>

//		666.	DONE	.999 || ->DS
//--- END CRK BLOCK --- --- --- --- --- --- --- ---

//--- --- --- --- --- --- --- --- ---
//	CRK BLOCK
//--- --- --- --- --- --- --- --- ---
namespace paint {

//		0.	BASE STRUCT
//	--- 	--- 	--- 	--- 	---
	struct FBO {
		unsigned int loc;
		unsigned int rbo;
		DS_TEXTURE* color;
	};
	struct CANVAS {
		unsigned int vao, vbo;
	};

//		1.	CONTS
//	--- 	--- 	--- 	--- 	---
	extern std::vector<FBO*> frameBuffers;
	extern CANVAS* canvas;

	extern const unsigned int SHADOWRES;

//--- --- --- --- --- --- --- --- ---
//		2.	PROTOS
//	--- 	--- 	--- 	--- 	---
	FBO* createColorBuffer();
	FBO* createDepthBuffer();
	void initCanvas();
	void deleteCanvas();
	void deleteFrameBuffers();

};

//		666.	DONE	.999 || ->DS
//--- END CRK BLOCK --- --- --- --- --- --- --- ---

#endif //FBO_H
