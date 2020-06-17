#ifndef __ZJC_BinTypes_H__
#define __ZJC_BinTypes_H__

#ifdef __cplusplus
extern "C" {
#endif

//  - --- - --- - --- - --- -

#include "../ZJC_CommonTypes.h"

//  - --- - --- - --- - --- -

typedef struct frac16 { 
    uchar b1;
    uchar b2;

} f16;

//  - --- - --- - --- - --- -

int nthbit(uchar b, int n);
int takebits(uchar b, uint iStart, uint iEnd);
float frac16_tofloat(f16* frac);
f16 float_tofrac16(float v);

int zjc_convertor_init();
int zjc_convertor_end();

//  - --- - --- - --- - --- -

typedef struct VertexPacked3D {
    f16 co[3];
    f16 no[3];
    f16 uv[2];

} VP3D;

VP3D build_vertpacked_3d(float* values);

typedef struct VertexPacked2D {
    f16 co[2];
    f16 uv[2];

} VP2D;

VP2D build_vertpacked_2d(float* values);

typedef struct PhysVertexPacked3D {
    f16 co[3];

} pVP3D;

pVP3D build_physvert_3d (float* values);

typedef struct PhysVertexPacked2D {
    f16 co[2];

} pVP2D;

pVP2D build_physvert_2d (float* values);

//  - --- - --- - --- - --- -

#ifdef __cplusplus
}
#endif

#endif //__ZJC_BinTypes_H__
