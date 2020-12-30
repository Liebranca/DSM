#include "ZJC_Evil.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>


//  - --- - --- - --- - --- -

#define errout(message, ...)                    fprintf(stderr, message, __VA_ARGS__)

#define __EVIL_HAS_WON__                      { errout ("\nExecution failed; application terminated.");\
                                                exit(EXIT_FAILURE);                                             }

//  - --- - --- - --- - --- -

void __terminator__(uint errorcode,
                    cchar* info)          {

    cchar* mstart;
    if (errorcode < 0x40)                 { mstart = "FATAL EXCEPTION"; gEvilState = FATAL;                 }
    else                                  { mstart = "EXCEPTION";       gEvilState = ERROR;                 }



    errout("%s [ERRCODE %u]: ", mstart, errorcode);



    switch(errorcode)                     { 
        case  0:                            errout("Insuficcient memory (%s requested).", info); break;
        case  1:                            errout("Access violation.");                         break;
        case  2:                            errout("The end times have come.");                  break;
        case  3:                            errout("You just did something illegal.");           break;

        case 64:                            errout("Couln't open file <%s>",              info); break;
        case 65:                            errout("File couldn't be closed <%s>",        info); break;
        case 66:                            errout("Error writting to file <%s>",         info); break;
        case 67:                            errout("Error reading from file <%s>.",       info); break;

        case 68:                            errout("Inappropriate file signature <%s>",   info); break;


                                                                                                            }

    errout("\nERRLOC TRACKER: %i ENTRIES\n", LOCREG_I);
    __evil_printlocreg__(1);


    if(gEvilState == FATAL)                 __EVIL_HAS_WON__

    else                                  { /*?*/                                                           }
}

//  - --- - --- - --- - --- -

void __zjcitoa__(uint x,
                 char* buff,
                 int radix )              { itoa(x, buff, radix);                                           }

void* __evil_malloc__(uint count,
                      uint size)          { void* buff = malloc( count * size );
                                            memset(buff, 0, count * size); return buff;                     }

void __evil_free__(void* buff)            { free(buff);                                                     }

//  - --- - --- - --- - --- -

void __evil_geterrloc__(cchar* path,
                        cchar* func,
                        uint line  )      {

    snprintf(LOCREG[LOCREG_I], 255, "Location %i <%s> on func %s line %i\n",
                                    LOCREG_I, path, func, line); LOCREG_I++;                                }

void __evil_printlocreg__(int flush)      {

    for(uint i = 0; i < LOCREG_I; i++)    { fprintf(stderr, LOCREG[i]); if (flush) { LOCREG[i][0] = '\0'; } }
    if(flush)                             { LOCREG_I = 0;                                                   }
                                                                                                            }

void __evil_poplocreg__()                 {

    if(LOCREG_I)                          { LOCREG[LOCREG_I-1][0] = '\0'; LOCREG_I--;                       }
                                                                                                            }

//  - --- - --- - --- - --- -

