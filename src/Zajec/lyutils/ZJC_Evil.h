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

void    terminator(uint errorcode, cchar* info);

static cchar* evil_shpath(cchar* path) {
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

void*   evil_malloc(uint count, uint size);
void    evil_free(void* buff);

void  evil_geterrloc(cchar* path, cchar* func, uint line);
void  evil_printlocreg(int flush);
void  evil_poplocreg();

//  - --- - --- - --- - --- -

static cuint FATAL = 0x01;
static cuint ERROR = 0x02;

//  - --- - --- - --- - --- -

#define __ERRLOC__ evil_shpath(__FILE__), __func__, __LINE__
#define GETLOC evil_geterrloc(__ERRLOC__)

//  - --- - --- - --- - --- -

#define WARD_EVIL_MALLOC(x)             if(x == NULL)       { terminator(0x00, "");                                     }
#define WARD_EVIL_MFREE(x)              if(x != NULL)       { evil_free((void*) x); x = NULL;                           }
#define WARD_EVIL_FOPEN(x, y)           if(x == NULL)       { terminator(0x40, y);  return ERROR;                       }
#define WARD_EVIL_FCLOSE(x, y)          if(x)               { terminator(0x41, y);  return ERROR;                       }
#define WARD_EVIL_FWRITE(x, y, z)       if(x != z)          { terminator(0x42, y);  return ERROR;                       }

#define WARD_EVIL_WRAP(x, func)         GETLOC;\
                                        x = func;\
                                        if(x == 1)          { return FATAL;                                             }\
                                        else if(x == 2)     { return ERROR;                                             }\
                                        else                { evil_poplocreg();                                         }

#define WARD_EVIL_UNSIG(x, tresh)       GETLOC;\
                                        if(x < tresh)       { terminator(0x03, "");                                     }\
                                        else                { evil_poplocreg(); x -= tresh;                             }

#define MEAN_NZID_WARNING(modname, id) if(!id)              { printf("WARNING (%s): use non-zero IDs, you schmuck!",    \
                                                                     modname                                        );  }

#ifdef __cplusplus
}
#endif

#endif // ZAJEC_EVIL_H
