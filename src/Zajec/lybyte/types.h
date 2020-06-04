#ifndef LYBYTE_TYPES_H
#define LYBYTE_TYPES_H

//  - --- - --- - --- - --- -

#include "../lyutils/evil.h"

static size_t arrsize(int arr[])       { return (sizeof(arr) / sizeof(arr[0]));                     }

struct byte                            {
    uchar bits = 0b0;
    byte(const int _bits): bits(_bits) {                                                            }
    byte(const bool _b[8])             { int b = 0; for (uint i = 0; i < 8; i++)
                                       { b += _b[i] >> 2 * i; }; bits = b;                          }
    size_t size()                      { return mbiton({0xFF});                                     }
    bool operator [] (size_t i)        { return (bits & (1 << i));                                  }
    void operator = (int _bits)        { this->bits = _bits;                                        }
    void operator += (int _bits)       { this->bits += _bits;                                       }
    byte operator + (const int _bits)  { return bits + _bits;                                       }
    byte operator + (const byte other) { return byte(this->bits + other.bits);                      }
    explicit operator int()            { return this->bits;                                         }
    operator bool()                    { return bits;                                               }
    size_t mbiton(int mask)            { std::vector<bool> masked;
                                         for (uint i = 0; i < 8; i++)
                                       { if  (mask & (1 << i)) { masked.push_back(0x1); }           }
                                         return masked.size();                                      }

                                                                                                    };

//  - --- - --- - --- - --- -

struct bytearray                       {
    std::vector<byte> bytes;
                                                                                                    };
typedef std::vector<uchar> binfile;

//  - --- - --- - --- - --- -

namespace types {
    
    //  Takes sub-array from a binary array
    int takebits(byte b1, uint iStart, uint iEnd);

    //  Converts binary array to uint
    uint touint(byte b);

    //  Converts two binary arrays to a fractionary number
    float frac16tofloat(byte b1, byte b2);

}

//  - --- - --- - --- - --- -

namespace lybyte                       { using namespace types;                                     }

struct frac16                          {
    byte b1; byte b2;
    frac16(int _b1, int _b2)
        : b1(_b1), b2(_b2)             {                                                            }
    byte& operator [] (size_t i)       { return *((byte*)this + i);                                 }
    explicit operator float() const    { return lybyte::frac16tofloat(b1, b2);                      }
                                                                                                    };

//  - --- - --- - --- - --- -

// Printing methods for custom types

static std::ostream& operator << (
    std::ostream& stream, byte& b)     { stream << b.bits; return stream;                           }

static std::ostream& operator << (
    std::ostream& stream, frac16& f)   { stream << (float)f; return stream;                         }

#endif //LYBYTE_TYPES_H
