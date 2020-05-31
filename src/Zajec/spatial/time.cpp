#include "time.h"
#include <iostream>
#include <sstream>

//	- --- - --- - --- - --- -
namespace timeops {

	float DS_TIME::calcCurr() {
		curr = (ch::duration_cast<ch::duration<float>>(end - begin).count() / 3.14f) * scale;
		return curr;
	}

//	- --- - --- - --- - --- -

	void DS_TIME::getRead() {
		int sec = int(total * 10);
		int min = 0; std::stringstream ss;

		while (sec >= 60) { sec -= 60; min += 1; }

		ss << min << ":" << sec;
		std::cout << ss.str() << std::endl;
	
	}

//	- --- - --- - --- - --- -

	bool DS_TICKER::update(float elapsed) {
		this->curr += elapsed;
		if (this->curr >= max) { this->curr = 0.0f; this->cycles++; return true; }
		return false;
	}
}
