#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "../lymath/ZJC_GOPS.h"
#include "../lyutils/ZJC_Evil.h"
#include "ZJC_BinTypes.h"

fvRange frac8_range;
fvRange frac4_range;
fvRange frac16_range;
fvRange joj8_rg_range;
fvRange joj8_b_range;

uchar zjc_convertor_flags = 0;

int zjc_convertor_init(uchar flags)             {

    if (flags & BUILD_FRAC8)                    { frac8_range   = build_fvRange(128, 0.03125f);
                                                  frac4_range   = build_fvRange(16,  0.0625f);                      }
    if (flags & BUILD_FRAC16)                   { frac16_range  = build_fvRange(128, 0.0078125f);                   }
    if (flags & BUILD_JOJ8)                     { joj8_rg_range = build_fvRange(8, (float)1/8);
                                                  joj8_b_range  = build_fvRange(4, (float)1/4);                     }

    zjc_convertor_flags = flags;

    return 0;                                                                                                       }

int zjc_convertor_end()                         {

    if (zjc_convertor_flags & BUILD_FRAC8)      { del_fvRange(&frac8_range); del_fvRange(&frac4_range);             }
    if (zjc_convertor_flags & BUILD_FRAC16)     { del_fvRange(&frac16_range);                                       }
    if (zjc_convertor_flags & BUILD_JOJ8)       { del_fvRange(&joj8_rg_range); del_fvRange(&joj8_b_range);          }

    return 0;                                                                                                       }

//  - --- - --- - --- - --- -

VP3D_16 build_vertpacked_3d_16bit(float* values)     {

    VP3D_16 vert;
    vert.co[0] = float_tofrac16(*values);
    vert.co[1] = float_tofrac16(*(values+1));
    vert.co[2] = float_tofrac16(*(values+2));
    vert.no[0] = float_tofrac16(*(values+3));
    vert.no[1] = float_tofrac16(*(values+4));
    vert.no[2] = float_tofrac16(*(values+5));
    vert.uv[0] = float_tofrac16(*(values+6));
    vert.uv[1] = float_tofrac16(*(values+7));

    return vert;                                                                                                    }

VP3D_8 build_vertpacked_3d_8bit(float* values)      {

    VP3D_8 vert;
    vert.co[0] = float_tofrac8(*values);
    vert.co[1] = float_tofrac8(*(values+1));
    vert.co[2] = float_tofrac8(*(values+2));
    vert.uv    = float_tofrac4(*(values+3), *(values+4));

    return vert;                                                                                                    }

pVP3D_16 build_physvert_3d_16bit (float* values)     { 

    pVP3D_16 vert;
    vert.co[0] = float_tofrac16(*values);
    vert.co[1] = float_tofrac16(*(values+1));
    vert.co[2] = float_tofrac16(*(values+2));

    return vert;                                                                                                    }

pVP3D_8 build_physvert_3d_8bit (float* values)     { 

    pVP3D_8 vert;
    vert.co[0] = float_tofrac8(*values);
    vert.co[1] = float_tofrac8(*(values+1));
    vert.co[2] = float_tofrac8(*(values+2));

    return vert;                                                                                                    }

//  - --- - --- - --- - --- -

int nthbit(uchar b, int n)                      { return (b & (1 << n)) != 0;                                       }

int takebits(uchar b, uint iStart, uint iEnd)   {

    int x = 0b0; int bit;
    for (uint i = iStart; i < iEnd; i++)        { bit = nthbit(b, i); x += bit << i; }
    return x >> iStart;                                                                                             }

//  - --- - --- - --- - --- -

const int i = 3 & 0b000001;

float frac16_tofloat(f16 frac)                 {

    int sign      = nthbit(frac.b1, 0);
    int floatbits = takebits(frac.b1, 1, 8);
    int zerobits  = takebits(frac.b2, 0, 2);
    int intbits   = takebits(frac.b2, 2, 8);

    float fvalue = (float)floatbits * 0.0078125f;

//      - --- - --- - --- - --- -

    if (zerobits)                               {
        if      (zerobits == 1)                 { fvalue -= 0.001f;                                                 }
        else if (zerobits == 2)                 { fvalue += 0.001f;                                                 }
        else if (zerobits == 3)                 { fvalue *= 0.1f;                                                   }
                                                                                                                    }

//      - --- - --- - --- - --- -

    float result = intbits + fvalue;
    if (sign)                                   { result *= -1;                                                     }

    return result;                                                                                                  }

float frac8_tofloat(f8 frac)                   {

    float v = (frac >> 1) * 0.003125f;
    return v * (1 - (2 * (frac & 1)));                                                                              }

float* frac4_tofloat(f8 frac)                  {

    int   indices[2] = { takebits(frac, 0, 4), takebits(frac, 4, 8) };
    float v[2]       = { 0.0f,                 0.0f                 };

    for (uint i = 0; i < 2; i++)                { if (indices[i] == 15) { v[i] = 1.0f; }
                                                  else { v[i] = indices[i] * 0.0625f; }                             }

    return v;                                                                                                       }

//      - --- - --- - --- - --- -

f16 float_tofrac16(float v) {

    f16 frac = {0, 0};

    if (fabs(v) >= 64.0)                        { printf("%f is out of FRAC16 range (63.992188)", v);               }
    else
    {

        int sign = 0; if (v < 0) { sign = 1; }
        v = (float) fabs(v);
        char s[10]; snprintf(s, 9, "%f", v);

        int fromdot = 0;
        for(uint i = 0; i < 10; i++)            { if(s[i] == '.') { break; } fromdot++;                             }

        char decimals[7] = "000000";
        memcpy(decimals, &s[fromdot + 1], 6);

        char integer[3] = "00";
        int singledigit = fromdot == 1;
        memcpy(integer, &s, fromdot);
        if(singledigit)                         { integer[1] = '\0';                                                }

        uint closest = 0; int rounding = 0b00;
        char ws[9] = "0."; ws[8] = '\0';

        float w = (float) atof(strcat(ws, decimals));

        if (*decimals == '0')
        {
            if (*decimals != '0')               { closest = fvRange_take_closest(&frac16_range, w * 10);
                                                  rounding = 0b11;                                                  }
        }

        else
        { 
            closest = fvRange_take_closest(&frac16_range, w);
            float decimals_packed = *(frac16_range.values.buff + closest);

            if      (decimals_packed > w)       { rounding = 0b01;                                                  }
            else if (decimals_packed < w )      { rounding = 0b10;                                                  }
        }

        frac.b1 = (closest << 1) + sign;
        frac.b2 = (atoi(integer) << 2) + rounding;

    }

    return frac;
}

f8 float_tofrac8(float v)                       {

    f8    frac;
    uchar index = fvRange_take_closest_1b(&frac8_range, (float)fabs(v));

    frac = (index << 1) + (v < 0);

    return frac;                                                                                                    }

float* trinormal_8bit(f8 p1[3],
                      f8 p2[3],
                      f8 p3[3])                 {

    float p1_unpack[3]   = { frac8_tofloat(p1[0]), frac8_tofloat(p1[1]), frac8_tofloat(p1[2])                       };
    float p2_unpack[3]   = { frac8_tofloat(p2[0]), frac8_tofloat(p2[1]), frac8_tofloat(p2[2])                       };
    float p3_unpack[3]   = { frac8_tofloat(p3[0]), frac8_tofloat(p3[1]), frac8_tofloat(p3[2])                       };

    return fv3_normalize(fv3_cross(fv3_sub(p2_unpack, p1_unpack), fv3_sub(p3_unpack, p1_unpack)));                  }

f8*    sumtrinormals_8bit(float face_normals[3],
                          uint len)             {

    f8 vertex_normal[3] = { 0, 0, 0 };

    float dlen          = (float)1/len;
    vertex_normal[0]    = float_tofrac8(face_normals[0] * dlen);
    vertex_normal[1]    = float_tofrac8(face_normals[1] * dlen);
    vertex_normal[2]    = float_tofrac8(face_normals[2] * dlen);

    return vertex_normal;                                                                                           }

f8 float_tofrac4(float v1, float v2)            {

    f8 frac;

    uchar index1 = fvRange_take_closest_1b(&frac4_range, v1);
    uchar index2 = fvRange_take_closest_1b(&frac4_range, v2);

    frac = index1 + (index2 << 4);

    return frac;                                                                                                    }

//      - --- - --- - --- - --- -

uchar color_to_joj8(float r,
                    float g,
                    float b)                    {

    uchar color =   (fvRange_take_closest_1b(&joj8_rg_range, r)      )
                  + (fvRange_take_closest_1b(&joj8_rg_range, g) << 3 )
                  + (fvRange_take_closest_1b(&joj8_b_range,  b) << 6 );

    return color;                                                                                                   }

//      - --- - --- - --- - --- -

