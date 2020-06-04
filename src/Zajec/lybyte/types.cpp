#include <iostream>
#include <string>
#include <vector>

#include "types.h"
#include "../lymath/gops.h"


namespace types {
	
//	- --- - --- - --- - --- -

	int takebits(byte b1, uint iStart, uint iEnd) {
		// maybe we should check that iEnd <= 8
		// risking a segfault if someone somehow doesn't realize how to use this!
		
		int x = 0b0;
		for (uint i = iStart; i < iEnd; i++)
		{ x += (b1[i] << i); } return x;
	}

//	- --- - --- - --- - --- -

	// lame to keep this one liner, it's only saving files.cpp a single call

	uint touint(byte b) { return (uint)b; }

//	- --- - --- - --- - --- -

	float frac16tofloat(byte b1, byte b2) {
		
		// b1[0]: sign (+/-)
		// b1[1-8]: decimals
		// b2[0-1]: zeroes
		// b2[2-8]: integer

		bool sign = b1[0];
		int floatbits = takebits(b1, 1, 8);
		byte zerobits = (b2[0], b2[1]);
		int intbits = takebits(b2, 2, 8);

		// plain conversion for integer part; conversion times 1/128 for decimal part.
		// and why is 1/128 our magical number here? only Lyeb knows for sure

		float fvalue = (float)floatbits * 0.0078125f;

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
		float result = intbits + fvalue;
		if (sign) { result *= -1; }

		return result;
	}
}