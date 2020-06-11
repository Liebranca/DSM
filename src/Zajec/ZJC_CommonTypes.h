#ifndef __ZJC_COMMONTYPES_H__
#define __ZJC_COMMONTYPES_H__

extern "C" {

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

    namespace zjc {
        long hexstr_tolong(char* v);
    }

}

#define HEXLIT(value, type) (type) zjc::hexstr_tolong(value);

#endif // __ZJC_COMMONTYPES_H__
