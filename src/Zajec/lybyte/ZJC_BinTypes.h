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

} frac16;

//  - --- - --- - --- - --- -

int nthbit(uchar b, int n);
int takebits(uchar b, uint iStart, uint iEnd);
float frac16_tofloat(frac16* frac);
frac16 float_tofrac16(float v);

int zjc_convertor_init();
int zjc_convertor_end();

//  - --- - --- - --- - --- -

typedef struct VertexPacked3D {
    frac16 co[3];
    frac16 no[3];
    frac16 uv[2];

} VertexPacked3D;

VertexPacked3D build_vertpacked_3d(float* values);

typedef struct PhysVertexPacked3D {
    frac16 co[3];

} PhysVertexPacked3D;

PhysVertexPacked3D build_physvert_3d (float* values);

//  - --- - --- - --- - --- -

#ifdef __cplusplus
}
#endif

#endif //__ZJC_BinTypes_H__
