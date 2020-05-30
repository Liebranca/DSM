#ifndef LYBYTE_TYPES_H
#define LYBYTE_TYPES_H

//	- --- - --- - --- - --- -

#include <vector>
#include "../Zajec.h"

//	- --- - --- - --- - --- -

namespace types {

	//	Converts binary array to uint
	uint todecimal(byte b);

	//	Converts uint to binary array
	byte tobits(uint x, uint size = 8);

	//	Takes sub-array from a binary array
	byte takebits(byte b1, uint iStart, uint iEnd);

	//	Converts binary array to uint
	uint touint(byte b);

	//	Converts two binary arrays to a fractionary number
	float tofloat2(byte b1, byte b2);

}

//	- --- - --- - --- - --- -

namespace lybyte {
	using namespace types;
}

#endif //LYBYTE_TYPES_H
