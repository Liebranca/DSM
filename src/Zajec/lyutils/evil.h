#ifndef ZAJEC_EVIL_H
#define ZAJEC_EVIL_H

//  - --- - --- - --- - --- -

#include "../unsigdefs.h"
#include "../constdefs.h"

//  - --- - --- - --- - --- -

namespace zjc {

#define retchar(x) reinterpret_cast<char*>(x)
#define retcchar(x) reinterpret_cast<const char*>(x)

//  - --- - --- - --- - --- -

void terminate(int errorcode, concha* loc[]);

char* evil_shpath(const char* f);

void* evil_malloc(uint count, uint size, concha* loc[]);
void evil_free(void* buff);

}

#define WARD_EVIL_MALLOC(x, y) if (x == nullptr) { zjc::terminate(0x00, y); }

#define __HASHIT__(x) #x
#define HASHIT(x) __HASHIT__(x)

#define __LINESTRING__ HASHIT(__LINE__)

#define __ERRLOC__ { zjc::evil_shpath(__FILE__), ", on func ", __func__, " @line ", __LINESTRING__ }
#define $ERRLOC concha* ERRLOC[] = __ERRLOC__




#endif // ZAJEC_EVIL_H
