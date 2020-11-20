#include "ZJC_Stack.h"
#include "../lyutils/ZJC_Evil.h"

#include <stdio.h>

//  - --- - --- - --- - --- -

Stack* ZJC_buildstack(uint stacksize)           {

    Stack* stack  = (Stack*) evil_malloc(1,         sizeof(Stack));

    stack->values = (uint* ) evil_malloc(stacksize, sizeof(uint ));

    stack->size   = stacksize;
    stack->top    = 0;

    return stack;                                                                                                       }

void ZJC_delstack(Stack* stack)                 { WARD_EVIL_MFREE(stack->values); WARD_EVIL_MFREE(stack);               }

//  - --- - --- - --- - --- -

int ZJC_pushstack(Stack* stack,
                  uint   value)                 {

    if(stack->top >= stack->size)               { fprintf(stderr, "Can't push to a full stack\n"); return 0;            }

    stack->top++;
    stack->values[stack->top] = value;

    return 1;                                                                                                           }

uint ZJC_popstack(Stack* stack)                 {

    if(stack->top <= 0)                         { fprintf(stderr, "Can't pop from an empty stack\n"); return 0;         }

    ushort value = stack->values[stack->top];
    stack->top--;

    return value+1;                                                                                                     }

//  - --- - --- - --- - --- -

