#include "chWIN.h"
#include <iostream>

//	- --- - --- - --- - --- -

int kFWD = SDLK_w; int kBCK = SDLK_s;
int kLFT = SDLK_a; int kRGT = SDLK_d;
int kSPC = SDLK_SPACE; int kLCTRL = SDLK_LCTRL;
int kUSE = SDLK_e;

int kESC = SDLK_ESCAPE;

//	- --- - --- - --- - --- -

chWIN::chWIN(const char title[], int width, int height)
{
	window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED,
	SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL);
	ww = width; wh = height;

	isClosed = false;

};

//	- --- - --- - --- - --- -

chWIN::~chWIN()
{
	SDL_DestroyWindow(window);
};

//	- --- - --- - --- - --- -

void chWIN::mouseWrap() {
	SDL_WarpMouseInWindow(window, ww / 2, wh / 2);
	mouseIgnore = true; mouseRel[0] = 0; mouseRel[1] = 0;

}

//	- --- - --- - --- - --- -

void chWIN::pollEvents(){
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