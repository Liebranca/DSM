#ifndef ZAJEC_EVIL_H
#define ZAJEC_EVIL_H

//  - --- - --- - --- - --- -

#include <vector>

//  - --- - --- - --- - --- -

typedef unsigned int uint;
typedef unsigned char uchar;

//  - --- - --- - --- - --- -

// Reserved uint for macro loops
static uint _i, _j, _k, _l;

// Vector forward loop
#define vec_floop(x, y) for (x = 0; x < y.size(); x++)

// Vector backwards loop
#define vec_bloop(x, y) for (x = y.size() - 1; x > -1; x--)

// Array forward loop 
#define arr_floop(x, y) for (x = 0; x < sizeof(y) / sizeof(y[0]); x++)

// Array backwards loop
#define arr_bloop(x, y) for (x = (sizeof(y) / sizeof(y[0])) -1; x > -1; x--)

//  - --- - --- - --- - --- -

#endif // ZAJEC_EVIL_H
