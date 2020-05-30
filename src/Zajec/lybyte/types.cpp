#include <vector>

#include "types.h"
#include "../lymath/iops.h"


namespace types {

//		· --- · --- · --- · --- ·

	uint todecimal(byte b) {
		uint x = 0;
		for (uint i = 0; i < b.size(); i++) {
			x += b[i] * (int)pow(2, i);
		}

		return x;
	}

//		· --- · --- · --- · --- ·

	std::vector<bool> tobits(uint x, uint size) {
		x = lymath::gmini(x, (int)pow(2, size) - 1);
		byte b;

		for (uint i = 0; i < size; i++) {
			b.push_back(x % 2);
			x = x / 2;
		}

		return b;
	}

//		· --- · --- · --- · --- ·

	byte takebits(byte b1, uint iStart, uint iEnd) {
		byte b2; b2.resize(iStart - iEnd);
		for (unsigned int i = iStart; i < iEnd; i++) {
			b2[i] = b1[i];
		}
		return b2;
	}

//		· --- · --- · --- · --- ·

	uint uint2(byte b) { return todecimal(b); }
	uint uint3(byte b) { return todecimal(b); }

//		· --- · --- · --- · --- ·

	float float2(byte b1, byte b2) {
		bool sign = b1[0];

		byte floatbits = takebits(b1, 1, 8);
		byte zerobits = { b2[0], b2[1] };
		byte intbits = takebits(b2, 2, 8);

		uint ivalue = todecimal(intbits); float fvalue = (float)todecimal(floatbits) * 0.0078125f;

		if (zerobits[0] || zerobits[1]) {
			if (zerobits[0] && !zerobits[1]) {
				fvalue -= 0.001f;
			}
			else if (!zerobits[0] && zerobits[1]) {
				fvalue += 0.001f;
			}
			else if (zerobits[0] && zerobits[1]) {
				fvalue *= 0.1f;
			}
		}

		float result = ivalue + fvalue;
		if (sign) { result *= -1; }

		return result;
	}
}