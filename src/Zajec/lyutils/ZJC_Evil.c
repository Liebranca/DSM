#include "ZJC_Evil.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>


//  - --- - --- - --- - --- -

#define errout(message, ...)                    fprintf(stderr, message, __VA_ARGS__)

#define __EVIL_HAS_WON__                      { errout ("\nExecution failed; application terminated.");\
                                                exit(EXIT_FAILURE);                                             }

//  - --- - --- - --- - --- -

void terminator(uint errorcode,
                cchar* info) {

    cchar* mstart;
    if (errorcode < 0x40)                 { mstart = "FATAL EXCEPTION"; gEvilState = FATAL;                 }
    else                                  { mstart = "EXCEPTION";       gEvilState = ERROR;                 }



    errout("%s [ERRCODE %u]: ", mstart, errorcode);



    switch(errorcode)                     { 
        case 0:                             errout("Insuficcient memory.");                      break;
        case 1:                             errout("Access violation.");                         break;
        case 2:                             errout("The end times have come.");                  break;
        case 3:                             errout("You just did something illegal.");           break;

        case 64:                            errout("Couln't open file <%s>",              info); break;
        case 65:                            errout("File couldn't be closed <%s>",        info); break;
        case 66:                            errout("File couldn't be writen <%s>",        info); break;
        case 67:                            errout("Wrong file type for archive <%s>",    info); break;
        case 68:                            errout("Error reading file <%s> overstride.", info); break;


                                                                                                            }

    errout("\nERRLOC TRACKER: %i ENTRIES\n", LOCREG_I);
    evil_printlocreg(1);


    if(gEvilState == FATAL)                 __EVIL_HAS_WON__

    else                                  { /*?*/                                                           }
}

//  - --- - --- - --- - --- -

void* evil_malloc  (uint count,
                    uint size)            { void* buff = malloc( count * size ); WARD_EVIL_MALLOC(buff);
                                            memset(buff, 0, count * size); return buff;                     }

void evil_free    (void* buff)            { free(buff);                                                     }

//  - --- - --- - --- - --- -

void evil_geterrloc(cchar* path,
                    cchar* func,
                    uint line)            {

    snprintf(LOCREG[LOCREG_I], 255, "Location %i <%s> on func %s line %i\n",
                                    LOCREG_I, path, func, line); LOCREG_I++;                                }

void evil_printlocreg(int flush)          {

    for(uint i = 0; i < LOCREG_I; i++)    { fprintf(stderr, LOCREG[i]); if (flush) { LOCREG[i][0] = '\0'; } }
    if(flush)                             { LOCREG_I = 0;                                                   }
                                                                                                            }

void evil_poplocreg()                     {

    if(LOCREG_I)                          { LOCREG[LOCREG_I-1][0] = '\0'; LOCREG_I--;                       }
                                                                                                            }

//  - --- - --- - --- - --- -

