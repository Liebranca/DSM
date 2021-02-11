#ifndef __ZJC_STACK_H__
#define __ZJC_STACK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../ZJC_CommonTypes.h"

//  - --- - --- - --- - --- -

// A common LIFO stack to hold uints
typedef struct ZJC_STACK { 

    uint  size;                                 // Size of the stack
    uint  top;                                  // Ptr to top of the stack (Size to zero)
    uint* values;                               // Dynamic buffer for the stack's values

} Stack;

//  - --- - --- - --- - --- -

// Creates and returns a pointer to a new stack
Stack* ZJC_build_stack(uint statcksize);

// Deletes a stack
void ZJC_del_stack(Stack* stack);

// Push to stack
sint ZJC_push_stack(Stack* stack, uint value);

// Pop from stack
uint ZJC_pop_stack(Stack* stack);

//  - --- - --- - --- - --- -

#ifdef __cplusplus
}
#endif

#endif // __ZJC_STACK_H__
