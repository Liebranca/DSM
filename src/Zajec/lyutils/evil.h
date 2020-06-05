#ifndef ZAJEC_EVIL_H
#define ZAJEC_EVIL_H

//  - --- - --- - --- - --- -

#include <vector>

//  - --- - --- - --- - --- -

typedef unsigned int uint;
typedef unsigned char uchar;

//  - --- - --- - --- - --- -

static uint _i, _j, _k, _l;

#define vec_floop(x, y) for (x = 0; x < y.size(); x++)
#define vec_bloop(x, y) for (x = y.size() - 1; x > -1; x--)
#define arr_floop(x, y) for (x = 0; x < sizeof(y) / sizeof(y[0]); x++)
#define arr_bloop(x, y) for (x = (sizeof(y) / sizeof(y[0])) -1; x > -1; x--)

//  - --- - --- - --- - --- -

#endif // ZAJEC_EVIL_H
