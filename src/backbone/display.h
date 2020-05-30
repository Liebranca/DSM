//--- --- --- --- --- --- --- --- ---
//	* 	L 	I 	E 	B 	R 	A 	N 	C 	A 	*
//
//	-programmer, cg artist.
//	-contact: dantepaola25@gmail.com
//
//--- --- --- --- --- --- --- --- ---

#ifndef DISPLAY_H
#define DISPLAY_H

//--- --- --- --- --- --- --- --- ---
//	CRK BLOCK
//--- --- --- --- --- --- --- --- ---
//		0.	LIBRARIES
//	--- 	--- 	--- 	--- 	---
#include <string>
#include <vector>
#include "SDL/SDL.h"
#include "GL/glew.h"
#include "glm/glm.hpp"

//		666.	DONE	.999 || ->DS
//--- END CRK BLOCK --- --- --- --- --- --- --- ---


//--- --- --- --- --- --- --- --- ---
//	CRK BLOCK
//--- --- --- --- --- --- --- --- ---
class DS_DISPLAY
{
//--- --- --- --- --- --- --- --- ---
//		0.	EXTERNAL
//	--- 	--- 	--- 	--- 	---
	public:

//		1.0		NITS
//	--- 	--- 	--- 	--- 	---
	DS_DISPLAY(const std::string& title, int height, int width);
	virtual ~DS_DISPLAY();
	bool getIsClosed();


	void swapBuffers();
	SDL_Window* getWin() { return window; }

//		1.1		FUNKS
//	--- 	--- 	--- 	--- 	---
	void pollEvents();
	void mouseWrap();

//		1.2		CONTS
//	--- 	--- 	--- 	--- 	---
	std::vector<bool> pkeys = { 0,0,0,0,0,0,0 };
	std::vector<bool> helkeys = { 0 };
	std::vector<float> mouseRel = { 0,0 };
	std::vector<int> mousePos = { 0,0 };

//		1.3		VARS
//	--- 	--- 	--- 	--- 	---
	float mouseSens = 2.0f;
	bool mouseIgnore = false;
	
//--- --- --- --- --- --- --- --- ---
//		2.	INTERNAL
//	--- 	--- 	--- 	--- 	---
	private:

//		2.1		CONTS
//	--- 	--- 	--- 	--- 	---
	SDL_Window* window;
	SDL_GLContext context;

//		2.2		VARS
//	--- 	--- 	--- 	--- 	---
	bool isClosed; int ww, wh;

};

//		666.	DONE	.999 || ->DS
//--- END CRK BLOCK --- --- --- --- --- --- --- ---

#endif // DISPLAY_H
