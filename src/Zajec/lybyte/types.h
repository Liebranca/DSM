#ifndef LYBYTE_TYPES_H
#define LYBYTE_TYPES_H

//  - --- - --- - --- - --- -

#include "../unsigdefs.h"

//  - --- - --- - --- - --- -

namespace zjc {

struct byte                            {
    uchar bits = 0b0;
    byte(const int _bits): bits(_bits) {                                                            }
    byte(const bool _b[8])             { int b = 0; for (uint i = 0; i < 8; i++)
                                       { b += _b[i] >> 2 * i; }; bits = b;                          }
    bool operator [] (size_t i)        { return (bits & (1 << i));                                  }
    void operator = (int _bits)        { this->bits = _bits;                                        }
    void operator += (int _bits)       { this->bits += _bits;                                       }
    byte operator + (const int _bits)  { return bits + _bits;                                       }
    byte operator + (const byte other) { return byte(this->bits + other.bits);                      }
    explicit operator int()            { return this->bits;                                         }
    operator bool()                    { return bits;                                               }
                                                                                                    };

//  - --- - --- - --- - --- -
    
    int takebits(byte b1, uint iStart, uint iEnd);
    float frac16tofloat(byte b1, byte b2);

//  - --- - --- - --- - --- -

struct frac16                          {
    byte b1, b2;
    frac16() : b1(0b0), b2(0b0)        {                                                            }
    frac16(float v);
    frac16(int _b1, int _b2)
        : b1(_b1), b2(_b2)             {                                                            }

    byte& operator [] (size_t i)       { return *((byte*)this + i);                                 }
    explicit operator float() const    { return frac16tofloat(b1, b2);                              }
                                                                                                    };

                                                                                                    }
//  - --- - --- - --- - --- -
                                                                                                    
static  std::ostream& operator << (
        std::ostream& stream,
        zjc::byte& b)                  { stream << b.bits; return stream;                           }
static  std::ostream& operator << (
        std::ostream& stream,
        zjc::frac16& f)                { stream << (float)f; return stream;                         }

#endif //LYBYTE_TYPES_H
