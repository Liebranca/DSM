#ifndef DISPLAY_H
#define DISPLAY_H

//	- --- - --- - --- - --- -

#include <string>
#include <vector>
#include "SDL/SDL.h"
#include "GL/glew.h"
#include "glm/glm.hpp"

//	- --- - --- - --- - --- -

class DS_DISPLAY
{
	public:

	DS_DISPLAY(const std::string& title, int height, int width);
	virtual ~DS_DISPLAY();
	bool getIsClosed();

	void swapBuffers();
	SDL_Window* getWin() { return window; }

//	- --- - --- - --- - --- -

	void pollEvents();
	void mouseWrap();

//	- --- - --- - --- - --- -

	std::vector<bool> pkeys = { 0,0,0,0,0,0,0 };
	std::vector<bool> helkeys = { 0 };
	std::vector<float> mouseRel = { 0,0 };
	std::vector<int> mousePos = { 0,0 };

//	- --- - --- - --- - --- -
	float mouseSens = 2.0f;
	bool mouseIgnore = false;
	
//	- --- - --- - --- - --- -

	private:

	SDL_Window* window;
	SDL_GLContext context;
	bool isClosed; int ww, wh;

};

#endif // DISPLAY_H
