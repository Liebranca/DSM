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

Stack* ZJC_buildstack   (uint statcksize           );
void   ZJC_delstack     (Stack* stack              );
int    ZJC_pushstack    (Stack* stack, uint value  );
uint   ZJC_popstack     (Stack* stack              );

//  - --- - --- - --- - --- -

#ifdef __cplusplus
}
#endif

#endif // __ZJC_STACK_H__
