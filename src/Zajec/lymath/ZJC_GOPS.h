/*
    ┌───────────────────────────────────────────────────────────────────────────────────┐
    │  oh noe! Zajec is rocket science! T_T                                             │
    │  but not really. this is just templates in C macro-mindfuck style                 │
    │                                                                                   │
    │  _DECL macros expand to the function declaration                                  │
    │  syntax is (restype, funcname, identifier, *args)                                 │
    │                                                                                   │
    │  identifier is just some token to differentiate functions (conventions below)     │
    │  for args, use the form (argtype argname, argtype argname, ...)                   │
    │  i hardly use more than four but you should be able to have as many as you want   │
    │                                                                                   │
    │  also, T (restype) can be used inside args. so (T x) is (restype) x;              │
    │                                                                                   │
    │  confussed yet? hehe                                                              │
    │                                                                                   │
    │  _DEFN macros expand to the function definition                                   │
    │  syntax is (restype, funcname, identifier, body, *args)                           │
    │                                                                                   │
    │  this is when it gets to new high bishzeen levels of weird as youre basically     │
    │  writting the body of a function inside the arguments of a macro. just sickening! │
    │  aside from that, its the same as its _DECL counterpart                           │
    │                                                                                   │
    │  one aside, dont you fucking dare put _DEFNs on a header or ill haunt your dreams │
    │                                                                                   │
    │  once youve written your _DECL and _DEFN macros for a function, you then have to  │
    │  create a copy of that function for a certain type.                               │
    │                                                                                   │
    │  i like macro_name(type, identifier), you can be more adventurous if you want.    │
    │                                                                                   │
    ╞═══════════════════════════════════════════════════════════════════════════════════╡
    │                                                                                   │
    │ IDENTIFIER CONVENTIONS                                                            │
    │                                                                                   │
    ╞═══════════════════════════════════════════════════════════════════════════════════╡
    │                                                                                   │
    │ this are the identifiers that i, grand fucking daddy lyeb, like to use better     │
    │                                                                                   │
    │ if youre from the future expanding Zajec for whatever reason, stick to them       │
    │ else youll just make this harder to follow, and who wants that?                   │
    │                                                                                   │
    ╞══════════╤════════════════════════════════════════════════════════════╤═══════════╡
    │  token   │                       type description                     │  typedef  │
    ├──────────┼────────────────────────────────────────────────────────────┼───────────┤
    │    c     │         signed      8-bits   char                          │  schar    │
    │    s     │         signed     16-bits   short                         │  sshort   │
    │    i     │         signed     32-bits   int or long                   │  sint     │
    │    f     │                    32-bits   float                         │           │
    │    l     │         signed     64-bits   long or long long             │  slong    │
    │    d     │                    64-bits   double                        │           │
    ├──────────┼────────────────────────────────────────────────────────────┼───────────┤
    │    cc    │ const   signed      8-bits   char                          │  cchar    │
    │    cs    │ const   signed     16-bits   short                         │  cshort   │
    │    ci    │ const   signed     32-bits   int or long                   │  cint     │
    │    cf    │ const              32-bits   float                         │           │
    │    cl    │ const   signed     64-bits   long or long long             │  clong    │
    │    cd    │ const              64-bits   double                        │           │
    ├──────────┼────────────────────────────────────────────────────────────┼───────────┤
    │    uc    │         unsigned    8-bits   char                          │  uchar    │
    │    us    │         unsigned   16-bits   short                         │  ushort   │
    │    ui    │         unsigned   32-bits   int or long                   │  uint     │
    │    ul    │         unsigned   64-bits   long or long long             │  ulong    │
    ├──────────┼────────────────────────────────────────────────────────────┼───────────┤
    │    cuc   │ const   unsigned    8-bits   char                          │  cuchar   │
    │    cus   │ const   unsigned   16-bits   short                         │  cushort  │
    │    cui   │ const   unsigned   32-bits   int or long                   │  cuint    │
    │    cul   │ const   unsigned   64-bits   long or long long             │  culong   │
    │          │                                                            │           │
    └──────────┴────────────────────────────────────────────────────────────┴───────────┘
*/

//  - --- - --- - --- - --- -

#ifndef __ZJC_GOPS_H__
#define __ZJC_GOPS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../ZJC_CommonTypes.h"
#include "../lyutils/ZJC_Generic.h"

//  - --- - --- - --- - ---

float iv2_distance(int a[2], int b[2]);

//  - --- - --- - --- - --- -

void  fv3_sub(float* a, float* b);
void  fv3_add(float* a, float* b);
void  fv3_cross(float* dest, float* a, float* b);
float fv3_length(float* vec);
void  fv3_normalize(float* vec);

//  - --- - --- - --- - --- -
// Get greater of (v1, v2)
#define GOPSMAX_DECL(T, ide) GENFUNC_DECL(T, max, ide, T v1, T v2)

GOPSMAX_DECL(sint,  i);
GOPSMAX_DECL(float, f);

//  - --- - --- - --- - --- -
// Get lesser of (v1, v2)
#define GOPSMIN_DECL(T, ide) GENFUNC_DECL(T, min, ide, T v1, T v2)

GOPSMIN_DECL(sint,  i);
GOPSMIN_DECL(float, f);

//  - --- - --- - --- - --- -
// Returns max(v1, v2) minus min(v1, v2)
#define GOPSSUBMIN_DECL(T, ide) GENFUNC_DECL(T, submin, ide, T v1, T v2)

GOPSSUBMIN_DECL(sint,  i);
GOPSSUBMIN_DECL(float, f);

//  - --- - --- - --- - --- -
// Returns min(v1, v2) minus max(v1, v2)
#define GOPSSUBMAX_DECL(T, ide) GENFUNC_DECL(T, submax, ide, T v1, T v2)

GOPSSUBMAX_DECL(sint,  i);
GOPSSUBMAX_DECL(float, f);

//  - --- - --- - --- - --- -
// Inverts sign of v1 if boo
#define GOPSFLIPIF_DECL(T, ide) GENFUNC_DECL(T, flipif, ide, T v1, sint boo)

GOPSFLIPIF_DECL(sint,  i);
GOPSFLIPIF_DECL(float, f);

//  - --- - --- - --- - --- -
// Round x to nearest integer (APPARENTLY)
float fround(float x);

//  - --- - --- - --- - --- -
// Clamp v to range(start, end)
#define GOPSCLAMP_DECL(T, ide) GENFUNC_DECL(T, clamp, ide, T v, T start, T end)

GOPSCLAMP_DECL(sint,  i);
GOPSCLAMP_DECL(float, f);

//  - --- - --- - --- - --- -
// Add (-/+) steps to v and loop around range(start, end)
#define GOPSWRAP_DECL(T, ide) GENFUNC_DECL(T, wrap, ide, T v, T steps, T start, T end)

GOPSWRAP_DECL(sint,  i);
GOPSWRAP_DECL(float, f);

//  - --- - --- - --- - --- -
// Get v1 is at least (-/+) m distance of v2
#define GOPSAPPROA_DECL(T, ide) GENFUNC_DECL(sint, approa, ide, T v1, T v2, T m)

GOPSAPPROA_DECL(sint,  i);
GOPSAPPROA_DECL(float, f);

//  - --- - --- - --- - --- -
// Get distance if v1 is at least (-/+) m distance of v2
#define GOPSAPPROAD_DECL(T, ide) GENFUNC_DECL(T, approad, ide, T v1, T v2, T m)

GOPSAPPROAD_DECL(sint,  i);
GOPSAPPROAD_DECL(float, f);

//  - --- - --- - --- - --- -
// Value range
#define VRANGE_STRUCT_PROTO(T, ide, strtd, name, asarg) GENSTRUCT_DECL(                                      \
                                                                                                             \
/* Type, struct typedef, identifier */                                                                       \
    T, strtd, ide,                                                                                           \
                                                                                                             \
/* Struct fields */                                                                                          \
    uint mag; T step; T* buff;,                                                                              \
                                                                                                             \
/* Method declarations */                                                                                    \
    GENFUNC_DECL2(name*, ZJC_, build_, name, uint mag, T step);                                              \
    GENFUNC_DECL2(void,  ZJC_, del_,   name, name* asarg     );                                              \
    GENFUNC_DECL2(uint,  ZJC_, find_,  name, name* asarg, T v);                                              \
)

#define VRANGE_STRUCT_DECL(T, ide) VRANGE_STRUCT_PROTO(T, ide, vRange, ide##vRange, ide##vr)

VRANGE_STRUCT_DECL(float, f);

//  - --- - --- - --- - --- -

#ifdef __cplusplus
}
#endif

#endif // __ZJC_GOPS_H__
