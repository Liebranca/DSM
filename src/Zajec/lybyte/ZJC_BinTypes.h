#ifndef __ZJC_BinTypes_H__
#define __ZJC_BinTypes_H__

#ifdef __cplusplus
extern "C" {
#endif

//  - --- - --- - --- - --- -

#include "../ZJC_CommonTypes.h"

#define   BUILD_FRAC  0x01
#define   BUILD_JOJ   0x02

//  - --- - --- - --- - --- -

typedef struct JOJ_COLOR_COMPRESSED {

    uchar chroma_u;
    uchar chroma_v;
    uchar luma;

} JOJPIX;

//  - --- - --- - --- - --- -

JOJPIX rgb_to_joj        (float r, float g, float b);
void   joj_to_rgb        (float pixel[3], JOJPIX* joj);

uchar  bitsize           (uchar x);
uchar  usbitsize         (ushort x);

int    nthbit            (uchar b, int n);
int    uinthbit          (uint b, int n);

int    takebits          (uchar b, uint iStart, uint iEnd);

float  frac_tofloat      (uchar frac);
float  ufrac_tofloat     (uchar frac);
uchar  float_tofrac      (float v);
uchar  float_toufrac     (float v);

int    zjc_convertor_init(uchar flags);
int    zjc_convertor_end ();

//  - --- - --- - --- - --- -

typedef struct VertexPacked3D_8bit {
    uchar co[3];
    uchar normal[3];
    uchar tangent[3];
    uchar bitangent[3];
    uchar uv[2];

} VP3D_8;

VP3D_8 build_vertpacked_3d_8bit(float* values);

typedef struct PhysVertexPacked3D_8bit {
    uchar co[3];

} pVP3D_8;

pVP3D_8 build_physvert_3d_8bit (float* values);

//  - --- - --- - --- - --- -

#ifdef __cplusplus
}
#endif

#endif //__ZJC_BinTypes_H__
