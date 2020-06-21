#include "ZJC_Stack.h"
#include "../lyutils/ZJC_Evil.h"

#include <stdio.h>

//  - --- - --- - --- - --- -

sStack build_sStack(ulong size)                 {

    sStack stack = {0};
    stack.size = size;
    stack.top  = 0;

    stack.values = (ushort*) evil_malloc(size, sizeof(ushort));

    return stack;                                                                                                       }

void   del_sStack  (sStack* stack)              { WARD_EVIL_MFREE(stack->values);                                       }

//  - --- - --- - --- - --- -

int    sStack_push (sStack* stack,
                    ushort value)               {

    if(stack->top >= stack->size)               { fprintf(stderr, "Can't push to a full stack\n"); return 0;            }

    stack->top++;
    stack->values[stack->top] = value;

    return 1;                                                                                                           }

ushort sStack_pop  (sStack* stack)              {

    if(stack->top <= 0)                         { fprintf(stderr, "Can't pop from an empty stack\n"); return 0;         }

    ushort value = stack->values[stack->top];
    stack->top--;

    return value+1;                                                                                                     }

//  - --- - --- - --- - --- -

