#include <iostream>

#include "chCONT.h"
#include "chMANG.h"
#include "GL/glew.h"

//	- --- - --- - --- - --- -

chCONT::chCONT() {

	time_internal = new lyspace::DS_TIME();

	chmang = new chMANG(); chmang->createWin("Test", 640, 480);
	context = SDL_GL_CreateContext(chmang->getCurWin()->getWin());

	GLenum status = glewInit();
	if (status != GLEW_OK) std::cerr << "GLEW DONE FUKK'D UP" << std::endl;

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

}

//	- --- - --- - --- - --- -

chCONT::~chCONT() {
	SDL_GL_DeleteContext(context);
	delete chmang;
	SDL_Quit();

	std::cout << "chcont out\n";
}

//	- --- - --- - --- - --- -

bool chCONT::winOpen() { return !chmang->getCurWin()->getIsClosed(); }
void chCONT::pollEvents() { chmang->getCurWin()->pollEvents(); }

//	- --- - --- - --- - --- -

void chCONT::chkwait() {
	if (time_internal->getSleep()) {
		SDL_Delay( (Uint32)time_internal->getDelta() );
	}
}
