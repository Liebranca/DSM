#include "ZJC_Stack.h"
#include "../lyutils/ZJC_Evil.h"

#include <stdio.h>

//  - --- - --- - --- - --- -

Stack* ZJC_build_stack(uint stacksize)          {

    Stack* stack  = NULL;

    WARD_EVIL_MALLOC(stack,         Stack, sizeof(Stack), 1        );
    WARD_EVIL_MALLOC(stack->values, uint,  sizeof(uint ), stacksize);

    stack->size   = stacksize;
    stack->top    = 0;

    return stack;                                                                                                       }

void ZJC_del_stack(Stack* stack)                { WARD_EVIL_MFREE(stack->values); WARD_EVIL_MFREE(stack);               }

//  - --- - --- - --- - --- -

sint ZJC_push_stack(Stack* stack,
                    uint   value)               {

    if(stack->top >= stack->size)               { fprintf(stderr, "Can't push to a full stack\n"); return 0;            }

    stack->top++;
    stack->values[stack->top] = value;

    return 1;                                                                                                           }

uint ZJC_pop_stack(Stack* stack)                {

    if(stack->top <= 0)                         { fprintf(stderr, "Can't pop from an empty stack\n"); return 0;         }

    ushort value = stack->values[stack->top];
    stack->top--;

    return value+1;                                                                                                     }

//  - --- - --- - --- - --- -

