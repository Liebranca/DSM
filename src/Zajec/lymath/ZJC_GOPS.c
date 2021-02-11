#include "ZJC_GOPS.h"

#include "../lyutils/ZJC_Evil.h"

#include <stdio.h>
#include <math.h>

//  - --- - --- - --- - --- -

float iv2_distance(int a[2], int b[2])  { return (float)sqrt( pow(a[0] - b[0], 2) + pow(a[1] - b[1], 2) );  }

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

#define GOPSMAX_DEFN(T, ide) GENFUNC_DEFN(                                                                   \
                                                                                                             \
    /* Type, name & identifier */                                                                            \
    T, max, ide,                                                                                             \
                                                                                                             \
    /* Function body */                                                                                      \
    if(v1 > v2) { return v1; } return v2;,                                                                   \
                                                                                                             \
    /* Arguments */                                                                                          \
    T v1, T v2)

// (int) Get greater of (v1, v2)
GOPSMAX_DEFN(sint,  i);

// (float) Get greater of (v1, v2)
GOPSMAX_DEFN(float, f);

//  - --- - --- - --- - --- -

#define GOPSMIN_DEFN(T, ide) GENFUNC_DEFN(                                                                   \
                                                                                                             \
    /* Type, name & identifier */                                                                            \
    T, min, ide,                                                                                             \
                                                                                                             \
    /* Function body */                                                                                      \
    if(v1 < v2) { return v1; } return v2;,                                                                   \
                                                                                                             \
    /* Arguments */                                                                                          \
    T v1, T v2)

// (int) Get lesser of (v1, v2)
GOPSMIN_DEFN(sint,  i);

// (float) Get lesser of (v1, v2)
GOPSMIN_DEFN(float, f);

//  - --- - --- - --- - --- -

#define GOPSSUBMIN_DEFN(T, ide) GENFUNC_DEFN(                                                                \
                                                                                                             \
    /* Type, name & identifier */                                                                            \
    T, submin, ide,                                                                                          \
                                                                                                             \
    /* Function body */                                                                                      \
    return max##ide(v1, v2) - min##ide(v1, v2);,                                                             \
                                                                                                             \
    /* Arguments */                                                                                          \
    T v1, T v2)

// (int) Returns max(v1, v2) minus min(v1, v2)
GOPSSUBMIN_DEFN(sint,  i);

// (float) Returns max(v1, v2) minus min(v1, v2)
GOPSSUBMIN_DEFN(float, f);

//  - --- - --- - --- - --- -

#define GOPSSUBMAX_DEFN(T, ide) GENFUNC_DEFN(                                                                \
                                                                                                             \
    /* Type, name & identifier */                                                                            \
    T, submax, ide,                                                                                          \
                                                                                                             \
    /* Function body */                                                                                      \
    return min##ide(v1, v2) - max##ide(v1, v2);,                                                             \
                                                                                                             \
    /* Arguments */                                                                                          \
    T v1, T v2)

// (int) Returns min(v1, v2) minus max(v1, v2)
GOPSSUBMAX_DEFN(sint,  i);

// (float) Returns min(v1, v2) minus max(v1, v2)
GOPSSUBMAX_DEFN(float, f);

//  - --- - --- - --- - --- -

#define GOPSFLIPIF_DEFN(T, ide) GENFUNC_DEFN(                                                                \
                                                                                                             \
    /* Type, name & identifier */                                                                            \
    T, flipif, ide,                                                                                          \
                                                                                                             \
    /* Function body */                                                                                      \
    return v1 * (1 - (2 * (boo != 0)) );,                                                                    \
                                                                                                             \
    /* Arguments */                                                                                          \
    T v1, sint boo)

// (int) Inverts sign of v1 if boo
GOPSFLIPIF_DEFN(sint,  i);

// (float) Inverts sign of v1 if boo
GOPSFLIPIF_DEFN(float, f);

//  - --- - --- - --- - --- -

float fround(float x)                   { int y = (sint)(x * 100 + .5f); return (float)(y * 0.01f);         }

//  - --- - --- - --- - --- -

#define GOPSCLAMP_DEFN(T, ide) GENFUNC_DEFN(                                                                 \
                                                                                                             \
    /* Type, name & identifier */                                                                            \
    T, clamp, ide,                                                                                           \
                                                                                                             \
    /* Function body */                                                                                      \
    if      (v < start) { v = start; }                                                                       \
    else if (v > end  ) { v = end;   }                                                                       \
    return v;,                                                                                               \
                                                                                                             \
    /* Arguments */                                                                                          \
    T v, T start, T end)

// (int) Clamp v to range(start, end)
GOPSCLAMP_DEFN(sint,  i);

// (float) Clamp v to range(start, end)
GOPSCLAMP_DEFN(float, f);

//  - --- - --- - --- - --- -
/* add steps to v while wrapping around range

   so if 1 in (0, 9) - 17 gets you 2; 1 in (0, 9) + 18 gets you 1
   very specific func for a looping colle; i.e. add to ptr until end of array, then loop back              */

#define GOPSWRAP_DEFN(T, ide) GENFUNC_DEFN(                                                                  \
                                                                                                             \
    /* Type, name & identifier */                                                                            \
    T, wrap, ide,                                                                                            \
                                                                                                             \
    /* Function body */                                                                                      \
    T diff;                                                                                                  \
                                                                                                             \
    while(steps)                                                                                             \
    {                                                                                                        \
                                                                                                             \
/*  - --- - --- - --- - --- -                                                                              */\
/* get (-) start to v | (+) v to end distance                                                              */\
                                                                                                             \
        if     (steps < 0)  { diff = 1 + submin##ide(start, v);                                             }\
        else                { diff = 1 + submin##ide(end,   v);                                             }\
                                                                                                             \
        v += steps;                                                                                          \
                                                                                                             \
/*  - --- - --- - --- - --- -                                                                              */\
/* if v not in (start, end) then substract distance from steps and run again, else break                   */\
                                                                                                             \
        if     (v < start)  { steps += diff; v = end;                                                       }\
        else if(v > end  )  { steps -= diff; v = start;                                                     }\
        else                { break;                                                                        }\
                                                                                                             \
    }                                                                                                        \
                                                                                                             \
    return v;,                                                                                               \
                                                                                                             \
    /* Arguments */                                                                                          \
    T v, T steps, T start, T end)

// (int) Add (-/+) steps to v and loop around range(start, end)
GOPSWRAP_DEFN(sint,  i);

// (float) Add (-/+) steps to v and loop around range(start, end)
GOPSWRAP_DEFN(float, f);

//  - --- - --- - --- - --- -

#define GOPSAPPROA_DEFN(T, ide) GENFUNC_DEFN(                                                                \
                                                                                                             \
    /* Type, name & identifier */                                                                            \
    sint, approa, ide,                                                                                       \
                                                                                                             \
    /* Function body */                                                                                      \
    return ( (v2 - m) < v1) && (v1 < (v2 + m) );,                                                            \
                                                                                                             \
    /* Arguments */                                                                                          \
    T v1, T v2, T m)

// (int) Get v1 is at least (-/+) m distance of v2
GOPSAPPROA_DEFN(sint,  i);

// (float) Get v1 is at least (-/+) m distance of v2
GOPSAPPROA_DEFN(float, f);

//  - --- - --- - --- - --- -

#define GOPSAPPROAD_DEFN(T, ide) GENFUNC_DEFN(                                                               \
                                                                                                             \
    /* Type, name & identifier */                                                                            \
    T, approad, ide,                                                                                         \
                                                                                                             \
    /* Function body */                                                                                      \
    if(approa##ide(v1, v2, m)) { return submin##ide(v1, v2); } return (T)999;,                               \
                                                                                                             \
    /* Arguments */                                                                                          \
    T v1, T v2, T m)

// (int) Get distance if v1 is at least (-/+) m distance of v2
GOPSAPPROAD_DEFN(sint,  i);

// (float) Get distance if v1 is at least (-/+) m distance of v2
GOPSAPPROAD_DEFN(float, f);

/*  - --- - --- - --- - --- -   START GENERIC STRUCT   - --- - --- - --- - --- -   */

#define VRANGE_DEFN_PROTO(T, ide, name, asarg)                                                               \
                                                                                                             \
/*  - --- - --- - --- - --- -      Method */                                                                 \
                                GENFUNC_DEFN2(                                                               \
                                                                                                             \
/* Type, module, descriptor & name */                                                                        \
    name*, ZJC_, build_, name,                                                                               \
                                                                                                             \
/* Function body */                                                                                          \
    name* asarg;                                                                                             \
                                                                                                             \
    WARD_EVIL_MALLOC(asarg->buff, T, sizeof(T), mag);                                                        \
                                                                                                             \
    asarg->mag  = mag;                                                                                       \
                                                                                                             \
    if(step < 0)                        { step = -step;                                                      \
                                                                                                             \
                                          for(int i = (int)(-(int)(mag) / 2); i < (int)(mag / 2); i++)       \
                                        { asarg->buff[i + (mag / 2)] = (i*step); }                          }\
                                                                                                             \
    else                                { for(uint i = 0; i < mag; i++)                                      \
                                        { asarg->buff[i] = (i*step); }                                      }\
    asarg->step = step;                                                                                      \
    return asarg;,                                                                                           \
                                                                                                             \
/* Arguments */                                                                                              \
    uint mag, T step);                                                                                       \
                                                                                                             \
/*  - --- - --- - --- - --- -      Method */                                                                 \
                                GENFUNC_DEFN2(                                                               \
                                                                                                             \
/* Type, module, descriptor & name */                                                                        \
    void, ZJC_, del_, name,                                                                                  \
                                                                                                             \
/* Function body */                                                                                          \
    WARD_EVIL_MFREE(asarg->buff);,                                                                           \
                                                                                                             \
/* Arguments */                                                                                              \
    name* asarg);                                                                                            \
                                                                                                             \
/*  - --- - --- - --- - --- -      Method */                                                                 \
                                GENFUNC_DEFN2(                                                               \
                                                                                                             \
/* Type, module, descriptor & name */                                                                        \
    uint, ZJC_, find_, name,                                                                                 \
                                                                                                             \
/* Function body */                                                                                          \
    if (v >= asarg->buff[asarg->mag-1]) { return asarg->mag-1;                                              }\
    if (v <= asarg->buff[0]           ) { return 0;                                                         }\
                                                                                                             \
    T s    = (T) (asarg->step * 0.5f);                                                                       \
    T dist = 999;                                                                                            \
                                                                                                             \
    T newdist;                                                                                               \
                                                                                                             \
    uint  closest = 0;                                                                                       \
                                                                                                             \
    for (uint i = 0; i < asarg->mag; i++) { newdist = approadf(v, asarg->buff[i], s);                        \
                                       if ( newdist < dist ) { dist = newdist; closest = i; }               }\
                                                                                                             \
    return closest;,                                                                                         \
                                                                                                             \
/* Arguments */                                                                                              \
    name* asarg, T v);                                                                                       \

#define VRANGE_DEFN(T, ide) VRANGE_DEFN_PROTO(T, ide, ide##vRange, ide##vr)

/*  - --- - --- - --- - --- -   END GENERIC STRUCT     - --- - --- - --- - --- -   */

// (float) Value range
VRANGE_DEFN(float, f);
