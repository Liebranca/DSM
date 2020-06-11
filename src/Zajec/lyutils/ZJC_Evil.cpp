#include "ZJC_Evil.h"
#include <cstdlib>
#include <iostream>

//  - --- - --- - --- - --- -

extern "C" namespace zjc {

    void terminator(uint errorcode,
                    cchar* loc,
                    cchar* info) {

        cchar* mstart;
        if (errorcode < 0x40)                 { mstart = "FATAL EXCEPTION"; gEvilState = FATAL;                 }
        else                                  { mstart = "EXCEPTION";       gEvilState = ERROR;                 }



        std::cerr << mstart << " [ERRCODE " << errorcode << "]: ";



        switch(errorcode)                     { case 0: std::cerr << "Insuficcient memory."; break;
                                                case 1: std::cerr << "Access violation."; break;
                                                case 2: std::cerr << "Pray to god this one never happens."; break;


                                                case 64: std::cerr << "Couln't open file <"
                                                                   << info << ">"; break;

                                                case 65: std::cerr << "File couldn't be closed <"
                                                                   << info << ">"; break;

                                                case 66: std::cerr << "File couldn't be writen <"
                                                                   << info << ">"; break;

                                                case 67: std::cerr << "Wrong file type for archive <"
                                                                   << info << ">"; break;

                                                case 68: std::cerr << "Error reading file <" << info << ">,"
                                                                   << "stride over max value."; break;



                                                case 100: std::cerr << "Faulty path <" << info << ">"; break;

                                                                                                                }

        std::cerr << "\nERRLOC " << loc << "\n";


        if(gEvilState == FATAL)                 __EVIL_HAS_WON__

        else                                  { /*?*/                                                           }
    }

//  - --- - --- - --- - --- -

    cchar* evil_shpath(cchar* f)              {
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
                        cchar* loc)          { void* buff = malloc( count * size );
                                                WARD_EVIL_MALLOC(buff, loc); return buff;                       }

    void evil_free    (void* buff)            { free(buff);                                                     }

//  - --- - --- - --- - --- -

    cchar* evil_geterrloc(
                        cchar* file,
                        cchar* a,
                        cchar* func,
                        cchar* b,
                        cchar* line)         {

        uint sizes[5] =                       { strlen(file), strlen(a), strlen(func),
                                                strlen(b), strlen(line)                                         };
        
        uint totalsize =                      ( sizes[0] + sizes[1] + sizes[2] + sizes[3] + sizes[4] ) + 2;

        cchar* arr[5] =                      { file, a, func, b, line                                          };

        char* str =                           ( char*) evil_malloc(totalsize, sizeof(char), ""                  );

        str[0] = '<'; uint ptr_i = 1;
        for(uint i = 0; i < 5; i++)           { for(uint j = 0; j < sizes[i]; j++)
                                              { str[ptr_i] = arr[i][j]; ptr_i++; }                              
                                                if(i == 0) { str[ptr_i] = '>'; ptr_i++; }                       }
        
        str[totalsize] = '\0';
        return str;                                                                                             }

//  - --- - --- - --- - --- -

}
