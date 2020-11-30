#ifndef __ZJC_CONTAINER_H__
#define __ZJC_CONTAINER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "ZJC_Stack.h"
#include "ZJC_Hash.h"

//  - --- - --- - --- - --- -

typedef struct ZJC_CONTAINER {

    uint    objsize;
    cuchar* objname;

    Stack*  stack;
    HASH*   hash;

    void*   buff;

} Container;

//  - --- - --- - --- - --- -

Container*  ZJC_build_cont      (uint       tablesize, uint objsize, uint maxvalue, cuchar* objname );
void        ZJC_del_cont        (Container* cont                                                    );

uint        ZJC_push_cont       (Container* cont,      uint id                                      );
uint        ZJC_pop_cont        (Container* cont,      uint id                                      );
uint        ZJC_findLoc_cont    (Container* cont,      uint id     , int shutit                     );

//  - --- - --- - --- - --- -

#define ZJC_findItem_cont(cont, type, id, mute) {                                                                        \
                                                                                                                         \
    if(id)                                      { uint loc = ZJC_findLoc_cont(cont, id, mute);                           \
                                                  if(!loc) { return (void*) 0; }                                         \
                                                  return ((type*) cont->buff) + loc;                                    }\
                                                                                                                         \
    return (void*) 0;                                                                                                   }

#define ZJC_getItem_cont(cont, type, loc)       {                                                                        \
                                                                                                                         \
    if(loc > cont->stack->size || !loc)         { printf("Location %u out of bounds for %s container.\n",                \
                                                         loc, cont->objname);                                           }\
                                                                                                                         \
    else if(((type*) cont->buff) + loc )        { return ((type*) cont->buff) + loc;                                    }\
                                                                                                                         \
    printf("Location %u points to an empty %s.\n", loc, cont->objname);                                                  \
                                                                                                                         \
    return (void*) 0;                                                                                                   }

//  - --- - --- - --- - --- -

#ifdef __cplusplus
}
#endif

#endif // __ZJC_CONTAINER_H__
