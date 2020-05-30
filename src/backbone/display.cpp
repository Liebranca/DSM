#include "display.h"
#include <iostream>

int kFWD = SDLK_w; int kBCK = SDLK_s;
int kLFT = SDLK_a; int kRGT = SDLK_d;
int kSPC = SDLK_SPACE; int kLCTRL = SDLK_LCTRL;
int kUSE = SDLK_e;

int kESC = SDLK_ESCAPE;

DS_DISPLAY::DS_DISPLAY(const std::string& title, int width, int height)
{
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	//SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	//SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

	window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED,
	SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL);
	ww = width; wh = height;

	context = SDL_GL_CreateContext(window);
	SDL_ShowCursor(SDL_DISABLE);

	GLenum status = glewInit();
	if (status != GLEW_OK) std::cerr << "GLEW DONE FUKK'D UP" << std::endl;
	isClosed = false;

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

};

DS_DISPLAY::~DS_DISPLAY()
{
	SDL_ShowCursor(SDL_ENABLE);
	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();

};

bool DS_DISPLAY::getIsClosed() { return isClosed; }

void DS_DISPLAY::swapBuffers()
{
	SDL_GL_SwapWindow(window);

};

void DS_DISPLAY::mouseWrap() {
	/*int x = mousePos[0], y = mousePos[1];
	int CX = int(ww / 2); int CY = int(wh / 2);
	int step = 100, bstep = 25;
	if (x < step) { x = ww - step - bstep; }
	else if (x > ww - step) { x = step + bstep; }
	if (y < step) { y = wh - step - bstep; }
	else if (y > wh - step) { y = step + bstep; }*/

	SDL_WarpMouseInWindow(window, ww / 2, wh / 2);
	mouseIgnore = true; mouseRel = { 0,0 };

}

void DS_DISPLAY::pollEvents(){
	SDL_Event event; int k;
	if (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
			isClosed = true;
			break;
		case SDL_KEYDOWN:
			k = event.key.keysym.sym;
			if (k == kESC) { isClosed = true; }
			
			if (k == kFWD) { pkeys[0] = true; }
			else if (k == kBCK) { pkeys[1] = true; }
			
			if (k == kLFT) { pkeys[2] = true; }
			else if (k == kRGT) { pkeys[3] = true; }

			if (k == kSPC) { pkeys[4] = true; }
			else if (k == kLCTRL) { pkeys[5] = true; }

			if (k == kUSE) { pkeys[6] = true; }

			break;

		case SDL_KEYUP:
			k = event.key.keysym.sym;

			if (k == kFWD) { pkeys[0] = false; }
			else if (k == kBCK) { pkeys[1] = false; }

			if (k == kLFT) { pkeys[2] = false; }
			else if (k == kRGT) { pkeys[3] = false; }

			if (k == kSPC) { pkeys[4] = false; }
			else if (k == kLCTRL) { pkeys[5] = false; }

			if (k == kUSE) { pkeys[6] = helkeys[0] = false; }

			break;

		case SDL_MOUSEMOTION:
			mousePos[0] = event.motion.x; mousePos[1] = event.motion.y;
			if (!this->mouseIgnore) {
				mouseRel[0] = float(event.motion.xrel * mouseSens);
				mouseRel[1] = float(event.motion.yrel * mouseSens);
			}
				
			break;
		}
	}
}