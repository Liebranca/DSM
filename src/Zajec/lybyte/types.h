#ifndef LYBYTE_TYPES_H
#define LYBYTE_TYPES_H

//	- --- - --- - --- - --- -

#include <vector>
#include "../Zajec.h"

static size_t arrsize(int arr[])	   { return (sizeof(arr) / sizeof(arr[0]));						}

struct byte							   {
	int bits = 0b0;
	byte(const int _bits): bits(_bits) {                                                            }
	byte(bool _b[8])				   { int b = 0; for (uint i = 0; i < 8; i++)
									   { b += _b[i] >> 2 * i; }; bits = b;                          }
	size_t size()					   { return mbiton({0xFF});										}
	bool operator [] (size_t i)        { return (bits & (1 << i));									}
	void operator = (int _bits)		   { this->bits = _bits;										}
	void operator += (int _bits)	   { this->bits += _bits;										}
	explicit operator int()			   { return bits;												}
	//explicit operator bool()		   { return (uint)bits;											}
	operator bool()					   { return bits;												}
	/*std::vector<bool> mbiton(int is[]) { std::vector<bool> _bits;
										 size_t size = arrsize(is); _bits.resize(size);
										 for (uint i = 0; i < size; i++) { _bits[i] = bits[i];		}
										 return _bits;												}*/
	size_t mbiton(int mask)			   { std::vector<bool> masked;
										 for (uint i = 0; i < 8; i++)
									   { if  (mask & (1 << i)) { masked.push_back(0x1); }	        }
										 return masked.size();										}

																									};

static std::ostream& operator << (
	std::ostream& stream, byte& b)	   { stream << b.bits; return stream;							}

typedef std::vector<byte> bytearray;
typedef std::vector<uchar> binfile;

//	- --- - --- - --- - --- -

namespace types {
	
	//	Takes sub-array from a binary array
	int takebits(byte b1, uint iStart, uint iEnd);

	//	Converts binary array to uint
	uint touint(byte b);

	//	Converts two binary arrays to a fractionary number
	float tofloat2(byte b1, byte b2);

}

//	- --- - --- - --- - --- -

namespace lybyte {
	using namespace types;
}

struct frac {
	byte b1; byte b2;

	frac(int _b1, int _b2) : b1(_b1), b2(_b2) {}

	byte& operator [] (size_t i) { return *((byte*)this + i); }
	explicit operator float() const { return lybyte::tofloat2(b1, b2); }
};

#endif //LYBYTE_TYPES_H
