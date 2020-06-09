#include "evil.h"
#include <cstdlib>
#include <iostream>

//  - --- - --- - --- - --- -

namespace zjc {

    inline void terminate(int errorcode,
                          concha* loc,
                          concha* info) {
        
        std::cerr << "FATAL ERRCODE "
                  << errorcode << ": ";

        switch(errorcode)                     { 
                                                case 0: std::cerr << "Malloc fail at "; break;
                                                case 1: std::cerr << "Bad access at "; break;

                                                case 2: std::cerr << "Couln't open file <"
                                                                  << info << "> \nERRLOC "; break;
                                                case 3: std::cerr << "File couldn't be closed <"
                                                                  << info << "> \nERRLOC "; break;
                                                case 4: std::cerr << "File couldn't be writen <"
                                                                  << info << "> \nERRLOC "; break;

                                                case 5: std::cerr << "Faulty path <"
                                                                  << info << "> \nERRLOC "; break;
                                                                                                                }

        std::cerr << loc << "\n" << "\nExecution failed; application terminated." << std::endl;
        std::exit(EXIT_FAILURE);
    }

//  - --- - --- - --- - --- -

    char* evil_shpath(concha* f)          {
        char s[128];
        bool r[] =                            { false, false, false                                             };

        uint size = 0;
        while (*f++)                          { size++;                                                         }
        f -= size;

        uint barcount = 1, si = 0;
        uint folds[12]; folds[0] = size - 1;
        uint maxtraverse = 127;
        if (size < 128)                       { maxtraverse = size;                                             }

//  - --- - --- - --- - --- -

        for(uint i = size - 1;
            i > size - maxtraverse; i--)      {
            if( (f[i] == '/')
            || (f[i] == '\\') )               { r[0] = (f[i+1] == 's');
                                                r[1] = (f[i+2] == 'r');
                                                r[2] = (f[i+3] == 'c');

                                                if (r[0] * r[1] * r[2]) { break; }
                                                folds[barcount] = i; barcount++; }                              }
    
        if (!barcount)                        { std::cerr << f << "can't be resolved!"; std::exit(1);           }

//  - --- - --- - --- - --- -

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

//  - --- - --- - --- - --- -

    void* evil_malloc  (size_t count,
                        size_t size,
                        concha* loc)          { void* buff = malloc( count * size );
                                                WARD_EVIL_MALLOC(buff, loc); return buff;                       }

    void evil_free    (void* buff)            { free(buff);                                                     }

    concha* evil_geterrloc(
                        concha* file,
                        concha* a,
                        concha* func,
                        concha* b,
                        concha* line)         {

        uint sizes[5] =                       { strlen(file), strlen(a), strlen(func),
                                                strlen(b), strlen(line)                                         };
        
        uint totalsize =                      ( sizes[0] + sizes[1] + sizes[2] + sizes[3] + sizes[4] ) + 2;

        concha* arr[5] =                      { file, a, func, b, line                                          };

        char* str =                           ( char*) evil_malloc(totalsize, sizeof(char), ""                  );

        str[0] = '<'; uint ptr_i = 1;
        for(uint i = 0; i < 5; i++)           { for(uint j = 0; j < sizes[i]; j++)
                                              { str[ptr_i] = arr[i][j]; ptr_i++; }                              
                                                if(i == 0) { str[ptr_i] = '>'; ptr_i++; }                       }
        
        str[totalsize] = '\0';
        return str;                                                                                             }

//  - --- - --- - --- - --- -

}
