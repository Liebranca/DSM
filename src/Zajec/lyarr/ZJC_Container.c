#include "ZJC_Container.h"
#include "../lyutils/ZJC_Evil.h"

#include <stdio.h>

//  - --- - --- - --- - --- -

Container* ZJC_buildcont(uint    stacksize,
                         uint    tablesize,
                         uint    objsize,
                         cuchar* objname    )   {

    Container* cont = (Container*) evil_malloc  (1,         sizeof(Container)                                           );
    cont->buff      = (uchar*    ) evil_malloc  (stacksize, objsize                                                     );

    cont->ID_table  = evil_malloc               (tablesize, sizeof(uint     )                                           );
    cont->stack     = ZJC_buildstack            (stacksize                                                              );
    cont->objsize   = objsize;
    cont->objname   = objname;

    for(uint i = stacksize; i > 0; i--)         { ZJC_pushstack(cont->stack, i);                                        }

    return cont;                                                                                                        }

void ZJC_delcont(Container* cont)               { ZJC_delstack    (cont->stack); WARD_EVIL_MFREE(cont->ID_table);
                                                  WARD_EVIL_MFREE (cont->buff ); WARD_EVIL_MFREE(cont          );       }

//  - --- - --- - --- - --- -

uint ZJC_pushcont(Container* cont,
                  uint       id)                { uint  loc = ZJC_popstack(cont->stack); WARD_EVIL_UNSIG(loc, 1);
                                                  cont->ID_table[id] = loc; return loc;                                 }

uint ZJC_cont_findLoc(Container* cont,
                      uint       id    )        {

    uint loc = cont->ID_table[id];
    if(loc == 0)                                { printf("No %s with ID %u was found.\n", cont->objname, id); return 0; }

    return loc;                                                                                                         }

//  - --- - --- - --- - --- -
