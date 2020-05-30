#ifndef LYBYTE_TYPES_H
#define LYBYTE_TYPES_H

//	· --- · --- · --- · --- ·

#include <vector>
#include "../Zajec.h"

//	· --- · --- · --- · --- ·

namespace types {
	uint todecimal(byte b);
	byte tobits(uint x, uint size = 8);
	byte takebits(byte b1, uint iStart, uint iEnd);

	uint uint2(byte b);
	uint uint3(byte b);
	float float2(byte b1, byte b2);

}

//	· --- · --- · --- · --- ·

namespace lybyte {
	using namespace types;
}

#endif //LYBYTE_TYPES_H
