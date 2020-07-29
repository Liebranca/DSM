#ifndef __ZJC_STACK_H__
#define __ZJC_STACK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../ZJC_CommonTypes.h"
#include <stdint.h>

//  - --- - --- - --- - --- -

typedef struct SHORT_STACK { 

    size_t  size;
    ulong   top;
    ushort* values;

} sStack;

//  - --- - --- - --- - --- -

sStack* build_sStack(size_t size);
void    del_sStack  (sStack* stack);
int     sStack_push (sStack* stack, ushort value);
ushort  sStack_pop  (sStack* stack);

//  - --- - --- - --- - --- -

#ifdef __cplusplus
}
#endif

#endif // __ZJC_STACK_H__
