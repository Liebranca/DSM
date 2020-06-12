#ifndef __ZJC_BinTypes_H__
#define __ZJC_BinTypes_H__

//  - --- - --- - --- - --- -

#include "../ZJC_CommonTypes.h"

//  - --- - --- - --- - --- -

extern "C" namespace zjc {

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

struct bytechunk                       {
    ushort size;
    ushort position;
    cuchar* values;
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

struct VertexPacked3D                  {                                                            
    frac16 co[3] = {0, 0, 0};
    frac16 no[3] = {0, 0, 0};
    frac16 uv[2] = {0, 0};

    VertexPacked3D()                   {                                                            }
    VertexPacked3D(float* values)      { co[0] = *values;     co[1] = *(values+1);
                                         co[2] = *(values+2); no[0] = *(values+3);
                                         no[1] = *(values+4); no[2] = *(values+5);
                                         uv[0] = *(values+6); uv[1] = *(values+7);                  }
                                                                                                    };

struct PhysVertexPacked3D              {                                                            
    frac16 co[3] = {0, 0, 0};

    PhysVertexPacked3D()               {                                                            }
    PhysVertexPacked3D(float* values)  { co[0] = *values;     co[1] = *(values+1);
                                         co[2] = *(values+2);                                       }
                                                                                                    };

struct VertexPacked2D                  {                                                            
    frac16 co[2] = {0, 0};
    frac16 uv[2] = {0, 0};

    VertexPacked2D(float* values)      { co[0] = *values;     co[1] = *(values+1);
                                         co[2] = *(values+2); uv[0] = *(values+3);
                                         uv[1] = *(values+4);                                       }
                                                                                                    };

struct PhysVertexPacked2D              {                                                            
    frac16 co[3] = {0, 0};

    PhysVertexPacked2D(float* values)  { co[0] = *values;     co[1] = *(values+1);                  }
                                                                                                    };

                                                                                                    }
//  - --- - --- - --- - --- -

#endif //__ZJC_BinTypes_H__
