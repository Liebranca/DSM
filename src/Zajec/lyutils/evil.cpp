#include "evil.h"
#include <cstdlib>
#include <iostream>
#include <string>

namespace zjc {

    inline void terminate(int errorcode,
                          concha* loc[])      {
        switch(errorcode)                     { 
                                                case 0: std::cerr << "(!0) Malloc fail at "; break;
                                                case 1: std::cerr << "(!1) Bad access at "; break;
                                                case 2: std::cerr << "(!2) Couln't open file "; break;
                                                case 3: std::cerr << "(!2) Faulty path: "; break;
                                                                                                                }
        for(uint i = 0; i < 5; i++)           { std::cerr << *(loc+i);                                          }
        std::exit(EXIT_FAILURE);
    }

    char* evil_shpath(const char* f)          {
        char s[128]; bool src[] =             { false, false, false                                             };

        uint size = 0;
        while (*f++)                          { size++;                                                         }
        f -= size;

        uint barcount = 1, si = 0;
        uint folds[12]; folds[0] = size - 1;
        uint maxtraverse = 127;
        if (size < 128)                       { maxtraverse = size;                                             }

        for(uint i = size - 1;
            i > size - maxtraverse; i--)      {
            if( (f[i] == '/')
            || (f[i] == '\\') )               { src[0] = f[i+1] == 's';
                                                src[1] = f[i+2] == 'r';
                                                src[2] = f[i+3] == 'c';
                                                if ( src[0] * src[1] * src[2] ) { break; }
                                                folds[barcount] = i; barcount++; }                              }
    
        if (!barcount)                        { std::cerr << f << "can't be resolved!"; std::exit(1);           }

        bool exit = false;
        for (int j = barcount-2; j > -1; j--) {
            if (exit)                         { break;                                                          }
            for(uint i = folds[j+1];
                i < folds[j]; i++)            {
                if(f[i] == '\0')              { exit = true; break; } s[si] = f[i]; si++;                       }
                                                                                                                }
        
        s[si] = '\0';
        return s;

                                                                                                                }

    void* evil_malloc  (uint count,
                        uint size,
                        concha* loc[])        { void* buff = malloc( count * size );
                                                WARD_EVIL_MALLOC(buff, loc); return buff;                       }

    void evil_free    (void* buff)            { free(buff);                                                     }

}
