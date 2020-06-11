#ifndef ZAJEC_EVIL_H
#define ZAJEC_EVIL_H

//  - --- - --- - --- - --- -

#include "../ZJC_CommonTypes.h"

//  - --- - --- - --- - --- -

extern "C" namespace zjc {

    static uint gEvilState = 0x00;

    void    terminator(uint errorcode, cchar* loc, cchar* info);

    cchar*  evil_shpath(cchar* f);

    void*   evil_malloc(size_t count, size_t size, cchar* loc);
    void    evil_free(void* buff);

    cchar*  evil_geterrloc(cchar* file, cchar* a, cchar* func, cchar* b, cchar* line);

}

//  - --- - --- - --- - --- -

constexpr uint FATAL = 0x01;
constexpr uint ERROR = 0x02;


#define WARD_EVIL_MALLOC(x, y)          if(x == nullptr)    { zjc::terminator(0x00, y, "");                         }
#define WARD_EVIL_MFREE(x)              if(x != NULL)       { zjc::evil_free((void*)x);                             }
#define WARD_EVIL_FOPEN(x, y, z)        if(x == NULL)       { zjc::terminator(0x40, y, z);  return ERROR;           }
#define WARD_EVIL_FCLOSE(x, y, z)       if(x != NULL)       { zjc::terminator(0x41, y, z);  return ERROR;           }
#define WARD_EVIL_FWRITE(x, y, z, w)    if(x != w)          { zjc::terminator(0x42, y, z);  return ERROR;           }

#define WARD_EVIL_WRAP(x)               if(x == 1)          { return FATAL;                                         }\
                                        if(x == 2)          { return ERROR;                                         }

//  - --- - --- - --- - --- -

#define __EVIL_HAS_WON__                                    { std::cerr << "\nExecution failed;"\
                                                                        << "application terminated."\
                                                                        << std::endl; std::exit(EXIT_FAILURE);      }

//  - --- - --- - --- - --- -

#define __HASHIT__(x) #x
#define HASHIT(x) __HASHIT__(x)

#define __LINESTRING__ HASHIT(__LINE__)

#define __ERRLOC__ zjc::evil_shpath(__FILE__), " on func ", __func__, " line ", __LINESTRING__
#define GETLOC zjc::evil_geterrloc(__ERRLOC__)

//  - --- - --- - --- - --- -

#endif // ZAJEC_EVIL_H
