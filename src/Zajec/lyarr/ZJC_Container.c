#include "ZJC_Container.h"
#include "../lyutils/ZJC_Evil.h"

#include <stdio.h>

//  - --- - --- - --- - --- -

Container* ZJC_build_cont(uint    tablesize,
                          uint    objsize,
                          uint    maxvalue,
                          cuchar* objname  )    {

    Container* cont  = NULL;

    WARD_EVIL_MALLOC                            (cont,       Container, sizeof(Container), 1                            );
    WARD_EVIL_MALLOC                            (cont->buff, void,      objsize,           tablesize                    );

    cont->hash       = ZJC_build_hash           (tablesize, maxvalue                                                    );
    cont->stack      = ZJC_build_stack          (tablesize                                                              );
                     
    cont->objsize    = objsize;
    cont->objname    = objname;

    for(uint i = tablesize; i > 0; i--)         { ZJC_push_stack(cont->stack, i);                                       }
    return cont;                                                                                                        }

void ZJC_del_cont(Container* cont)              { ZJC_del_stack   (cont->stack); ZJC_del_hash   (cont->hash);
                                                  WARD_EVIL_MFREE (cont->buff ); WARD_EVIL_MFREE(cont      );           }

//  - --- - --- - --- - --- -

uint ZJC_push_cont(Container* cont,
                   uint       id)               { MEAN_NZID_WARNING("ZJC_Container", id);

                                                  uint  data = ZJC_pop_stack(cont->stack); WARD_EVIL_UNSIG(data, 1);
                                                  ZJC_push_hash(cont->hash, id, data); return data;                     }

uint ZJC_pop_cont(Container* cont,
                   uint       id)               {

    if(id)                                      { uint data    =      ZJC_pop_hash  (cont->hash,  id  );
                                                  if  (data) { return ZJC_push_stack(cont->stack, data);                }

                                                  printf     ( "Failed to pop ID %u from %s Container!",
                                                               id, cont->objname                       );
                                                               return 0;                                                }

    printf("%s Container: Non-zero IDs are invalid.", cont->objname);
    return 0;                                                                                                           }

//  - --- - --- - --- - --- -

uint ZJC_findLoc_cont(Container* cont,
                      uint       id,
                      int        shutit)        {

    if(id)
    {
        uint loc = ZJC_find_hash(cont->hash, id);

        if(!loc)                                { if(!shutit) { printf("No %s with ID %u was found.\n",
                                                                cont->objname, id                     ); } return 0;    }

        return loc;
    }

    return 0;                                                                                                           }

//  - --- - --- - --- - --- -
