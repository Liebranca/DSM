#include "ZJC_GOPS.h"
#include <stdio.h>

//  - --- - --- - --- - --- -

int   maxi(int v1, int v2)              { if(v1 > v2) { return v1; } return v2;                             }
float maxf(float v1, float v2)          { if(v1 > v2) { return v1; } return v2;                             }

int   mini(int v1, int v2)              { if(v1 < v2) { return v1; } return v2;                             }
float minf(float v1, float v2)          { if(v1 < v2) { return v1; } return v2;                             }

int   submini(int v1, int v2)           { return maxi(v1, v2) - mini(v1, v2);                               }
float subminf(float v1, float v2)       { return maxf(v1, v2) - minf(v1, v2);                               }

int   submaxi(int v1, int v2)           { return mini(v1, v2) - maxi(v1, v2);                               }
float submaxf(float v1, float v2)       { return minf(v1, v2) - maxf(v1, v2);                               }

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

fvRange build_fvRange(uint mag,
                      float step)       { 

    fvRange fvr; fvr.mag = mag; fvr.step = step;
    fvr.values = build_fArray(mag);

    for(uint i = 0; i < mag; i++)       { *(fvr.values.buff+i) = (i*step);                                  }

    return fvr;
}

void del_fvRange(fvRange* fvr)          { WARD_EVIL_MFREE(fvr->values.buff);                                }

uint32_t fvRange_take_closest(
                          fvRange* fvr,
                          float v)      {

    float s = fvr->step * 0.5f;
    float dist = 999.0f;
    float newdist;
    uint closest = 0;

    for (uint i = 0; i < fvr->mag; i++) { newdist = approadf(v, *(fvr->values.buff+i), s);
                                          if ( newdist < dist ) { dist = newdist; closest = i; }            }

    return closest;
}

