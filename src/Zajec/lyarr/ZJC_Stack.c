#include "ZJC_Stack.h"
#include "../lyutils/ZJC_Evil.h"

#include <stdio.h>

//  - --- - --- - --- - --- -

sStack* build_sStack(ulong size)                {

    sStack* stack  = (sStack*) evil_malloc(1, sizeof(sStack));
    stack->size    = size;
    stack->top     = 0;

    stack->values  = NULL;
    stack->values  = build_usArray(size);

    return stack;                                                                                                       }

void   del_sStack  (sStack* stack)              { del_usArray(stack->values); WARD_EVIL_MFREE(stack);                   }

//  - --- - --- - --- - --- -

int    sStack_push (sStack* stack,
                    ushort value)               {

    if(stack->top >= stack->size)               { fprintf(stderr, "Can't push to a full stack\n"); return 0;            }

    stack->top++;
    stack->values->buff[stack->top] = value;

    return 1;                                                                                                           }

ushort sStack_pop  (sStack* stack)              {

    if(stack->top <= 0)                         { fprintf(stderr, "Can't pop from an empty stack\n"); return 0;         }

    ushort value = stack->values->buff[stack->top];
    stack->top--;

    return value+1;                                                                                                     }

//  - --- - --- - --- - --- -

