#ifndef __ZJC_STACK_H__
#define __ZJC_STACK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../ZJC_CommonTypes.h"

//  - --- - --- - --- - --- -

typedef struct ZJC_STACK { 

    uint  size;
    uint  top;
    uint* values;

} Stack;

//  - --- - --- - --- - --- -

Stack* ZJC_build_stack   (uint statcksize           );
void   ZJC_del_stack     (Stack* stack              );
int    ZJC_push_stack    (Stack* stack, uint value  );
uint   ZJC_pop_stack     (Stack* stack              );

//  - --- - --- - --- - --- -

#ifdef __cplusplus
}
#endif

#endif // __ZJC_STACK_H__
