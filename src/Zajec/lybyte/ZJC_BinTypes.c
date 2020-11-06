#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "../lymath/ZJC_GOPS.h"
#include "../lyutils/ZJC_Evil.h"
#include "ZJC_BinTypes.h"

static fvRange frac_range           = { 0 };
static fvRange ufrac_range          = { 0 };

static fvRange joj_luma_range       = { 0 };
static fvRange joj_chroma_range     = { 0 };

static uchar   zjc_convertor_flags  = 0;

int zjc_convertor_init(uchar flags)             {

    if (flags & BUILD_FRAC)                     { build_fvRange(&frac_range,       128, 0.03125f   );
                                                  build_fvRange(&ufrac_range,      256, 0.00390625f);               }

    if (flags & BUILD_JOJ)                      { build_fvRange(&joj_luma_range,   64,  0.015625f);
                                                  build_fvRange(&joj_chroma_range, 64, -0.03125f);               }

    zjc_convertor_flags = flags;

    return 0;                                                                                                       }

int zjc_convertor_end()                         {

    if (zjc_convertor_flags & BUILD_FRAC)       { del_fvRange(&ufrac_range     ); del_fvRange(&frac_range    );     }
    if (zjc_convertor_flags & BUILD_JOJ)        { del_fvRange(&joj_chroma_range); del_fvRange(&joj_luma_range);     }

    return 0;                                                                                                       }

//  - --- - --- - --- - --- -

VP3D_8 build_vertpacked_3d_8bit(float* values)      {

    VP3D_8 vert;
    vert.co[0]        = float_tofrac  (*(values+0));
    vert.co[1]        = float_tofrac  (*(values+1));
    vert.co[2]        = float_tofrac  (*(values+2));
                      
    vert.normal[0]    = float_tofrac  (*(values+3));
    vert.normal[1]    = float_tofrac  (*(values+4));
    vert.normal[2]    = float_tofrac  (*(values+5));

    vert.tangent[0]   = float_tofrac  (*(values+6));
    vert.tangent[1]   = float_tofrac  (*(values+7));
    vert.tangent[2]   = float_tofrac  (*(values+8));

    vert.bitangent[0] = float_tofrac  (*(values+9));
    vert.bitangent[1] = float_tofrac  (*(values+10));
    vert.bitangent[2] = float_tofrac  (*(values+11));

    vert.uv[0]        = float_toufrac (*(values+12));
    vert.uv[1]        = float_toufrac (*(values+13));

    return vert;                                                                                                    }

pVP3D_8 build_physvert_3d_8bit (float* values)     { 

    pVP3D_8 vert;
    vert.co[0] = float_tofrac(*values);
    vert.co[1] = float_tofrac(*(values+1));
    vert.co[2] = float_tofrac(*(values+2));

    return vert;                                                                                                    }

//  - --- - --- - --- - --- -

uchar bitsize (uchar x)                         {

    uchar bsize = 0;
    uchar trail = 0;

    for(uchar b = 0; b < 8; b++)                { uchar bitset = nthbit(x, b);
                                                  if(bitset) { bsize++; bsize += trail; trail = 0; }
                                                  else       { trail++;                            }                }

    return bsize;                                                                                                   }

uchar usbitsize (ushort x)                      {

    uchar bsize = 0;
    uchar trail = 0;

    for(uchar b = 0; b < 16; b++)               { uchar bitset = uinthbit(x, b);
                                                  if(bitset) { bsize++; bsize += trail; trail = 0; }
                                                  else       { trail++;                            }                }

    return bsize;                                                                                                   }

int   nthbit    (uchar b, int n)                { return (b & (1 << n)) != 0;                                       }
int   uinthbit  (uint  b, int n)                { return (b & (1 << n)) != 0;                                       }

int takebits(uchar b, uint iStart, uint iEnd)   {

    int x = 0b0; int bit;
    for (uint i = iStart; i < iEnd + 1; i++)    { bit = nthbit(b, i); if(bit) { x |= bit << (i - iStart); }         }
    return x;                                                                                                       }

//  - --- - --- - --- - --- -

float frac_tofloat(uchar frac)                  { uchar i = frac >> 1; if(i == 127) { i++; }

                                                  float  v = i * 0.03125f;
                                                  return v * (1 - (2 * (frac & 1)));                                }

float ufrac_tofloat(uchar frac)                 { return frac * 0.00390625f;                                        }

//      - --- - --- - --- - --- -

uchar float_tofrac(float v)                     { uchar frac;

                                                  uchar index = fvRange_take_closest_1b(&frac_range, (float)fabs(v));
                                                  frac = (index << 1) + (v < 0); return frac;                       }

uchar float_toufrac(float v)                    { uchar frac = fvRange_take_closest_1b(&ufrac_range, v);
                                                  return frac;                                                      }

//      - --- - --- - --- - --- -

JOJPIX rgba_to_joj  (float r,
                     float g,
                     float b,
                     float a)                   {

    JOJPIX joj = { 0 };

    joj.luma     = fvRange_take_closest_1b(&joj_luma_range,   ( 0.257f * r) + (0.504f * g) + (0.098f * b));
    joj.chroma_u = fvRange_take_closest_1b(&joj_chroma_range, (-0.148f * r) - (0.291f * g) + (0.439f * b));
    joj.chroma_v = fvRange_take_closest_1b(&joj_chroma_range, ( 0.439f * r) - (0.368f * g) - (0.071f * b));
    joj.alpha    = fvRange_take_closest_1b(&joj_luma_range, a                                            );

    return joj;                                                                                                     }

void  joj_to_rgba  (float pixel[4],
                    JOJPIX* joj)                {

    float luma        = ( joj->luma     * 0.015625f  ) * 1.164000f;
    float chroma_u    = ( joj->chroma_u - 32         ) * 0.03125f;
    float chroma_v    = ( joj->chroma_v - 32         ) * 0.03125f;
    float alpha       = ( joj->alpha    * 0.015625f  );

    pixel[0]          = (luma) + (1.596f * chroma_v);
    pixel[1]          = (luma) - (0.392f * chroma_u) - (0.813f * chroma_v);
    pixel[2]          = (luma) + (2.017f * chroma_u);
    pixel[4]          = alpha;                                                                                          }

//      - --- - --- - --- - --- -

