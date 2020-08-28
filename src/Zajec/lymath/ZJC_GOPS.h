#ifndef __ZJC_GOPS_H__
#define __ZJC_GOPS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../lyarr/ZJC_DynArray.h"

//  - --- - --- - --- - --- -

float iv2_distance(int a[2], int b[2]);

//  - --- - --- - --- - --- -

void  fv3_sub(float* a, float* b);
void  fv3_add(float* a, float* b);
void  fv3_cross(float* dest, float* a, float* b);
float fv3_length(float* vec);
void  fv3_normalize(float* vec);

//  - --- - --- - --- - --- -

int   maxi(int v1, int v2);
float maxf(float v1, float v2);

int   mini(int v1, int v2);
float minf(float v1, float v2);

int   submini(int v1, int v2);
float subminf(float v1, float v2);

int   submaxi(int v1, int v2);
float submaxf(float v1, float v2);

int   flipifi(int v1, int boo);
float flipiff(float v1, int boo);

//  - --- - --- - --- - --- -

int   clampi(int v, int start, int end);
float clampf(float v, float start, float end);

int   approai(int v1, int v2, int m);
int   approaf(float v1, float v2, float m);

int   approadi(int v1, int v2, int m);
float approadf(float v1, float v2, float m);

//  - --- - --- - --- - --- -

typedef struct fvRange {

    uint32_t mag;
    float    step;
    fArray*  values;

} fvRange;

fvRange* build_fvRange(uint mag, float step);
void     del_fvRange(fvRange* fvr);

uint32_t fvRange_take_closest(fvRange* fvr, float v);
uchar    fvRange_take_closest_1b(fvRange* fvr, float v);

//  - --- - --- - --- - --- -

#ifdef __cplusplus
}
#endif

#endif // __ZJC_GOPS_H__
