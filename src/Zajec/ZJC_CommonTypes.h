#ifndef __ZJC_COMMONTYPES_H__
#define __ZJC_COMMONTYPES_H__

#ifdef __cplusplus
extern "C" {
#endif

#define ZJC_DAFSIZE             256

typedef const           char    cchar;
typedef const           short   cshort;
typedef const           int     cint;
typedef const           long    clong;

typedef unsigned        char    uchar;
typedef unsigned        short   ushort;
typedef unsigned        int     uint;
typedef unsigned        long    ulong;

typedef const unsigned  char    cuchar;
typedef const unsigned  short   cushort;
typedef const unsigned  int     cuint;
typedef const unsigned  long    culong;

//  - --- - --- - --- - --- -

typedef struct ZJC_FLOAT_PAIR   { float x; float y; } fpair;
typedef struct ZJC_INT_PAIR     { int x;   int   y; } ipair;

//  - --- - --- - --- - --- -

long hexstr_tolong (char* v);
long chexstr_tolong(cchar* v);

//  - --- - --- - --- - --- -

#ifdef __cplusplus
}
#endif

#endif // __ZJC_COMMONTYPES_H__
