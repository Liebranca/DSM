#include "time.h"
#include <sstream>

std::string DS_TIME::getRead() {
	int sec = int(total * 10);
	int min = 0; std::stringstream ss;

	while (sec >= 60) { sec -= 60; min += 1; }

	ss << min << ":" << sec;
	return ss.str();
	
}