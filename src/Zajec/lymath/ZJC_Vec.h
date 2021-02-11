#ifndef __ZJC_VEC2_H__
#define __ZJC_VEC2_H__

#include "../ZJC_CommonTypes.h"
#include "../lyutils/ZJC_Generic.h"

//  ---     ---     ---     ---     ---

#ifdef __cplusplus
extern "C" {
#endif

//  ---     ---     ---     ---     ---

#define VECSIZE2_ARGS(T)   T x, T y
#define VECSIZE2_FIELDS(T) T x; T y;

#define VECSIZE3_ARGS(T)   T x, T y, T z
#define VECSIZE3_FIELDS(T) T x; T y; T z;

#define VECSIZE4_ARGS(T)   T x, T y, T z, T w
#define VECSIZE4_FIELDS(T) T x; T y; T z; T w;

#define VECSIZE_ARGS(T, SIZE)   VECSIZE##SIZE##_ARGS(T)
#define VECSIZE_FIELDS(T, SIZE) VECSIZE##SIZE##_FIELDS(T)

//  ---     ---     ---     ---     ---

#define GENVEC_STRUCT_PROTO(T, ide, strtd, SIZE, name, asarg) GENSTRUCT_DECL(                                \
                                                                                                             \
/* Type, struct typedef, identifier */                                                                       \
    T, strtd, ide,                                                                                           \
                                                                                                             \
/* Struct fields */                                                                                          \
    VECSIZE_FIELDS(T, SIZE),                                                                                 \
                                                                                                             \
/* Method declarations */                                                                                    \
                                                                                                             \
    /* House-keeping funcs                                                     */                            \
    GENFUNC_DECL2(void, ZJC_, load_,   name, name** asarg##s, uint num, uint pos);                           \
    GENFUNC_DECL2(void, ZJC_, setpos_, name, uint pos                           );                           \
    GENFUNC_DECL2(uint, ZJC_, getpos_, name,                                    );                           \
    GENFUNC_DECL2(void, ZJC_, setscp_, name, uint start, uint end               );                           \
    GENFUNC_DECL2(void, ZJC_, flush_,  name,                                    );                           \
    GENFUNC_DECL2(T*,   ZJC_, getp_,   name, uint idex, uint mkcur              );                           \
                                                                                                             \
    /* Common checks                                                           */                            \
    GENFUNC_DECL2(sint, ZJC_, zcheck_, name,                                    );                           \
    GENFUNC_DECL2(sint, ZJC_, equals_, name,                                    );                           \
    GENFUNC_DECL2(name, ZJC_, sign_,   name,                                    );                           \
                                                                                                             \
    /* Data copying                                                            */                            \
    GENFUNC_DECL2(void, ZJC_, copy_,   name, uint left                          );                           \
    GENFUNC_DECL2(void, ZJC_, fill_,   name, T value                            );                           \
                                                                                                            )

GENVEC_STRUCT_PROTO(float, f, vec2, 2, fvec2, vec);

//  ---     ---     ---     ---     ---

#ifdef __cplusplus
}
#endif

#endif // __ZJC_VEC2_H__
