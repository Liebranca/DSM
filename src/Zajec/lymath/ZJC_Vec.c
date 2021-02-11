#include "ZJC_Vec.h"
#include "ZJC_GOPS.h"

#include <math.h>
#include <stdio.h>

/*  - --- - --- - --- - --- -   START GENERIC STRUCT   - --- - --- - --- - --- -   */

#define GENVEC_PTRARR_SIZE 64
#define GENVEC_DEFN_PROTO(T, ide, name, asarg, EPS, SIZE, ABS, PTR)                                          \
                                                                                                             \
/*  - --- - --- - --- - --- -      Vars   */                                                                 \
                                                                                                             \
static T*   PTR##S[GENVEC_PTRARR_SIZE] = { NULL }; static T*   PTR         = NULL;                           \
static uint PTR##POS = 0;                          static uint PTR##SCP[2] = { 0, 1 };                       \
                                                                                                             \
static uint ZJC_scpcount_##name() { return (PTR##SCP[1] - PTR##SCP[0]) - 1; }                                \
                                                                                                             \
/*  - --- - --- - --- - --- -      Method */                                                                 \
                                GENFUNC_DEFN2(                                                               \
                                                                                                             \
/* Type, module, descriptor & name */                                                                        \
    void, ZJC_, load_, name,                                                                                 \
                                                                                                             \
/* Function body */                                                                                          \
    pos = clampi(pos, 0, GENVEC_PTRARR_SIZE - 1);                                                            \
    num = clampi(num, 1, GENVEC_PTRARR_SIZE - pos);                                                          \
                                                                                                             \
    PTR = (T*) asarg##s[0];                                                                                  \
    for(uint i = pos; i < pos + num; i++) { PTR##S[i] = (T*) asarg##s[i]; },                                 \
                                                                                                             \
/* Arguments */                                                                                              \
    name** asarg##s, uint num, uint pos);                                                                    \
                                                                                                             \
/*  - --- - --- - --- - --- -      Method */                                                                 \
                                GENFUNC_DEFN2(                                                               \
                                                                                                             \
/* Type, module, descriptor & name */                                                                        \
    void, ZJC_, setpos_, name,                                                                               \
                                                                                                             \
/* Function body */                                                                                          \
    pos = clampi(pos, 0, GENVEC_PTRARR_SIZE - 1); PTR##POS = pos; PTR = PTR##S[pos];,                        \
                                                                                                             \
/* Arguments */                                                                                              \
    uint pos);                                                                                               \
                                                                                                             \
/*  - --- - --- - --- - --- -      Method */                                                                 \
                                GENFUNC_DEFN2(                                                               \
                                                                                                             \
/* Type, module, descriptor & name */                                                                        \
    uint, ZJC_, getpos_, name,                                                                               \
                                                                                                             \
/* Function body */                                                                                          \
    return PTR##POS;,                                                                                        \
                                                                                                             \
/* Arguments */                                                                                              \
    );                                                                                                       \
                                                                                                             \
/*  - --- - --- - --- - --- -      Method */                                                                 \
                                GENFUNC_DEFN2(                                                               \
                                                                                                             \
/* Type, module, descriptor & name */                                                                        \
    void, ZJC_, setscp_, name,                                                                               \
                                                                                                             \
/* Function body */                                                                                          \
    start = clampi(start, 0,         GENVEC_PTRARR_SIZE - 1); PTR##SCP[0] = start;                           \
    end   = clampi(end,   start + 1, GENVEC_PTRARR_SIZE    ); PTR##SCP[1] = end;,                            \
                                                                                                             \
/* Arguments */                                                                                              \
    uint start, uint end);                                                                                   \
                                                                                                             \
/*  - --- - --- - --- - --- -      Method */                                                                 \
                                GENFUNC_DEFN2(                                                               \
                                                                                                             \
/* Type, module, descriptor & name */                                                                        \
    void, ZJC_, flush_, name,                                                                                \
                                                                                                             \
/* Function body */                                                                                          \
    for(uint i = 0; i < GENVEC_PTRARR_SIZE; i++) { PTR##S[i] = NULL; };                                      \
    PTR = NULL; PTR##POS = 0; PTR##SCP[0] = 0; PTR##SCP[1] = 1;,                                             \
                                                                                                             \
/* Arguments */                                                                                              \
    );                                                                                                       \
                                                                                                             \
/*  - --- - --- - --- - --- -      Method */                                                                 \
                                GENFUNC_DEFN2(                                                               \
                                                                                                             \
/* Type, module, descriptor & name */                                                                        \
    T*, ZJC_, getp_, name,                                                                                   \
                                                                                                             \
/* Function body */                                                                                          \
    idex = clampi(idex, 0, GENVEC_PTRARR_SIZE - 1); if(mkcur) { ZJC_setpos_##name(idex); }                   \
    return PTR##S[idex];,                                                                                    \
                                                                                                             \
/* Arguments */                                                                                              \
    uint idex, uint mkcur);                                                                                  \
                                                                                                             \
/*  - --- - --- - --- - --- -      Method */                                                                 \
                                GENFUNC_DEFN2(                                                               \
                                                                                                             \
/* Type, module, descriptor & name */                                                                        \
    sint, ZJC_, zcheck_, name,                                                                               \
                                                                                                             \
/* Function body */                                                                                          \
    sint is_zero = 0; for(uint i = 0; i < SIZE; i++) { is_zero += ABS(PTR[i]) < EPS; }                       \
    return is_zero == SIZE;,                                                                                 \
                                                                                                             \
/* Arguments */                                                                                              \
    );                                                                                                       \
                                                                                                             \
/*  - --- - --- - --- - --- -      Method */                                                                 \
                                GENFUNC_DEFN2(                                                               \
                                                                                                             \
/* Type, module, descriptor & name */                                                                        \
    sint, ZJC_, equals_, name,                                                                               \
                                                                                                             \
/* Function body */                                                                                          \
    sint is_equal = 0; uint count = ZJC_scpcount_##name();                                                   \
                                                                                                             \
    for(uint j = 0; j < count; j++)                                                                          \
    {   if(j+1 > GENVEC_PTRARR_SIZE - 1) { break; } T* self = PTR##S[j]; T* other = PTR##S[j + 1];           \
        for(uint i = 0; i < SIZE; i++)                                                                       \
        { is_equal += ABS(self[i] - other[i]) < EPS; }                                                       \
    }                                                                                                        \
    return is_equal == (SIZE * count);,                                                                      \
                                                                                                             \
/* Arguments */                                                                                              \
    );                                                                                                       \
                                                                                                             \
/*  - --- - --- - --- - --- -      Method */                                                                 \
                                GENFUNC_DEFN2(                                                               \
                                                                                                             \
/* Type, module, descriptor & name */                                                                        \
    name, ZJC_, sign_, name,                                                                                 \
                                                                                                             \
/* Function body */                                                                                          \
    name result = {0}; T* resptr = (T*) (&result);                                                           \
                                                                                                             \
    for(uint i = 0; i < SIZE; i++)                                                                           \
    { resptr[i] = flipif##ide(1, PTR[i] < 0); }                                                              \
    return result;,                                                                                          \
                                                                                                             \
/* Arguments */                                                                                              \
    );                                                                                                       \
                                                                                                             \
/*  - --- - --- - --- - --- -      Method */                                                                 \
                                GENFUNC_DEFN2(                                                               \
                                                                                                             \
/* Type, module, descriptor & name */                                                                        \
    void, ZJC_, copy_, name,                                                                                 \
                                                                                                             \
/* Function body */                                                                                          \
    if(!left)                                                                                                \
    {                                                                                                        \
        for(uint j = PTR##SCP[0]; j < PTR##SCP[1] - 1; j++)                                                  \
        {   if(j+1 > GENVEC_PTRARR_SIZE - 1) { break; }                                                      \
            T* self = PTR##S[j]; T* other = PTR##S[j + 1];                                                   \
            for(uint i = 0; i < SIZE; i++) { other[i] = self[i]; }                                           \
        }                                                                                                    \
    }                                                                                                        \
                                                                                                             \
    else                                                                                                     \
    {                                                                                                        \
        for(uint j = PTR##SCP[1] - 1; j > PTR##SCP[0]; j--)                                                  \
        {   if(j-1 < 0) { break; }                                                                           \
            T* self = PTR##S[j]; T* other = PTR##S[j - 1];                                                   \
            for(uint i = 0; i < SIZE; i++) { other[i] = self[i]; }                                           \
        }                                                                                                    \
    },                                                                                                       \
                                                                                                             \
/* Arguments */                                                                                              \
    uint left);                                                                                              \
                                                                                                             \
/*  - --- - --- - --- - --- -      Method */                                                                 \
                                GENFUNC_DEFN2(                                                               \
                                                                                                             \
/* Type, module, descriptor & name */                                                                        \
    void, ZJC_, fill_, name,                                                                                 \
                                                                                                             \
/* Function body */                                                                                          \
    for(uint j = PTR##SCP[0]; j < PTR##SCP[1]; j++)                                                          \
    {   T* self = PTR##S[j];                                                                                 \
        for(uint i = 0; i < SIZE; i++) { self[i] = (T) value; }                                              \
    },                                                                                                       \
                                                                                                             \
/* Arguments */                                                                                              \
    T value);                                                                                                \

#define GENVEC_DEFN(T, ide, EPS, SIZE, ABS) GENVEC_DEFN_PROTO( T, ide, ide##vec##SIZE,                       \
                                                               vec, EPS, SIZE, ABS,                          \
                                                               ide##VEC##SIZE##PTR   )                       \

/*  - --- - --- - --- - --- -   END GENERIC STRUCT     - --- - --- - --- - --- -   */

GENVEC_DEFN(float, f, 1e-4f, 2, fabsf);

