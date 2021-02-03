#ifndef __ZJC_GOPS_H__
#define __ZJC_GOPS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../ZJC_CommonTypes.h"

//  - --- - --- - --- - --- -

float iv2_distance(int a[2], int b[2]);

//  - --- - --- - --- - --- -

void  fv3_sub(float* a, float* b);
void  fv3_add(float* a, float* b);
void  fv3_cross(float* dest, float* a, float* b);
float fv3_length(float* vec);
void  fv3_normalize(float* vec);

//  - --- - --- - --- - --- -

// (max_int) Get greater of (v1, v2)
sint maxi(sint v1, sint v2);

// (max_float) Get greater of (v1, v2)
float maxf(float v1, float v2);

// (min_int) Get lesser of (v1, v2)
sint mini(sint v1, sint v2);

// (min_float) Get lesser of (v1, v2)
float minf(float v1, float v2);

// (submin_int) Returns max(v1, v2) minus min(v1, v2)
sint submini(sint v1, sint v2);

// (submin_float) Returns max(v1, v2) minus min(v1, v2)
float subminf(float v1, float v2);

// (submax_int) Returns min(v1, v2) minus max(v1, v2)
sint submaxi(sint v1, sint v2);

// (submax_float) Returns min(v1, v2) minus max(v1, v2)
float submaxf(float v1, float v2);

// (flip_int_if) Inverts sign of v1 if boo
sint flipifi(sint v1, sint boo);

// (flip_float_if) Inverts sign of v1 if boo
float flipiff(float v1, sint boo);

// Round x to nearest integer (APPARENTLY)
float fround(float x);

//  - --- - --- - --- - --- -

// (clamp_int) Clamp v to range(start, end)
sint clampi(sint v, sint start, sint end);

// (clamp_float) Clamp v to range(start, end)
float clampf(float v, float start, float end);

// (wrap_int) Add (-/+) steps to v and loop around range(start, end)
sint wrapi(sint v, sint steps, sint start, sint end);

// (approach_int) Get v1 is at least (-/+) m distance of v2
sint approai(sint v1, sint v2, sint m);

// (approach_float) Get v1 is at least (-/+) m distance of v2
sint approaf(float v1, float v2, float m);

// (approach_distance_int) Get distance if v1 is at least (-/+) m distance of v2
sint approadi(sint v1, sint v2, sint m);

// (approach_distance_float) Get distance if v1 is at least (-/+) m distance of v2
float approadf(float v1, float v2, float m);

//  - --- - --- - --- - --- -

typedef struct fvRange {

    uint   mag;
    float  step;
    float* buff;

} fvRange;

void     build_fvRange(fvRange* fvr, uint mag, float step);
void     del_fvRange  (fvRange* fvr);

uint     fvRange_take_closest(fvRange* fvr, float v);
uchar    fvRange_take_closest_1b(fvRange* fvr, float v);

//  - --- - --- - --- - --- -

#ifdef __cplusplus
}
#endif

#endif // __ZJC_GOPS_H__
