#ifndef __ZJC_BinTypes_H__
#define __ZJC_BinTypes_H__

#ifdef __cplusplus
extern "C" {
#endif

//  - --- - --- - --- - --- -

#include "../ZJC_CommonTypes.h"

#define   BUILD_FRAC8  0x01
#define   BUILD_FRAC16 0x02
#define   BUILD_JOJ8   0x04
#define   BUILD_JOJ16  0x08

//  - --- - --- - --- - --- -

typedef struct frac16 { 
    uchar b1;
    uchar b2;

} f16;

typedef uchar frac8, f8;

//  - --- - --- - --- - --- -

uchar  color_to_joj8     (float r, float g, float b);
ushort color_to_joj16    (float r, float g, float b);


int    nthbit            (uchar b, int n);
int    takebits          (uchar b, uint iStart, uint iEnd);

float  frac16_tofloat    (f16 frac);
f16    float_tofrac16    (float v);

float  frac8_tofloat     (f8 frac);
void   frac4_tofloat     (float* dest, f8 frac);
f8     float_tofrac8     (float v);
f8     float_tofrac4     (float v1, float v2);

void   trinormal_8bit    (float* dest, f8 p1[3], f8 p2[3], f8 p3[3]);
void   sumtrinormals_8bit(float* dest, float* normals, uint len);

int    zjc_convertor_init(uchar flags);
int    zjc_convertor_end ();

//  - --- - --- - --- - --- -

typedef struct VertexPacked3D_16bit {
    f16 co[3];
    f16 no[3];
    f16 uv[2];

} VP3D_16;

VP3D_16 build_vertpacked_3d_16bit(float* values);

typedef struct VertexPacked3D_8bit {
    f8 co[3];
    f8 normal[3];
    f8 tangent[3];
    f8 bitangent[3];
    f8 uv[2];

} VP3D_8;

VP3D_8 build_vertpacked_3d_8bit(float* values);

typedef struct PhysVertexPacked3D_16bit {
    f16 co[3];

} pVP3D_16;

pVP3D_16 build_physvert_3d_16bit (float* values);

typedef struct PhysVertexPacked3D_8bit {
    f8 co[3];

} pVP3D_8;

pVP3D_8 build_physvert_3d_8bit (float* values);

//  - --- - --- - --- - --- -

#ifdef __cplusplus
}
#endif

#endif //__ZJC_BinTypes_H__
