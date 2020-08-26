#include "ZJC_GOPS.h"
#include <stdio.h>
#include <math.h>

//  - --- - --- - --- - --- -

void fv3_sub(float* a, float* b)        { a[0] = a[0] - b[0];
                                          a[1] = a[1] - b[1],
                                          a[2] = a[2] - b[2];                                               }

void fv3_add(float* a, float* b)        { a[0] = a[0] + b[0];
                                          a[1] = a[1] + b[1],
                                          a[2] = a[2] + b[2];                                               }

void fv3_cross(float* dest,
               float* a,
               float* b)                { dest[0] = (a[1] * b[2]) - (a[2] * b[1]);
                                          dest[1] = (a[2] * b[0]) - (a[0] * b[2]);
                                          dest[2] = (a[0] * b[1]) - (a[1] * b[0]);                          }

float  fv3_length(float* vec)           { return (float)sqrt( (vec[0] * vec[0])
                                                            + (vec[1] * vec[1])
                                                            + (vec[2] * vec[2]) );                          }

void fv3_normalize(float* vec)          { float length  = fv3_length(vec);

                                          vec[0]        = vec[0] / length;
                                          vec[1]        = vec[1] / length;
                                          vec[2]        = vec[2] / length;                                  }

//  - --- - --- - --- - --- -

int   maxi(int v1, int v2)              { if(v1 > v2) { return v1; } return v2;                             }
float maxf(float v1, float v2)          { if(v1 > v2) { return v1; } return v2;                             }

int   mini(int v1, int v2)              { if(v1 < v2) { return v1; } return v2;                             }
float minf(float v1, float v2)          { if(v1 < v2) { return v1; } return v2;                             }

int   submini(int v1, int v2)           { return maxi(v1, v2) - mini(v1, v2);                               }
float subminf(float v1, float v2)       { return maxf(v1, v2) - minf(v1, v2);                               }

int   submaxi(int v1, int v2)           { return mini(v1, v2) - maxi(v1, v2);                               }
float submaxf(float v1, float v2)       { return minf(v1, v2) - maxf(v1, v2);                               }

int   flipifi(int v1, int boo)          { return v1 * (1 - (2 * (boo != 0)) );                              }
float flipiff(float v1, int boo)        { return v1 * (1 - (2 * (boo != 0)) );                              }

//  - --- - --- - --- - --- -

int   clampi(int v,
             int start,
             int end)                   { if (v < start) { v = start; }
                                          else if (v > end) { v = end; }
                                          return v;                                                         }

float clampf(float v,
             float start,
             float end)                 { if (v < start) { v = start; }
                                          else if (v > end) { v = end; }
                                          return v;                                                         }

//  - --- - --- - --- - --- -

int   approai(int v1, int v2, int m)    { return ( (v2 - m) < v1) && (v1 < (v2 + m) );                      }

int   approaf(float v1,
              float v2,
              float m)                  { return ( (v2 - m) < v1) && (v1 < (v2 + m) );                      }

int   approadi(int v1, int v2, int m)   { if(approai(v1, v2, m)) { return submini(v1, v2); }
                                          return 999;                                                       }

float approadf(float v1,
               float v2,
               float m)                 { if(approaf(v1, v2, m)) { return subminf(v1, v2); }
                                          return 999;                                                       }

//  - --- - --- - --- - --- -

fvRange* build_fvRange(uint mag,
                      float step)       { 

    fvRange* fvr = (fvRange*) evil_malloc(1, sizeof(fvRange));

    fvr->mag    = mag;
    fvr->step   = step;
    fvr->values = build_fArray(mag);

    for(uint i = 0; i < mag; i++)       { *(fvr->values->buff+i) = (i*step);                                }

    return fvr;                                                                                             }

void del_fvRange(fvRange* fvr)          { WARD_EVIL_MFREE(fvr->values->buff); WARD_EVIL_MFREE(fvr);         }

uint32_t fvRange_take_closest(
                          fvRange* fvr,
                          float v)      {

    float s = fvr->step * 0.5f;
    float dist = 999.0f;
    float newdist;
    uint closest = 0;

    for (uint i = 0; i < fvr->mag; i++) { newdist = approadf(v, *(fvr->values->buff+i), s);
                                          if ( newdist < dist ) { dist = newdist; closest = i; }            }

    return closest;
}

uchar fvRange_take_closest_1b(fvRange* fvr,
                              float v)          {

    if (v >= fvr->values->buff[fvr->mag-1])     { return (uchar)fvr->mag-1;                                             }
    if (v <= fvr->values->buff[0]         )     { return (uchar)0;                                                      }

    if (fvr->mag >= 256)                        { fprintf(stderr,
                                                  "fvRange_take_closest_1b rejects float scales outside the range(0, 256)\n");

                                                  return 0;                                                             }

    float s = fvr->step * 0.5f;
    float dist = 999.0f;
    float newdist;
    uint closest = 0;

    for (uint i = 0; i < fvr->mag; i++)         { newdist = approadf(v, *(fvr->values->buff+i), s);
                                                  if ( newdist < dist ) { dist = newdist; closest = i; }                }

    return closest;                                                                                                     }

