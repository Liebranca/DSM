#ifndef ZAJEC_EVIL_H
#define ZAJEC_EVIL_H

//  - --- - --- - --- - --- -

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include "../ZJC_CommonTypes.h"

//  - --- - --- - --- - --- -

static uint gEvilState = 0x00;

static char LOCREG[64][256];
static uint LOCREG_I = 0;

void __terminator__(uint errorcode, cchar* info);
void __zjcitoa__(uint x, char* buff, int radix);

static cchar* __evil_shpath__(cchar* path) {
    cchar* file = path;
    cchar* curchar = '\0';
    int src[3] = { 0, 0, 0 };

    while (*path++) {

        curchar = path;
        if ((*curchar == '\\') || (*curchar == '/')) {
            src[0] = *(path+1) == 's';
            src[1] = *(path+2) == 'r';
            src[2] = *(path+3) == 'c';
            
            if(src[0]*src[1]*src[2]) { file = path+5; break; }

        }
    }
    return file;
}

void* __evil_malloc__(uint count, uint size);
void  __evil_free__(void* buff);

void  __evil_geterrloc__(cchar* path, cchar* func, uint line);
void  __evil_printlocreg__(int flush);
void  __evil_poplocreg__();

//  - --- - --- - --- - --- -

static cuint FATAL = 0x01;
static cuint ERROR = 0x02;

//  - --- - --- - --- - --- -

#define __ERRLOC__ __evil_shpath__(__FILE__), __func__, __LINE__
#define GETLOC __evil_geterrloc__(__ERRLOC__)

//  - --- - --- - --- - --- -

#define WARD_EVIL_MALLOC(buff, type, size, count)           { GETLOC; char sizestr[33];                                  \
                                                                                                                         \
    buff     = (type*) __evil_malloc__(count, size);                                                                     \
    if(buff == NULL) { __zjcitoa__(size * count, sizestr, 10); __terminator__(0x00, sizestr); } __evil_poplocreg__();   }

#define WARD_EVIL_MFREE(x)              if(x != NULL)       { __evil_free__((void*) x); x = NULL;                       }

//  - --- - --- - --- - --- -

#define WARD_EVIL_FOPEN(bin)                                { GETLOC;                                                    \
                                                                                                                         \
    if(bin->file == NULL) { __terminator__(0x40, bin->name); return ERROR; } __evil_poplocreg__();                      }

#define WARD_EVIL_FCLOSE(cl, path)                          { GETLOC;                                                    \
                                                                                                                         \
    if(cl) { __terminator__(0x41, path); return ERROR; } __evil_poplocreg__();                                          }

#define WARD_EVIL_FWRITE(writ, writsize, filename)          { GETLOC;                                                    \
                                                                                                                         \
    if(writ != writsize) { __terminator__(0x42, filename); return ERROR; } __evil_poplocreg__();                        }

#define WARD_EVIL_FREAD(read, readsize, filename)           { GETLOC;                                                    \
                                                                                                                         \
    if(read != readsize) { __terminator__(0x43, filename); return ERROR; } __evil_poplocreg__();                        }

//  - --- - --- - --- - --- -

#define WARD_EVIL_WRAP(x, func)         GETLOC;\
                                        x = func;\
                                        if(x == 1)          { return FATAL;                                             }\
                                        else if(x == 2)     { return ERROR;                                             }\
                                        else                { __evil_poplocreg__();                                     }

#define WARD_EVIL_UNSIG(x, tresh)       GETLOC;\
                                        if(x < tresh)       { __terminator__(0x03, "");                                 }\
                                        else                { __evil_poplocreg__(); x -= tresh;                         }

#define MEAN_NZID_WARNING(modname, id) if(!id)              { printf("WARNING (%s): use non-zero IDs, you schmuck!",    \
                                                                     modname                                        );  }

//  - --- - --- - --- - --- -

#ifdef __cplusplus
}
#endif

#endif // ZAJEC_EVIL_H
