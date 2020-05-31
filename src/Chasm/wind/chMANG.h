#ifndef CH_MANG_H
#define CH_MANG_H

//	- --- - --- - --- - --- -

#include <vector>
#include "chWIN.h"
#include "Zajec.h"

typedef std::vector<chWIN*> chWinVec;

//	- --- - --- - --- - --- -

// SDL window manager
class chMANG {
	
	public:
		chMANG();
		~chMANG();

		void createWin(const char title[], int width, int height);
		chWIN* getCurWin() { return getWin(curwin); }
		chWIN* getWin(uint i) { if (i < this->wins.size()) { return this->wins[i]; } return nullptr; }

//	- --- - --- - --- - --- -

	private:
		chWinVec wins; // Collection of opened windows
		uint curwin = 0;

};

extern chMANG* chmang;

#endif // CH_MANG_H
