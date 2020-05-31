#ifndef CH_WIN_H
#define CH_WIN_H

//	- --- - --- - --- - --- -

#include "SDL/SDL.h"

//	- --- - --- - --- - --- -

// Baseline SDL window wrap
class chWIN
{
	public:

	chWIN(const char title[], int height, int width);
	virtual ~chWIN();

	void swapBuffers() { SDL_GL_SwapWindow(window); }
	SDL_Window* getWin() { return window; }

	void hideCursor() { SDL_ShowCursor(SDL_DISABLE); }
	void showCursor() { SDL_ShowCursor(SDL_ENABLE); }

	bool getIsClosed() { return isClosed; }

//	- --- - --- - --- - --- -

	void pollEvents();
	void mouseWrap();

//	- --- - --- - --- - --- -

	bool pkeys[7] = { 0,0,0,0,0,0,0 };
	bool helkeys[1] = { 0 };
	float mouseRel[2] = { 0,0 };
	int mousePos[2] = { 0,0 };

//	- --- - --- - --- - --- -
	float mouseSens = 2.0f;
	bool mouseIgnore = false;
	
//	- --- - --- - --- - --- -

	private:

	SDL_Window* window;
	SDL_GLContext context;
	bool isClosed; int ww, wh;

};

#endif // CH_WIN_H
