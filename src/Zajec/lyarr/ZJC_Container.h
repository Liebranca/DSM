#ifndef __ZJC_CONTAINER_H__
#define __ZJC_CONTAINER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "ZJC_Stack.h"

//  - --- - --- - --- - --- -

typedef struct ZJC_CONTAINER {

    uint    objsize;
    cuchar* objname;

    Stack*  stack;
    uint*   ID_table;

    void*   buff;

} Container;

//  - --- - --- - --- - --- -

Container*  ZJC_buildcont       (uint       stacksize, uint tablesize, uint objsize, cuchar* objname);
void        ZJC_delcont         (Container* cont                                                    );

uint        ZJC_pushcont        (Container* cont,      uint id                                      );
uint        ZJC_cont_findLoc    (Container* cont,      uint id                                      );
uint        ZJC_cont_chkLoc     (Container* cont,      uint loc                                     );

//  - --- - --- - --- - --- -

#define ZJC_cont_find(cont, type, id) { uint loc = ZJC_cont_findLoc(cont, id); if(loc == 0) { return (void*) 0;         }\
                                        return ((type*) cont->buff) + loc;                                               \
                                                                                                                        }

#define ZJC_cont_get(cont, type, loc) { if(loc > cont->stack->size)                                                      \
                                      { printf("Location %u out of bounds for %s container.\n", loc, cont->objname); }   \
                                                                                                                         \
                                        else if( ((type*) cont->buff) + loc ) { return ((type*) cont->buff) + loc;      }\
                                        printf("Location %u points to an empty %s.\n", loc, cont->objname);             }\

//  - --- - --- - --- - --- -

#ifdef __cplusplus
}
#endif

#endif // __ZJC_CONTAINER_H__
