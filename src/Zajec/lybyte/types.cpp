#include <vector>

#include "types.h"
#include "../lymath/iops.h"


namespace types {

//	- --- - --- - --- - --- -

	uint todecimal(byte b) {
		uint x = 0;
		for (uint i = 0; i < b.size(); i++) {
			x += b[i] * (int)pow(2, i);
		}

		return x;
	}

//	- --- - --- - --- - --- -

	std::vector<bool> tobits(uint x, uint size) {
		x = lymath::gmini(x, (int)pow(2, size) - 1);
		byte b;

		for (uint i = 0; i < size; i++) {
			b.push_back(x % 2);
			x = x / 2;
		}

		return b;
	}

//	- --- - --- - --- - --- -

	byte takebits(byte b1, uint iStart, uint iEnd) {
		// maybe we should check that iEnd <= 8
		// risking a segfault if some wanker doesn't know how to use this!
		
		byte b2; b2.resize(iEnd - iStart);
		for (unsigned int i = iStart; i < iEnd; i++) {
			b2[i] = b1[i];
		}
		return b2;
	}

//	- --- - --- - --- - --- -

	// lame to keep this one liner, it's only saving files.cpp a single call

	uint touint(byte b) { return todecimal(b); }

//	- --- - --- - --- - --- -

	float tofloat2(byte b1, byte b2) {
		
		// b1[0]: sign (+/-)
		// b1[1-8]: decimals
		// b2[0-1]: zeroes
		// b2[2-8]: integer

		bool sign = b1[0]; 
		byte floatbits = takebits(b1, 1, 8); 
		byte zerobits = { b2[0], b2[1] }; 
		byte intbits = takebits(b2, 2, 8); 

		// plain conversion for integer part; conversion times 1/128 for decimal part.
		// and why is 1/128 our magical number here? only Lyeb knows for sure

		uint ivalue = todecimal(intbits); float fvalue = (float)todecimal(floatbits) * 0.0078125f;

//		- --- - --- - --- - --- -

		// precision-less handling of zeroes in decimal part,
		// the two bits go like so:
		// 01: "round" down
		// 10: "round" up
		// 11: divide by ten

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

//		- --- - --- - --- - --- -

		// add decimal and integer. invert result if sign
		// sign goes like: (0/1) -> (+/-)
		float result = ivalue + fvalue;
		if (sign) { result *= -1; }

		return result;
	}
}