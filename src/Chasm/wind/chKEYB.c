#include "chKEYB.h"

//  - --- - --- - --- - --- -

void chKEYB_SETKEY (short* keys, uchar input)   { *keys |=  (1 << input);                                                }
void chKEYB_CLRKEY (short* keys, uchar input)   { *keys &= ~(1 << input);                                                }
int  chKEYB_GETKEY (short* keys, short value)   { return (*keys & value) == value;                                       }

//  - --- - --- - --- - --- -
