#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "../lymath/ZJC_GOPS.h"
#include "../lyutils/ZJC_Evil.h"
#include "ZJC_BinTypes.h"

fvRange frac16_range;

int zjc_convertor_init()                        { frac16_range = build_fvRange(128, 0.0078125f); return 0;          }
int zjc_convertor_end()                         { del_fvRange(&frac16_range); return 0;                             }

//  - --- - --- - --- - --- -

VP3D build_vertpacked_3d(float* values)         {

    VP3D vert;
    vert.co[0] = float_tofrac16(*values);
    vert.co[1] = float_tofrac16(*(values+1));
    vert.co[2] = float_tofrac16(*(values+2));
    vert.no[0] = float_tofrac16(*(values+3));
    vert.no[1] = float_tofrac16(*(values+4));
    vert.no[2] = float_tofrac16(*(values+5));
    vert.uv[0] = float_tofrac16(*(values+6));
    vert.uv[1] = float_tofrac16(*(values+7));

    return vert;                                                                                                    }

VP2D build_vertpacked_2d(float* values)         {

    VP2D vert;
    vert.co[0] = float_tofrac16(*values);
    vert.co[1] = float_tofrac16(*(values+1));
    vert.uv[0] = float_tofrac16(*(values+2));
    vert.uv[1] = float_tofrac16(*(values+3));

    return vert;                                                                                                    }

pVP3D build_physvert_3d (float* values)         { 

    pVP3D vert;
    vert.co[0] = float_tofrac16(*values);
    vert.co[1] = float_tofrac16(*(values+1));
    vert.co[2] = float_tofrac16(*(values+2));

    return vert;                                                                                                    }

pVP2D build_physvert_2d (float* values)         {

    pVP2D vert;
    vert.co[0] = float_tofrac16(*values);
    vert.co[1] = float_tofrac16(*(values+1));

    return vert;                                                                                                    }

//  - --- - --- - --- - --- -

int nthbit(uchar b, int n)                      { return (b & (1 << n)) != 0;                                       }

int takebits(uchar b, uint iStart, uint iEnd)   {

    int x = 0b0; int bit;
    for (uint i = iStart; i < iEnd; i++)        { bit = nthbit(b, i); x += bit << i; }
    return x >> iStart;                                                                                             }

//  - --- - --- - --- - --- -

float frac16_tofloat(f16* frac)                 {

    int sign      = nthbit(frac->b1, 0);
    int floatbits = takebits(frac->b1, 1, 8);
    int zerobits  = takebits(frac->b2, 0, 2);
    int intbits   = takebits(frac->b2, 2, 8);

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
