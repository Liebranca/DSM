#include "chMANG.h"
#include <iostream>

chMANG* chmang = nullptr;

//	- --- - --- - --- - --- -

chMANG::chMANG() {
	
	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);

}

//	- --- - --- - --- - --- -

chMANG::~chMANG() {
	for (uint i = 0; i < this->wins.size(); i++) {
		delete this->wins[i];
	}
	std::cout << "chmang out\n";
}

//	- --- - --- - --- - --- -

void chMANG::createWin(const char title[], int width, int height) {
	wins.push_back(new chWIN(title, width, height));
}

//	- --- - --- - --- - --- -